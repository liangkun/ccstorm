// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#ifndef STORM_PROTOCOL_H
#define STORM_PROTOCOL_H

#include <iostream>
#include <string>
#include <vector>
#include "storm/internal/json.h"
#include "storm/topology-context.h"
#include "storm/tuple.h"

namespace storm { namespace internal { namespace protocol {

// Following functions implements storm's multi-lang protocol.
//
// See (multi-lang protocol)[https://storm.apache.org/documentation/Multilang-protocol.html] for
// details.

// Read and parse next input json message.
json::Value NextMessage(std::istream &is);

// Emit a json message to parent process, with the "end" line.
void EmitMessage(const json::Value &root, std::ostream &os);

// Emit "sync" message.
void EmitSync(std::ostream &os);

// Emit "ack" message.
void EmitAck(const std::string &id, std::ostream &os);

// Emit "fail" message.
void EmitFail(const std::string &id, std::ostream &os);

// Emit a tuple(send "emit" message), data of output will be stolen.
void EmitTuple(const std::string &stream, const Tuple *anchor, Values *output, std::ostream &os);

// Emit a tuple(send "emit" message), data of output will be stolen.
void EmitTuple(
        const std::string &stream,
        const std::vector<const Tuple*> &anchors,
        Values *output,
        std::ostream &os);

// Emit "log" message.
void EmitLog(const std::string &log, std::ostream &os);

// Finish initial handshake between ShellComponent and C++ Component.
// Caller will have the ownership of the result TopologyContext.
TopologyContext *InitialHandshake(std::istream &is, std::ostream &os);

// Caller will have the ownership of the result TopologyContext.
// Note: data in root json object will be stolen.
TopologyContext *ParseTopologyContext(json::Value *root);

// Caller will have the ownership of the result Tuple.
// Note: data in *root json object will be stolen.
Tuple *ParseTuple(json::Value *root);

}}}  // namespace storm::internal::protocol

#endif // STORM_PROTOCOL_H
