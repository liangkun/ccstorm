// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.
#ifndef CCSTORM_ICOMPONENT_H
#define CCSTORM_ICOMPONENT_H

#include <memory>
#include <iostream>
#include "storm/topology-context.h"
#include "storm/internal/protocol.h"

namespace storm {

// Base class of storm components.
class Component {
public:
    Component(std::istream &is, std::ostream &os): _is(is), _os(os) {}
    virtual ~Component() {}

    virtual void Run() {
        _tc.reset(internal::protocol::InitialHandshake(_is, _os));
    }

    std::istream &is() { return _is; }
    std::ostream &os() { return _os; }

private:
    std::istream &_is;
    std::ostream &_os;
    std::unique_ptr<TopologyContext> _tc;
};

}

#endif //CCSTORM_ICOMPONENT_H
