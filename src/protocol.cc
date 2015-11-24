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

TopologyContext *InitialHandshake(istream &is) {
    Value message = NextMessage(is);
    return ParseTopologyContext(message);
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
    string pid_dir{ root["pidDir"].asString() };

    // parse config
    Value &config = root["conf"];

    return new TopologyContext(taskid, &task_2_component, &pid_dir, &config);
}

}}}  // namespace storm::internal::protocol
