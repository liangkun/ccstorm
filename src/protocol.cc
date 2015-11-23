// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include "storm/internal/protocol.h"
#include <string>
#include <memory>
#include <cstdlib>
#include "rapidjson/document.h"
#include "storm/exceptions.h"

using std::atoi;
using std::string;
using std::map;
using std::unique_ptr;
using rapidjson::Document;
using rapidjson::Value;

namespace storm { namespace internal { namespace protocol {

unique_ptr<TopologyContext> ParseTopologyContext(const string &jstr) {
    Document doc;
    doc.Parse(jstr.c_str());
    if (doc.HasParseError()) {
        throw ProtocolException(doc.GetParseError(), "failed to parse initial handshake: " + jstr);
    }

    if (!doc.HasMember("context") || !doc.HasMember("pidDir") || !doc.HasMember("conf")) {
        throw ProtocolException("missing context, pidDir or conf field(s) in initial handshake");
    }

    Value &jcontext = doc["context"];
    if (!jcontext.HasMember("task->component") || !jcontext.HasMember("taskid")) {
        throw ProtocolException("missing task->component or task_id in context");
    }
    Value &jtask_2_component = jcontext["task->component"];
    int task_id = jcontext["taskid"].GetInt();
    map<int, string> task_2_component;
    for (auto iter = jtask_2_component.MemberBegin(); iter != jtask_2_component.MemberEnd(); ++iter) {
        int id = atoi(iter->name.GetString());
        task_2_component[id] = iter->value.GetString();
    }

    string pid_dir = doc["pidDir"].GetString();

    return unique_ptr<TopologyContext>(new TopologyContext(task_id, task_2_component, pid_dir));
}

}}}  // namespace storm::internal::protocol
