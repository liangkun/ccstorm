// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#ifndef STORM_JSON_H
#define STORM_JSON_H

#include <string>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/error/error.h"
#include "rapidjson/rapidjson.h"

namespace storm { namespace json {

// Redefine default allocator to rapidjson::CrtAllocator instead of rapidjson::MemoryPoolAllocator.
using CrtAllocator = rapidjson::CrtAllocator;
using Value = rapidjson::GenericValue<rapidjson::UTF8<>, CrtAllocator>;
using Document = rapidjson::GenericDocument<rapidjson::UTF8<>, CrtAllocator>;

// Provide a global rapidjson::CrtAllocator instance to use by default.
extern CrtAllocator g_Allocator;

// Json writer.
using StringBuffer = rapidjson::StringBuffer;
using Writer = rapidjson::Writer<StringBuffer>;

// ParseError information.
using GetParseErrorFunc = rapidjson::GetParseErrorFunc;
extern GetParseErrorFunc GetParseError;

// Convert a std::string into Value without copying
inline Value ToValue(const std::string &str) { return Value(str.c_str(), str.size()); }

}} // namespace storm::json

#endif // STORM_JSON_H
