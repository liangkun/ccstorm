// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#ifndef STORM_VALUES_H
#define STORM_VALUES_H

#include "storm/json.h"

namespace storm {

// Value contains a dynamically typed entry.
using Value = json::Value;

// Values contains a list of dynamically typed entries(indexed from 0) in a tuple.
using Values = json::Value;

// Default value allocator to use.
using CrtAllocator = json::CrtAllocator;
extern CrtAllocator &g_allocator;

// Convert a std::string into Value without copying
using json::ToValue;

}

#endif // STORM_VALUES_H
