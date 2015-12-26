// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include "storm/json.h"
#include <stdexcept>
#include <sstream>
#include <string>
#include "rapidjson/error/en.h"

using std::runtime_error;
using std::string;
using std::stringstream;

namespace storm { namespace json {

CrtAllocator g_allocator;
GetParseErrorFunc GetParseError = rapidjson::GetParseError_En;

string ValueToString(const Value &value) {
    StringBuffer buffer;
    Writer writer(buffer, &g_allocator);
    value.Accept(writer);
    const char *str = buffer.GetString();
    return string(str, buffer.GetSize());
}

}}  // namespace storm::internal::json
