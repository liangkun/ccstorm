// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include "storm/internal/protocol.h"
#include "storm/exceptions.h"
#include "gtest/gtest.h"
#include <string>
#include <memory>

using std::string;
using std::unique_ptr;
using storm::TopologyContext;
using storm::StormException;
using namespace storm::internal::protocol;

TEST(ProtocolTest, ParseTopologyContext) {
    const string normal(
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
    );

    try {
        unique_ptr<TopologyContext> tc = ParseTopologyContext(normal);


    ASSERT_STREQ("/path/to/pids", tc->pid_dir().c_str());
    ASSERT_EQ(3, tc->task_id());
    ASSERT_STREQ("rules-matcher", tc->component().c_str());
    } catch (StormException &e) {
        std::cerr << e.code() << ": " << e.message() << std::endl;
    }
}
