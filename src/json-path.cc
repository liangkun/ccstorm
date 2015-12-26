// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include "storm/json.h"
#include <stdexcept>
#include <sstream>
#include <string>
#include <utility>

using std::move;
using std::runtime_error;
using std::string;
using std::stringstream;
using std::isdigit;
using storm::json::Value;
using storm::json::g_allocator;

namespace {

// implements json path parser
void ParseIdentifier(const string &path, int current_index, const char **identifier, int *length) {
    *identifier = &path[current_index];
    int next = current_index;
    while (path[next] != '.' && path[next] != '[' && next < path.size()) ++next;
    *length = next - current_index;
}

void ParseIndex(const string &path, int current_index, int *result, int *length) {
    *result = 0;
    int next = current_index;
    while (path[next] != ']' && next < path.size()) {
        if (isdigit(path[next])) {
            *result = *result * 10 + path[next] - '0';
        } else {
            stringstream ss;
            ss << "unexpected char '" << path[next] << "' at " << next;
            throw runtime_error(ss.str());
        }
        ++next;
    }
    *length = next - current_index;
}

void ParseElement(const string &path, int current_index, Value &output) {
    while (current_index < path.size()) {
        switch (path[current_index]) {
            case '.':
                current_index += 1;
                const char *identifier;
                int identifier_length;
                ParseIdentifier(path, current_index, &identifier, &identifier_length);
                if (identifier_length == 0) {
                    stringstream ss;
                    ss << "empty identifier at " << current_index;
                    throw runtime_error(ss.str());
                }
                output.PushBack(Value(identifier, identifier_length, g_allocator), g_allocator);
                current_index += identifier_length;
                break;

            case '[':
                current_index += 1;
                int index;
                int index_length;
                ParseIndex(path, current_index, &index, &index_length);
                output.PushBack(Value(index), g_allocator);
                current_index += index_length;
                if (current_index >= path.size() || path[current_index] != ']') {
                    stringstream ss;
                    ss << "expect ']' at " << current_index;
                    throw runtime_error(ss.str());
                }
                current_index += 1;
                break;

            default: {
                stringstream ss;
                ss << "unexpected char '" << path[current_index] << "' at " << current_index;
                throw runtime_error(ss.str());
            }
        }
    }
}

void ParseRoot(const string &path, int current_index, Value &output) {
    if (current_index >= path.size()) {
        throw runtime_error("path ends even before root '$' sign.");
    }
    if (path[current_index] != '$') {
        stringstream ss;
        ss << "unexpected char '" << path[current_index] << "' at " << current_index;
        throw runtime_error(ss.str());
    }
    ParseElement(path, current_index + 1, output);
}

}  // anonymouse namespace

namespace storm { namespace json {

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

Value StringToPath(const string &path_str) {
    Value path;
    path.SetArray();
    ParseRoot(path_str, 0, path);
    return move(path);
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

}}  // namespace storm::json