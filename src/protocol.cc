// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include "storm/internal/protocol.h"
#include <cstdlib>
#include <iostream>
#include <map>
#include <string>
#include "json/json.h"
#include "storm/exception.h"

using std::atoi;
using std::getline;
using std::istream;
using std::map;
using std::string;
using Json::Reader;
using Json::Value;

namespace storm { namespace internal { namespace protocol {

TopologyContext *InitialHandshake(istream &input) {
    string jstr;
    string line;
    while (true) {
        if (!getline(input, line).good()) {
            throw ProtocolException("initial handshake terminated too early");
        }
        if (line == "end") {
            break;
        }
        jstr += line;
        jstr += "\n";
    }

    return ParseTopologyContext(jstr);
}

TopologyContext *ParseTopologyContext(const string &jstr) {
    Value root;
    Reader reader;
    if (!reader.parse(jstr, root)) {
        throw ProtocolException("failed to parse initial handshake: " + jstr);
    }

    // parse context
    Value &context = root["context"];
    int taskid = context["taskid"].asInt();
    Value &components = context["task->component"];
    map<int, string> task_2_component;
    for (auto iter = components.begin(); iter != components.end(); ++iter) {
        int id = atoi(iter.name().c_str());
        task_2_component[id] = iter->asString();
    }

    // parse pidDir
    string pid_dir{ root["pidDir"].asString() };

    // parse config
    Value &config = root["conf"];

    return new TopologyContext(taskid, &task_2_component, &pid_dir, &config);
}

}}}  // namespace storm::internal::protocol
