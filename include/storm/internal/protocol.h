// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.
#ifndef CCSTORM_PROTOCOL_H
#define CCSTORM_PROTOCOL_H

#include <iostream>
#include <string>
#include "json/json.h"
#include "storm/topology-context.h"

namespace storm { namespace internal { namespace protocol {

// Read and parse an input json message
Json::Value NextMessage(std::istream &is);

void EmitMessage(const Json::Value &root, std::ostream &os);

// Finish initial handshake between ShellComponent and C++ Component.
// Caller will have the ownership of the result TopologyContext.
TopologyContext *InitialHandshake(std::istream &is, std::ostream &os);

// Caller will have the ownership of the result TopologyContext.
TopologyContext *ParseTopologyContext(Json::Value &root);

}}}  // namespace storm::internal::protocol

#endif //CCSTORM_PROTOCOL_H
