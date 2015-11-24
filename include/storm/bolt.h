// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.
#ifndef CCSTORM_RICH_BOLT_H
#define CCSTORM_RICH_BOLT_H

#include <iostream>
#include "storm/component.h"
#include "storm/output-collector.h"

namespace storm {

class Bolt: public Component {
public:
    Bolt(std::istream &is, std::ostream &os): Component(is, os) {}
    virtual ~Bolt() {}

    virtual void Run() {
        Component::Run();
    }

    virtual void Prepare(const TopologyContext &tc, OutputCollector &collector) {}
    virtual void Execute(Tuple &input) {}
private:

};

}  // namespace storm

#endif  // CCSTORM_RICH_BOLT_H
