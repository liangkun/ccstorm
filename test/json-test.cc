// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include <string>
#include "storm/internal/json.h"
#include "gtest/gtest.h"

using std::string;
using namespace storm::internal::json;

TEST(RapidJsonTest, ParsingWithGlobalCrtAllocator) {
    const string json { "{\n"
                                "    \"hello\": \"world\",\n"
                                "    \"t\": true ,\n"
                                "    \"f\": false,\n"
                                "    \"n\": null,\n"
                                "    \"i\": 123,\n"
                                "    \"pi\": 3.1416,\n"
                                "    \"a\": [1, 2, 3, 4]\n"
                        "}" };
    Document document(&g_CrtAllocator);
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
