// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.
#ifndef CCSTORM_RICH_BOLT_H
#define CCSTORM_RICH_BOLT_H

#include <iostream>
#include <memory>
#include "json/json.h"
#include "storm/component.h"
#include "storm/output-collector.h"

namespace storm {

class Bolt: public Component {
public:
    Bolt(std::istream &is, std::ostream &os): Component(is, os), _oc(os) {}
    virtual ~Bolt() {}

    virtual void Run() {
        Component::Run();
        Prepare();

        while(is().good()) {
            Json::Value message = internal::protocol::NextMessage(is());
            _tuple.reset(internal::protocol::ParseTuple(message));
            if (_tuple->stream() == "__heartbeat") {
                internal::protocol::EmitSync(os());
            } else {
                Execute(*_tuple);
            }
        }
    }

    OutputCollector &oc() { return _oc; }

    virtual void Prepare() {}
    virtual void Execute(Tuple &input) {}

private:
    OutputCollector _oc;
    std::unique_ptr<Tuple> _tuple;
};

}  // namespace storm

#endif  // CCSTORM_RICH_BOLT_H
