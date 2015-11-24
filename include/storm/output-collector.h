// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.
#ifndef CCSTORM_OUTPUT_COLLECTOR_H_H
#define CCSTORM_OUTPUT_COLLECTOR_H_H

#include <iostream>
#include "storm/internal/protocol.h"

namespace storm {

class OutputCollector {
public:
    OutputCollector(std::istream &is, std::ostream &os): _is(is), _os(os) {}

    void Ack(const Tuple &input) {
        internal::protocol::EmitAck(input.id(), _os);
    }

    void Fail(const Tuple &input) {
        internal::protocol::EmitFail(input.id(), _os);
    }
private:
    std::istream &_is;
    std::ostream &_os;
};

}  // namespace storm

#endif  // CCSTORM_OUTPUT_COLLECTOR_H_H
