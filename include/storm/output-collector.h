// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.
#ifndef CCSTORM_OUTPUT_COLLECTOR_H_H
#define CCSTORM_OUTPUT_COLLECTOR_H_H

#include <iostream>
#include <string>
#include <vector>
#include "storm/internal/protocol.h"

namespace storm {

class OutputCollector {
public:
    OutputCollector(std::ostream &os): _os(os) {}

    void Ack(const Tuple &input) {
        internal::protocol::EmitAck(input.id(), _os);
    }

    void Fail(const Tuple &input) {
        internal::protocol::EmitFail(input.id(), _os);
    }

    void Emit(const std::string &stream, const std::vector<const Tuple*> &anchors, const Tuple &output) {
        internal::protocol::EmitTuple(stream, anchors, output, _os);
    }

private:
    std::ostream &_os;
};

}  // namespace storm

#endif  // CCSTORM_OUTPUT_COLLECTOR_H_H
