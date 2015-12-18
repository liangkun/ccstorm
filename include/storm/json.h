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
extern CrtAllocator g_allocator;

// Json writer.
using StringBuffer = rapidjson::StringBuffer;
using Writer = rapidjson::Writer<StringBuffer>;

// ParseError information.
using GetParseErrorFunc = rapidjson::GetParseErrorFunc;
extern GetParseErrorFunc GetParseError;

// Convert a std::string into Value without copying
inline Value ToValue(const std::string &str) { return Value(str.c_str(), str.size()); }

// Check if @param path is an array and only contains string and integer elements, throws
// runtime_error otherwise.
void CheckPath(const Value &path);

// Encode a json path into a string, throws runtime_error if @param path is illegal.
std::string PathToString(const Value &path);

// Get the value at the @param path in @param root. Return null if the path does not exist in
// @param root. throws runtime_error if @param path is illegal.
Value *GetValue(Value &root, const Value &path);

}} // namespace storm::json

#endif // STORM_JSON_H
