// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#ifndef STORM_OUTPUT_COLLECTOR_H
#define STORM_OUTPUT_COLLECTOR_H

#include <iostream>
#include <string>
#include <vector>
#include "storm/values.h"
#include "storm/internal/protocol.h"

namespace storm {

class OutputCollector {
public:
    OutputCollector(std::ostream &os): _os(os) {}

    void Ack(const Tuple *input) {
        internal::protocol::EmitAck(input->id(), _os);
    }

    void Fail(const Tuple *input) {
        internal::protocol::EmitFail(input->id(), _os);
    }

    void Emit(const std::string &stream, const Tuple *anchor, Values *output) {
        internal::protocol::EmitTuple(stream, anchor, output, _os);
    }

    void Emit(const std::string &stream, const std::vector<const Tuple*> &anchors, Values *output) {
        internal::protocol::EmitTuple(stream, anchors, output, _os);
    }

private:
    std::ostream &_os;
};

}  // namespace storm

#endif  // STORM_OUTPUT_COLLECTOR_H
