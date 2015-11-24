// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include <iostream>
#include <sstream>
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
        stringstream ss;
        ss << tuple.values()[0].asInt() + tuple.values()[1].asInt();
        result.append(ss.str());
        result.append(tuple.values()[2]);
        Tuple output(result);
        oc().Emit("default", &tuple, output);
        oc().Ack(tuple);
    }

private:
};

}  // anonymous namespace

int main(void) {
    unique_ptr<Adder> adder { new Adder(cin, cout) };
    adder->Run();
    return 0;
}