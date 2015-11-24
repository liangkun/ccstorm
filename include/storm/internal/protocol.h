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

// Parse initial handshake between ShellComponent and C++ Component.
TopologyContext *InitialHandshake(std::istream &is);

// Caller will have the ownership of result.
TopologyContext *ParseTopologyContext(Json::Value &root);

}}}  // namespace storm::internal::protocol

#endif //CCSTORM_PROTOCOL_H
