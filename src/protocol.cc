// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include "storm/internal/protocol.h"
#include <cstdlib>
#include <map>
#include <string>
#include "rapidjson/document.h"
#include "storm/exception.h"

using std::atoi;
using std::map;
using std::string;
using rapidjson::Document;
using rapidjson::Value;

namespace storm { namespace internal { namespace protocol {

TopologyContext *ParseTopologyContext(const string &jstr) {
    Document doc;
    doc.Parse(jstr.c_str());
    if (doc.HasParseError()) {
        throw ProtocolException(doc.GetParseError(), "failed to parse initial handshake: " + jstr);
    }

    // parse context
    Value &context = doc["context"];
    int taskid = context["taskid"].GetInt();
    Value &components = context["task->component"];
    map<int, string> task_2_component;
    for (auto iter = components.MemberBegin(); iter != components.MemberEnd(); ++iter) {
        int id = atoi(iter->name.GetString());
        task_2_component[id] = iter->value.GetString();
    }

    // parse pidDir
    string pid_dir = doc["pidDir"].GetString();

    // parse config
    Value &config = doc["conf"];

    return new TopologyContext(taskid, &task_2_component, &pid_dir, &config);
}

}}}  // namespace storm::internal::protocol
