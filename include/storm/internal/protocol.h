// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.
#ifndef CCSTORM_PROTOCOL_H
#define CCSTORM_PROTOCOL_H

#include <string>
#include "storm/topology-context.h"

namespace storm { namespace internal { namespace protocol {

// Caller will have the ownership of result.
TopologyContext *ParseTopologyContext(const std::string &jstr);

}}}  // namespace storm::internal::protocol

#endif //CCSTORM_PROTOCOL_H
