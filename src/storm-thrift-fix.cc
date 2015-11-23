// Copyright (c) 2015 ShuMei Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include "storm_types.h"

// @see https://issues.apache.org/jira/browse/THRIFT-2154 for details.
bool ExecutorInfo::operator<(const ExecutorInfo &that) const {
    return this < &that;
}

bool GlobalStreamId::operator<(const GlobalStreamId &that) const {
    return this < &that;
}