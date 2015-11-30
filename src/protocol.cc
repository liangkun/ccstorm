// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include "storm/internal/protocol.h"
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "storm/internal/json.h"
#include "storm/exception.h"

using std::atoi;
using std::getline;
using std::istream;
using std::ostream;
using std::ios;
using std::stringstream;
using std::ofstream;
using std::endl;
using std::map;
using std::move;
using std::string;
using std::unique_ptr;
using std::vector;
namespace json = storm::internal::json;

namespace storm { namespace internal { namespace protocol {

json::Value NextMessage(istream &is) {
    string jmessage;

    string line;
    while (is.good()) {
        if (!getline(is, line).good()) {
            throw ProtocolException("message istream terminated too early");
        }
        if (line == "end") {
            break;
        }
        jmessage += line;
        jmessage += "\n";
    }

#ifdef STORM_PROTOCOL_DEBUG
    ofstream ofs(STORM_PROTOCOL_DEBUG "/storm-protocol-inputs", ios::app);
    ofs << jmessage << endl;
#endif

    json::Document document(&json::g_CrtAllocator);
    document.Parse(jmessage.c_str());
    if (document.HasParseError()) {
        stringstream error;
        error << json::GetParseError(document.GetParseError())
              << " at position: "
              << document.GetErrorOffset();
        throw ProtocolException(error.str());
    }

    json::Value result;
    result.Swap(document);

    return result;
}

void EmitMessage(const json::Value &root, ostream &os) {
    json::StringBuffer buffer;
    json::Writer writer(buffer, &json::g_CrtAllocator);
    root.Accept(writer);
    os << buffer.GetString() << "\nend" << endl;

#ifdef STORM_PROTOCOL_DEBUG
    ofstream ofs(STORM_PROTOCOL_DEBUG "/storm-protocol-outputs", ios::app);
    ofs << buffer.GetString() << endl;
#endif

    if (os.bad()) {
        stringstream err;
        err << "failed to send message: " << buffer.GetString();
        throw ProtocolException(err.str());
    }
}

void EmitSync(ostream &os) {
    json::Value msg;
    msg.SetObject();
    msg.AddMember("command", "sync", json::g_CrtAllocator);
    EmitMessage(msg, os);
}

void EmitAck(const string &id, ostream &os) {
    json::Value msg;
    msg.SetObject();
    msg.AddMember("command", "ack", json::g_CrtAllocator);
    msg.AddMember("id", json::ToValue(id), json::g_CrtAllocator);
    EmitMessage(msg, os);
}

void EmitFail(const string &id, ostream &os) {
    json::Value msg;
    msg.SetObject();
    msg.AddMember("command", "fail", json::g_CrtAllocator);
    msg.AddMember("id", json::ToValue(id), json::g_CrtAllocator);
    EmitMessage(msg, os);
}

void EmitTuple(const string &stream, const Tuple *anchor, Values *output, ostream &os) {
    json::Value msg;
    msg.SetObject();
    msg.AddMember("command", "emit", json::g_CrtAllocator);
    msg.AddMember("stream", json::ToValue(stream), json::g_CrtAllocator);
    if (anchor != nullptr) {
        json::Value anchor_ids;
        anchor_ids.SetArray();
        anchor_ids.PushBack(json::ToValue(anchor->id()), json::g_CrtAllocator);
        msg.AddMember("anchors", anchor_ids, json::g_CrtAllocator);
    }
    // Currently, Values is the same as json::Value
    msg.AddMember("tuple", *output, json::g_CrtAllocator);
    EmitMessage(msg, os);
}

void EmitTuple(const string &stream, const vector<const Tuple*> &anchors, Values *output, ostream &os) {
    json::Value msg;
    msg.SetObject();
    msg.AddMember("command", "emit", json::g_CrtAllocator);
    msg.AddMember("stream", json::ToValue(stream), json::g_CrtAllocator);
    if (!anchors.empty()) {
        json::Value anchor_ids;
        anchor_ids.SetArray();
        for (auto &tuple: anchors) {
            anchor_ids.PushBack(json::ToValue(tuple->id()), json::g_CrtAllocator);
        }
        msg.AddMember("anchors", anchor_ids, json::g_CrtAllocator);
    }
    msg.AddMember("tuple", *output, json::g_CrtAllocator);
    EmitMessage(msg, os);
}

void EmitLog(const string &log, ostream &os) {
    json::Value msg;
    msg.SetObject();
    msg.AddMember("command", "log", json::g_CrtAllocator);
    msg.AddMember("msg", json::ToValue(log), json::g_CrtAllocator);
    EmitMessage(msg, os);
}

TopologyContext *InitialHandshake(istream &is, ostream &os) {
    json::Value message = NextMessage(is);
    unique_ptr<TopologyContext> tc{ ParseTopologyContext(&message) };
    pid_t current_pid = getpid();

    // create pid file
    stringstream pid_file_path;
    pid_file_path << tc->pid_dir() << "/" << current_pid;
    ofstream pid_file(pid_file_path.str());

    // send pid to os
    json::Value pid_message;
    pid_message.SetObject();
    pid_message.AddMember("pid", json::Value(current_pid), json::g_CrtAllocator);
    EmitMessage(pid_message, os);

    return tc.release();
}

TopologyContext *ParseTopologyContext(json::Value *root) {
    // parse context
    json::Value &rootRef = *root;
    json::Value &context = rootRef["context"];
    int taskid = context["taskid"].GetInt();
    json::Value &components = context["task->component"];
    map<int, string> task_2_component;
    for (auto iter = components.MemberBegin(); iter != components.MemberEnd(); ++iter) {
        int id = atoi(iter->name.GetString());
        task_2_component[id] = iter->value.GetString();
    }

    // parse pidDir
    string pid_dir = rootRef["pidDir"].GetString();

    // parse config
    json::Value &config = rootRef["conf"];

    return new TopologyContext(taskid, move(task_2_component), move(pid_dir), move(config));
}

Tuple *ParseTuple(json::Value *root) {
    json::Value &rootRef = *root;

    json::Value &idValue = rootRef["id"];
    string id{ idValue.GetString(), idValue.GetStringLength() };

    json::Value &compValue = rootRef["comp"];
    string comp{ compValue.IsNull()? "" : string(compValue.GetString(), compValue.GetStringLength()) };

    json::Value &streamValue = rootRef["stream"];
    string stream{ streamValue.GetString(), streamValue.GetStringLength() };

    int task = rootRef["task"].GetInt();

    return new Tuple(move(id), move(comp), move(stream), task, move(rootRef["tuple"]));
}

}}}  // namespace storm::internal::protocol
