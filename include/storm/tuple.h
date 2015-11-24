// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.
#ifndef CCSTORM_TUPLE_H
#define CCSTORM_TUPLE_H

#include <string>
#include "json/json.h"

namespace storm {

class Tuple {
public:
    Tuple(const std::string &id,
          const std::string &component,
          const std::string &stream,
          int task,
          const Json::Value &values
    ): _id(id), _component(component), _stream(stream), _task(task), _values(values) {}

    const std::string &id() const { return _id; }
    const std::string &component() const { return _component; }
    const std::string &stream() const { return _stream; }
    int task() const { return _task; }
    const Json::Value &values() const { return _values; }
    Json::Value &mutable_values() { return _values; }

private:
    std::string _id;
    std::string _component;
    std::string _stream;
    int _task;
    Json::Value _values;
};

}  // namespace storm

#endif  // CCSTORM_TUPLE_H
