// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include <cstdlib>
#include <memory>
#include <fstream>
#include <sstream>
#include <vector>
#include <utility>
#include "gtest/gtest.h"
#include "storm/internal/json.h"
#include "storm/exception.h"
#include "storm/tuple.h"
#include "storm/internal/protocol.h"

using std::remove;
using std::stringstream;
using std::string;
using std::unique_ptr;
using std::vector;
using std::move;
using storm::TopologyContext;
using storm::ProtocolException;
using storm::Value;
using storm::Values;
using storm::g_CrtAllocator;
using storm::Tuple;
using namespace storm::internal::protocol;
namespace json = storm::internal::json;

TEST(ProtocolTest, NextMessage_Legal) {
    stringstream ss;
    ss << "{\"count\":12}\nend\n"
       << "{\"object\":{\"message\":\"hello, world\"}}\nend\n";

    json::Value message = NextMessage(ss);
    ASSERT_EQ(12, message["count"].GetInt());

    message = NextMessage(ss);
    ASSERT_STREQ("hello, world", message["object"]["message"].GetString());
}

TEST(ProtocolTest, NextMessage_Illegal) {
    stringstream no_end;
    no_end << "{\"count\":12}\n";
    ASSERT_THROW(NextMessage(no_end), ProtocolException);

    stringstream illegal_json;
    illegal_json << "{abc:\nend\n";
    ASSERT_THROW(NextMessage(illegal_json), ProtocolException);
}

TEST(ProtocolTest, EmitMessage) {
    json::Value root;
    root.SetObject();
    root.AddMember("count", json::Value(1), json::g_CrtAllocator);

    stringstream ss;
    EmitMessage(root, ss);
    ASSERT_EQ(root, NextMessage(ss));

    root.AddMember("object", json::Value(), json::g_CrtAllocator);
    root["object"].SetObject();
    root["object"].AddMember("message", "hello, world", json::g_CrtAllocator);
    EmitMessage(root, ss);
    ASSERT_EQ(root, NextMessage(ss));
}

TEST(ProtocolTest, EmitSync) {
    stringstream ss;
    EmitSync(ss);
    json::Value msg = NextMessage(ss);

    ASSERT_STREQ("sync", msg["command"].GetString());
}

TEST(ProtocolTest, EmitAck) {
    stringstream ss;
    EmitAck("12335", ss);
    json::Value msg = NextMessage(ss);

    ASSERT_STREQ("ack", msg["command"].GetString());
    ASSERT_STREQ("12335", msg["id"].GetString());
}

TEST(ProtocolTest, EmitFail) {
    stringstream ss;
    EmitFail("12335", ss);
    json::Value msg = NextMessage(ss);

    ASSERT_STREQ("fail", msg["command"].GetString());
    ASSERT_STREQ("12335", msg["id"].GetString());
}

TEST(ProtocolTest, EmitTuple_SingleAnchor) {
    stringstream ss;
    Values values;
    values.SetArray();
    values.PushBack("hello", g_CrtAllocator);
    values.PushBack(Value(1), g_CrtAllocator);
    values.PushBack("world", g_CrtAllocator);

    // no anchors
    Values output0(values, g_CrtAllocator);
    EmitTuple("mystream", nullptr, &output0, ss);
    json::Value msg_no_anchor = NextMessage(ss);
    ASSERT_STREQ("emit", msg_no_anchor["command"].GetString());
    ASSERT_FALSE(msg_no_anchor.HasMember("anchors"));
    ASSERT_STREQ("mystream", msg_no_anchor["stream"].GetString());
    ASSERT_EQ(values, msg_no_anchor["tuple"]);

    // with anchors
    Values output1(values, g_CrtAllocator);
    unique_ptr<Tuple> an { new Tuple("12330", "an1", "default", 1, Values()) };
    EmitTuple("mystream", an.get(), &output1, ss);
    json::Value msg_with_anchor = NextMessage(ss);
    ASSERT_STREQ("emit", msg_no_anchor["command"].GetString());
    ASSERT_STREQ("mystream", msg_no_anchor["stream"].GetString());
    ASSERT_EQ(values, msg_no_anchor["tuple"]);
    ASSERT_STREQ("12330", msg_with_anchor["anchors"][0].GetString());
}

