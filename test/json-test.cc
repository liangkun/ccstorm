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

TEST(JsonTest, CheckPath_Legal) {
    Value empty;
    empty.SetArray();
    CheckPath(empty);

    Value names;
    names.SetArray();
    names.PushBack("a", g_allocator);
    names.PushBack("b", g_allocator);
    CheckPath(names);

    Value indices;
    indices.SetArray();
    indices.PushBack(Value(9), g_allocator);
    indices.PushBack(Value(1000), g_allocator);
    CheckPath(names);

    Value mixed;
    mixed.SetArray();
    mixed.PushBack("a", g_allocator);
    mixed.PushBack(Value(9), g_allocator);
    mixed.PushBack("b", g_allocator);
    CheckPath(mixed);
    ASSERT_TRUE(true);
}

TEST(JsonTest, CheckPath_Illegal) {
    Value obj;
    obj.SetObject();
    ASSERT_THROW(CheckPath(obj), runtime_error);

    Value double_element;
    double_element.SetArray();
    double_element.PushBack("a", g_allocator);
    double_element.PushBack(Value(3.14), g_allocator);
    ASSERT_THROW(CheckPath(double_element), runtime_error);

    Value obj_element;
    obj_element.SetArray();
    Value element;
    element.SetObject();
    obj_element.PushBack("a", g_allocator);
    obj_element.PushBack(element, g_allocator);
    ASSERT_THROW(CheckPath(obj_element), runtime_error);
}

TEST(JsonTest, PathToString_Legal) {
    Value empty;
    empty.SetArray();
    string empty_str = PathToString(empty);
    ASSERT_STREQ("$", empty_str.c_str());

    Value names;
    names.SetArray();
    names.PushBack("a", g_allocator);
    names.PushBack("b", g_allocator);
    string names_str = PathToString(names);
    ASSERT_STREQ("$.a.b", names_str.c_str());

    Value indices;
    indices.SetArray();
    indices.PushBack(Value(9), g_allocator);
    indices.PushBack(Value(1000), g_allocator);
    string indices_str = PathToString(indices);
    ASSERT_STREQ("$[9][1000]", indices_str.c_str());

    Value mixed;
    mixed.SetArray();
    mixed.PushBack("a", g_allocator);
    mixed.PushBack(Value(9), g_allocator);
    mixed.PushBack("b", g_allocator);
    string mixed_str = PathToString(mixed);
    ASSERT_STREQ("$.a[9].b", mixed_str.c_str());
}

TEST(JsonTest, PathToString_Illegal) {
    Value obj;
    obj.SetObject();
    ASSERT_THROW(PathToString(obj), runtime_error);

    Value double_element;
    double_element.SetArray();
    double_element.PushBack("a", g_allocator);
    double_element.PushBack(Value(3.14), g_allocator);
    ASSERT_THROW(PathToString(double_element), runtime_error);

    Value obj_element;
    obj_element.SetArray();
    Value element;
    element.SetObject();
    obj_element.PushBack("a", g_allocator);
    obj_element.PushBack(element, g_allocator);
    ASSERT_THROW(PathToString(obj_element), runtime_error);
}

TEST(JsonTest, GetValue_Legal) {
    const string json { "{\n"
                        "    \"hello\": \"world\",\n"
                        "    \"t\": true ,\n"
                        "    \"f\": false,\n"
                        "    \"n\": null,\n"
                        "    \"i\": 123,\n"
                        "    \"pi\": 3.1416,\n"
                        "    \"a\": [1, 2, 3, 4],\n"
                        "    \"b\":{\"c\":10, \"d\":[5, 6, 7]}"
                        "}" };
    Document document(&g_allocator);
    document.Parse(json.c_str());

    Value empty;
    empty.SetArray();
    ASSERT_EQ(&document, GetValue(document, empty));

    Value names;
    names.SetArray();
    names.PushBack("b", g_allocator);
    names.PushBack("c", g_allocator);
    ASSERT_EQ(10, GetValue(document, names)->GetInt());

    Value mixed;
    mixed.SetArray();
    mixed.PushBack("b", g_allocator);
    mixed.PushBack("d", g_allocator);
    mixed.PushBack(Value(1), g_allocator);
    ASSERT_EQ(6, GetValue(document, mixed)->GetInt());

    Value nonexists;
    nonexists.SetArray();
    nonexists.PushBack("a", g_allocator);
    nonexists.PushBack("b", g_allocator);
    ASSERT_EQ(nullptr, GetValue(document, nonexists));
}

TEST(JsonTest, GetValue_Illegal) {
    const string json { "{\n"
                        "    \"hello\": \"world\",\n"
                        "    \"t\": true ,\n"
                        "    \"f\": false,\n"
                        "    \"n\": null,\n"
                        "    \"i\": 123,\n"
                        "    \"pi\": 3.1416,\n"
                        "    \"a\": [1, 2, 3, 4],\n"
                        "    \"b\":{\"c\":10, \"d\":[5, 6, 7]}"
                        "}" };
    Document document(&g_allocator);
    document.Parse(json.c_str());

    Value obj;
    obj.SetObject();
    ASSERT_THROW(GetValue(document, obj), runtime_error);

    Value double_element;
    double_element.SetArray();
    double_element.PushBack("a", g_allocator);
    double_element.PushBack(Value(3.14), g_allocator);
    ASSERT_THROW(GetValue(document, double_element), runtime_error);

    Value obj_element;
    obj_element.SetArray();
    Value element;
    element.SetObject();
    obj_element.PushBack("a", g_allocator);
    obj_element.PushBack(element, g_allocator);
    ASSERT_THROW(GetValue(document, obj_element), runtime_error);
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
