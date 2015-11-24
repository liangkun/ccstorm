// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include <iostream>
#include <memory>
#include <vector>
#include "storm/storm.h"

using namespace std;
using namespace storm;
using namespace Json;

namespace {

class Adder: public Bolt {
public:
    using Bolt::Bolt;

    virtual void Execute(Tuple &tuple) {
        Value result;
        result.append(Value(tuple.values()[0].asInt() + tuple.values()[1].asInt()));
        _anchors.clear();
        _anchors.push_back(&tuple);
        Tuple output(result);
        oc().Emit("test", _anchors, output);
        oc().Ack(tuple);
    }

private:
    vector<const Tuple *> _anchors;
};

}  // anonymous namespace

int main(void) {
    unique_ptr<Adder> adder { new Adder(cin, cout) };
    adder->Run();
    return 0;
}