TEST(ProtocolTest, EmitTuple_MultiAnchor) {
    stringstream ss;
    Values values;
    values.SetArray();
    values.PushBack("hello", g_CrtAllocator);
    values.PushBack(Value(1), g_CrtAllocator);
    values.PushBack("world", g_CrtAllocator);

    // no anchors
    Values output0(values, g_CrtAllocator);
    vector<const Tuple*> anchors;
    EmitTuple("mystream", anchors, &output0, ss);
    json::Value msg_no_anchor = NextMessage(ss);
    ASSERT_STREQ("emit", msg_no_anchor["command"].GetString());
    ASSERT_FALSE(msg_no_anchor.HasMember("anchors"));
    ASSERT_STREQ("mystream", msg_no_anchor["stream"].GetString());
    ASSERT_EQ(values, msg_no_anchor["tuple"]);

    // with anchors
    Values output1(values, g_CrtAllocator);
    unique_ptr<Tuple> an0 { new Tuple("12330", "an1", "default", 1, Values()) };
    unique_ptr<Tuple> an1 { new Tuple("12331", "an1", "default", 1, Values()) };
    unique_ptr<Tuple> an2 { new Tuple("12332", "an1", "default", 1, Values()) };
    anchors.push_back(an0.get());
    anchors.push_back(an1.get());
    anchors.push_back(an2.get());
    EmitTuple("mystream", anchors, &output1, ss);
    json::Value msg_with_anchor = NextMessage(ss);
    ASSERT_STREQ("emit", msg_no_anchor["command"].GetString());
    ASSERT_STREQ("mystream", msg_no_anchor["stream"].GetString());
    ASSERT_EQ(values, msg_no_anchor["tuple"]);
    ASSERT_STREQ("12330", msg_with_anchor["anchors"][0].GetString());
    ASSERT_STREQ("12331", msg_with_anchor["anchors"][1].GetString());
    ASSERT_STREQ("12332", msg_with_anchor["anchors"][2].GetString());
}

TEST(ProtocolTest, EmitLog) {
    stringstream ss;
    EmitLog("hello, world", ss);
    json::Value msg = NextMessage(ss);

    ASSERT_STREQ("log", msg["command"].GetString());
    ASSERT_STREQ("hello, world", msg["msg"].GetString());
}

TEST(ProtocolTest, ParseTopologyContext_Legal) {
    const string legal {
        "{\"pidDir\":\"\\/path\\/to\\/pids\","
            "\"context\":{"
                "\"task->component\":{"
                "\"1\":\"__acker\","
                "\"2\":\"kafka-reader\","
                "\"3\":\"rules-matcher\","
                "\"4\":\"sentry-query\","
                "\"5\":\"sentry-query-prepare\"},"
                "\"taskid\":3},"
            "\"conf\":{\"storm.id\":\"test-topology\"}}"
    };
    json::Document document(&json::g_CrtAllocator);
    document.Parse(legal.c_str());
    ASSERT_FALSE(document.HasParseError());

    json::Value root;
    root.Swap(document);

    unique_ptr<TopologyContext> tc{ ParseTopologyContext(&root) };

    ASSERT_EQ(3, tc->task_id());
    ASSERT_STREQ("/path/to/pids", tc->pid_dir().c_str());
    ASSERT_EQ(5u, tc->task_2_component().size());
    ASSERT_STREQ("kafka-reader", tc->task_2_component().at(2).c_str());
    ASSERT_STREQ("sentry-query", tc->task_2_component().at(4).c_str());
    ASSERT_STREQ("rules-matcher", tc->component().c_str());
    ASSERT_STREQ("test-topology", tc->config()["storm.id"].GetString());
}

TEST(ProtocolTest, InitialHandshake_Legal) {
    const string legal {
            "{\"pidDir\":\"\\/tmp\","
                    "\"context\":{"
                    "\"task->component\":{"
                    "\"1\":\"__acker\","
                    "\"2\":\"kafka-reader\","
                    "\"3\":\"rules-matcher\","
                    "\"4\":\"sentry-query\","
                    "\"5\":\"sentry-query-prepare\"},"
                    "\"taskid\":3},"
                    "\"conf\":{\"storm.id\":\"test-topology\"}}"
    };
    stringstream ss;
    ss << legal << "\n" << "end" << "\n";
    stringstream os;

    unique_ptr<TopologyContext> tc{ InitialHandshake(ss, os) };
    ASSERT_EQ(3, tc->task_id());
    ASSERT_STREQ("/tmp", tc->pid_dir().c_str());
    ASSERT_EQ(5u, tc->task_2_component().size());
    ASSERT_STREQ("kafka-reader", tc->task_2_component().at(2).c_str());
    ASSERT_STREQ("sentry-query", tc->task_2_component().at(4).c_str());
    ASSERT_STREQ("rules-matcher", tc->component().c_str());
    ASSERT_STREQ("test-topology", tc->config()["storm.id"].GetString());

    json::Value jpid = NextMessage(os);
    ASSERT_TRUE(jpid.HasMember("pid"));
    ASSERT_TRUE(jpid["pid"].IsInt());

    stringstream pid_file_path;
    pid_file_path << "/tmp/" << jpid["pid"].GetInt();
    ASSERT_EQ(0, remove(pid_file_path.str().c_str()));
}

TEST(ProtocolTest, ParseTuple_Hearbeat) {
    const string heartbeat = {
            "{"
                    "\"comp\":null,"
                    "\"tuple\":[],"
                    "\"task\":-1,"
                    "\"stream\":\"__heartbeat\","
                    "\"id\":\"-8158762702567970975\""
            "}"
    };
    stringstream ss;
    ss << heartbeat << "\n" << "end" << "\n";
    json::Value root = NextMessage(ss);
    unique_ptr<Tuple> tuple{ ParseTuple(&root) };

    ASSERT_STREQ("-8158762702567970975", tuple->id().c_str());
    ASSERT_STREQ("", tuple->component().c_str());
    ASSERT_STREQ("__heartbeat", tuple->stream().c_str());
    ASSERT_EQ(-1, tuple->task());
    Values values;
    values.SetArray();
    ASSERT_EQ(values, tuple->values());

}
