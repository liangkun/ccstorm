// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#ifndef STORM_VALUES_H
#define STORM_VALUES_H

#include "storm/internal/json.h"

namespace storm {

// Value contains a dynamically typed entry.
using Value = internal::json::Value;

// Values contains a list of dynamically typed entries(indexed from 0) in a tuple.
using Values = internal::json::Value;

// Default value allocator to use.
using CrtAllocator = internal::json::CrtAllocator;
extern CrtAllocator &g_CrtAllocator;

// Convert a std::string into Value without copying;
static inline Value ToValue(const std::string &str) { return Value(str.c_str(), str.size()); }

}

#endif // STORM_VALUES_H
