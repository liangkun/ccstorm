// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#ifndef STORM_ICOMPONENT_H
#define STORM_ICOMPONENT_H

#include <memory>
#include <iostream>
#include "storm/exception.h"
#include "storm/topology-context.h"
#include "storm/internal/protocol.h"

namespace storm {

// Base class of storm components(Bolt or Spout).
class Component {
public:
    Component(): _is(nullptr), _os(nullptr) {}  // default constructor
    virtual ~Component() {}

    virtual void Run(std::istream &is, std::ostream &os) {
        _is = &is;
        _os = &os;
        _tc.reset(internal::protocol::InitialHandshake(is, os));
    }

    std::istream &is() { return *_is; }
    std::ostream &os() { return *_os; }
    const TopologyContext &tc() { return *_tc; }

protected:
    void Log(const std::string &msg) { internal::protocol::EmitLog(msg, *_os); }

private:
    // Component does not own _is and _os
    std::istream *_is;
    std::ostream *_os;
    std::unique_ptr<TopologyContext> _tc;
};

}

#endif // STORM_ICOMPONENT_H
