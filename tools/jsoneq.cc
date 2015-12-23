// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include <iostream>
#include "storm/json.h"

using namespace storm::json;
using namespace std;

int main(int argc, char **argv) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <json1> <json2>" << endl;
        return 1;
    }

    Document document1(&g_allocator);
    document1.Parse(argv[1]);
    if (document1.HasParseError()) {
        cerr << "first argument is not a valid json" << endl;
        return 1;
    }

    Document document2(&g_allocator);
    document2.Parse(argv[2]);
    if (document2.HasParseError()) {
        cerr << "second argument is not a valid json" << endl;
        return 1;
    }

    return !(document1 == document2);
}
