// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#ifndef STORM_EXCEPTIONS_H
#define STORM_EXCEPTIONS_H

#include <string>

namespace storm {

class StormException {
public:
    StormException(): _code(-1), _message("") {}
    explicit StormException(int code): _code(code), _message("") {}
    explicit StormException(const std::string &message): _code(-1), _message(message) {}
    StormException(int code, const std::string &message): _code(code), _message(message) {}
    virtual ~StormException() {}

    int code() { return _code; }
    const std::string &message() { return _message; }

private:
    int _code;
    const std::string _message;
};

class ProtocolException: public StormException {
    using StormException::StormException;
};

}  // namespace storm

#endif // STORM_EXCEPTIONS_H
