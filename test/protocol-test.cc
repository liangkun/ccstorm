// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include <cstdlib>
#include <memory>
#include <fstream>
#include <sstream>
#include "gtest/gtest.h"
#include "json/json.h"
#include "storm/exception.h"
#include "storm/internal/protocol.h"

using std::remove;
using std::stringstream;
using std::string;
using std::unique_ptr;
using Json::Reader;
using Json::Value;
using storm::TopologyContext;
using storm::ProtocolException;
using namespace storm::internal::protocol;

TEST(ProtocolTest, NextMessage_Legal) {
    stringstream ss;
    ss << "{\"count\":12}\nend\n"
       << "{\"object\":{\"message\":\"hello, world\"}}\nend\n";

    Value message = NextMessage(ss);
    ASSERT_EQ(12, message["count"].asInt());

    message = NextMessage(ss);
    ASSERT_STREQ("hello, world", message["object"]["message"].asCString());
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
    Value root;
    root["count"] = 1;

    stringstream ss;
    EmitMessage(root, ss);
    ASSERT_EQ(root, NextMessage(ss));

    root["object"]["message"] = "hello, world";
    EmitMessage(root, ss);
    ASSERT_EQ(root, NextMessage(ss));
}

TEST(ProtocolTest, EmitSync) {
    stringstream ss;
    EmitSync(ss);
    Value msg = NextMessage(ss);

    ASSERT_STREQ("sync", msg["command"].asCString());
}

TEST(ProtocolTest, EmitAck) {
    stringstream ss;
    EmitAck("12335", ss);
    Value msg = NextMessage(ss);

    ASSERT_STREQ("ack", msg["command"].asCString());
    ASSERT_STREQ("12335", msg["id"].asCString());
}

TEST(ProtocolTest, EmitFail) {
    stringstream ss;
    EmitFail("12335", ss);
    Value msg = NextMessage(ss);

    ASSERT_STREQ("fail", msg["command"].asCString());
    ASSERT_STREQ("12335", msg["id"].asCString());
}

TEST(ProtocolTest, EmitLog) {
    stringstream ss;
    EmitLog("hello, world", ss);
    Value msg = NextMessage(ss);

    ASSERT_STREQ("log", msg["command"].asCString());
    ASSERT_STREQ("hello, world", msg["msg"].asCString());
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
    Value root;
    Reader reader;
    ASSERT_TRUE(reader.parse(legal, root));

    unique_ptr<TopologyContext> tc{ ParseTopologyContext(root) };

    ASSERT_EQ(3, tc->task_id());
    ASSERT_STREQ("/path/to/pids", tc->pid_dir().c_str());
    ASSERT_EQ(5u, tc->task_2_component().size());
    ASSERT_STREQ("kafka-reader", tc->task_2_component().at(2).c_str());
    ASSERT_STREQ("sentry-query", tc->task_2_component().at(4).c_str());
    ASSERT_STREQ("rules-matcher", tc->component().c_str());
    ASSERT_STREQ("test-topology", tc->config()["storm.id"].asCString());
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
    ASSERT_STREQ("test-topology", tc->config()["storm.id"].asCString());

    Value jpid = NextMessage(os);
    ASSERT_TRUE(jpid.isMember("pid"));
    ASSERT_TRUE(jpid["pid"].isInt());

    stringstream pid_file_path;
    pid_file_path << "/tmp/" << jpid["pid"].asInt();
    ASSERT_EQ(0, remove(pid_file_path.str().c_str()));
}
