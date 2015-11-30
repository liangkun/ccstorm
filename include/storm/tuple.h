// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#ifndef STORM_TUPLE_H
#define STORM_TUPLE_H

#include <string>
#include "storm/values.h"

namespace storm {

class Tuple {
public:
    Tuple(std::string &&id,
          std::string &&component,
          std::string &&stream,
          int task,
          Values &&values
    ): _id(id), _component(component), _stream(stream), _task(task) {
        _values = values;
    }

    Tuple(const std::string &id,
          const std::string &component,
          const std::string &stream,
          int task,
          Values &&values
    ): _id(id), _component(component), _stream(stream), _task(task) {
        _values = values;
    }

    Tuple(Values &&values): _id("0"), _component(""), _stream("default"), _task(0) {
        _values = values;
    }

    const std::string &id() const { return _id; }
    const std::string &component() const { return _component; }
    const std::string &stream() const { return _stream; }
    int task() const { return _task; }
    const Values &values() const { return _values; }
    Values &mutable_values() { return _values; }

private:
    std::string _id;
    std::string _component;
    std::string _stream;
    int _task;
    Values _values;
};

}  // namespace storm

#endif  // STORM_TUPLE_H
