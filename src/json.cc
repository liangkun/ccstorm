// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include "storm/json.h"
#include "rapidjson/error/en.h"

namespace storm { namespace json {

CrtAllocator g_Allocator;
GetParseErrorFunc GetParseError = rapidjson::GetParseError_En;

}}  // namespace storm::internal::json
