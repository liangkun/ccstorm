// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include <memory>
#include <string>
#include "gtest/gtest.h"
#include "storm/exception.h"
#include "storm/internal/protocol.h"

using std::string;
using std::unique_ptr;
using storm::TopologyContext;
using storm::ProtocolException;
using namespace storm::internal::protocol;

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

    unique_ptr<TopologyContext> tc {ParseTopologyContext(legal)};

    ASSERT_EQ(3, tc->task_id());
    ASSERT_STREQ("/path/to/pids", tc->pid_dir().c_str());
    ASSERT_EQ(5u, tc->task_2_component().size());
    ASSERT_STREQ("kafka-reader", tc->task_2_component().at(2).c_str());
    ASSERT_STREQ("sentry-query", tc->task_2_component().at(4).c_str());
    ASSERT_STREQ("rules-matcher", tc->component().c_str());
    ASSERT_STREQ("test-topology", tc->config()["storm.id"].GetString());
}

TEST(ProtocolTest, ParseTopologyContext_IllegalJson) {
    ASSERT_THROW(ParseTopologyContext(""), ProtocolException);
    ASSERT_THROW(ParseTopologyContext("{abc: 12}"), ProtocolException);
}
