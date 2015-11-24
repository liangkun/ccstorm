// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.
#ifndef CCSTORM_PROTOCOL_H
#define CCSTORM_PROTOCOL_H

#include <iostream>
#include <string>
#include "storm/topology-context.h"

namespace storm { namespace internal { namespace protocol {

// Read an input json message

// Parse initial handshake between ShellComponent and C++ Component.
TopologyContext *InitialHandshake(std::istream &input);

// Caller will have the ownership of result.
TopologyContext *ParseTopologyContext(const std::string &jstr);

}}}  // namespace storm::internal::protocol

#endif //CCSTORM_PROTOCOL_H
