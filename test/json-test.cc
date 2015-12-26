// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include <stdexcept>
#include <string>
#include "storm/json.h"
#include "gtest/gtest.h"

using std::runtime_error;
using std::string;
using namespace storm::json;

TEST(JsonTest, ParsingWithGlobalCrtAllocator) {
    const string json { "{\n"
                                "    \"hello\": \"world\",\n"
                                "    \"t\": true ,\n"
                                "    \"f\": false,\n"
                                "    \"n\": null,\n"
                                "    \"i\": 123,\n"
                                "    \"pi\": 3.1416,\n"
                                "    \"a\": [1, 2, 3, 4]\n"
                        "}" };
    Document document(&g_allocator);
    document.Parse(json.c_str());
    ASSERT_FALSE(document.HasParseError());
    ASSERT_STREQ("world", document["hello"].GetString());
    ASSERT_TRUE(document["t"].GetBool());
    ASSERT_FALSE(document["f"].GetBool());
    ASSERT_TRUE(document["n"].IsNull());
    ASSERT_EQ(123, document["i"].GetInt());
    ASSERT_DOUBLE_EQ(3.1416, document["pi"].GetDouble());
    Value &a = document["a"];
    ASSERT_EQ(4, a.Size());
    for (int i = 0; i < a.Size(); ++i) {
        ASSERT_EQ(i + 1, a[i].GetInt());
    }

    ASSERT_NE(nullptr, dynamic_cast<CrtAllocator*>(&document.GetAllocator()));
}

TEST(JsonTest, ValueToString) {
    Value null;
    ASSERT_STREQ("null", ValueToString(null).c_str());

    Value empty_object;
    empty_object.SetObject();
    ASSERT_STREQ("{}", ValueToString(empty_object).c_str());

    Value empty_array;
    empty_array.SetArray();
    ASSERT_STREQ("[]", ValueToString(empty_array).c_str());

    const string json{ "{"
                               "\"hello\":\"world\","
                               "\"t\":true,"
                               "\"f\":false,"
                               "\"n\":null,"
                               "\"i\":123,"
                               "\"pi\":3.1416,"
                               "\"a\":[1,2,3,4],"
                               "\"b\":{\"c\":10,\"d\":[5,6,7]}"
                       "}" };
    Document document(&g_allocator);
    document.Parse(json.c_str());

    string njson = ValueToString(document);
    Document ndocument(&g_allocator);
    ndocument.Parse(njson.c_str());
    ASSERT_EQ(document, ndocument);
}
