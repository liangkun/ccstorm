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

void CheckPath(const Value &path) {
    if (!path.IsArray()) throw runtime_error("json path is not an array");
    for (unsigned int i = 0; i < path.Size(); ++i) {
        if (!(path[i].IsString() || path[i].IsInt())) {
            stringstream error;
            error << "json path contains illegal element at position " << i;
            throw runtime_error(error.str());
        }
    }
}

string ValueToString(const Value &value) {
    StringBuffer buffer;
    Writer writer(buffer, &g_allocator);
    value.Accept(writer);
    const char *str = buffer.GetString();
    return string(str, buffer.GetSize());
}

string PathToString(const Value &path) {
    if (!path.IsArray()) throw runtime_error("json path is not an array");

    stringstream ss;
    ss << "$";
    for (unsigned int i = 0; i < path.Size(); ++i) {
        if (path[i].IsString()) {
            ss << "." << string(path[i].GetString(), path[i].GetStringLength());
        } else if (path[i].IsInt()) {
            ss << "[" << path[i].GetInt() << "]";
        } else {
            stringstream error;
            error << "json path contains illegal element at position " << i;
            throw runtime_error(error.str());
        }
    }
    return ss.str();
}

const Value *GetValue(const Value &root, const Value &path) {
    return GetValue(const_cast<Value &>(root), path);
}

Value *GetValue(Value &root, const Value &path) {
    if (!path.IsArray()) throw runtime_error("json path is not an array");

    Value *next = &root;
    for (size_t i = 0; i < path.Size(); ++i) {
        const Value &element = path[i];
        if (element.IsString()) {
            if (!next->IsObject()) return nullptr;
            auto it = next->FindMember(element);
            if (it == next->MemberEnd()) return nullptr;
            next = &(it->value);
        } else if (element.IsInt()) {
            if (!next->IsArray()) return nullptr;
            int index = element.GetInt();
            if (index < 0 || index >= next->Size()) return nullptr;
            next = &((*next)[index]);
        } else {
            stringstream error;
            error << "json path contains illegal element at position " << i;
            throw runtime_error(error.str());
        }
    }

    return next;
}

}}  // namespace storm::internal::json
