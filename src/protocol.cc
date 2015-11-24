// Copyright (c) 2015 ShuMei Inc. All rights reserved.
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
#include "json/json.h"
#include "storm/exception.h"

using std::atoi;
using std::getline;
using std::istream;
using std::ostream;
using std::stringstream;
using std::ofstream;
using std::endl;
using std::map;
using std::string;
using std::unique_ptr;
using Json::Reader;
using Json::FastWriter;
using Json::Value;

namespace storm { namespace internal { namespace protocol {

Value NextMessage(istream &is) {
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

    Value result;
    Reader reader;
    if (!reader.parse(jmessage, result)) {
        throw ProtocolException(reader.getFormattedErrorMessages());
    }
    return result;
}

void EmitMessage(const Json::Value &root, ostream &os) {
    FastWriter writer;
    os << writer.write(root) << "end" << endl;
    if (os.bad()) {
        stringstream err;
        err << "failed to send message: " << root;
        throw ProtocolException(err.str());
    }
}

TopologyContext *InitialHandshake(istream &is, ostream &os) {
    Value message = NextMessage(is);
    unique_ptr<TopologyContext> tc{ ParseTopologyContext(message) };
    pid_t current_pid = getpid();

    // create pid file
    stringstream pid_file_path;
    pid_file_path << tc->pid_dir() << "/" << current_pid;
    ofstream pid_file(pid_file_path.str());

    // send pid to os
    Value pid_message;
    pid_message["pid"] = current_pid;
    EmitMessage(pid_message, os);

    return tc.release();
}

TopologyContext *ParseTopologyContext(Value &root) {
    // parse context
    Value context = root["context"];
    int taskid = context["taskid"].asInt();
    Value &components = context["task->component"];
    map<int, string> task_2_component;
    for (auto iter = components.begin(); iter != components.end(); ++iter) {
        int id = atoi(iter.name().c_str());
        task_2_component[id] = iter->asString();
    }

    // parse pidDir
    string pid_dir = root["pidDir"].asString();

    // parse config
    Value config = root["conf"];

    return new TopologyContext(taskid, &task_2_component, &pid_dir, &config);
}

}}}  // namespace storm::internal::protocol
