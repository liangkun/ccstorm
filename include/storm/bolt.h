// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#ifndef STORM_RICH_BOLT_H
#define STORM_RICH_BOLT_H

#include <iostream>
#include <memory>
#include "storm/json.h"
#include "storm/component.h"
#include "storm/output-collector.h"

namespace storm {

class Bolt: public Component {
public:
    Bolt(): Component() {}  // default constructor
    virtual ~Bolt() {}

    virtual void Run(std::istream &istream, std::ostream &ostream) {
        Component::Run(istream, ostream);
        _oc.reset(new OutputCollector(os()));
        Prepare();

        while(is().good()) {
            json::Value message { internal::protocol::NextMessage(is()) };
            if (message.IsArray()) {
                continue;  // ignore receiving taskids for now
            }
            _tuple.reset(internal::protocol::ParseTuple(&message));
            if (_tuple->stream() == "__heartbeat") {
                internal::protocol::EmitSync(os());
            } else {
                Execute(_tuple);
            }
        }
    }

protected:
    OutputCollector &oc() { return *_oc; }

    virtual void Prepare() {}

    // Execute() can do anything to input tuple.
    virtual void Execute(std::unique_ptr<Tuple> &input) {}

private:
    std::unique_ptr<OutputCollector> _oc;
    std::unique_ptr<Tuple> _tuple;
};

}  // namespace storm

#endif  // STORM_RICH_BOLT_H
