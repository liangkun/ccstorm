// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.
#ifndef STORM_SPIN_LOCK_H
#define STORM_SPIN_LOCK_H

#include <atomic>

namespace storm {

// simple spin lock
class spin_lock {
public:
    spin_lock(std::atomic_flag *flag): _flag(flag) {
        while(_flag->test_and_set()) /* nop */ ;
    }
    ~spin_lock() { _flag->clear(); }
    spin_lock(const spin_lock &) = delete;
    spin_lock &operator=(const spin_lock &) = delete;

private:
    std::atomic_flag *_flag;
};

}  // namespace storm

#endif  // STORM_SPIN_LOCK_H
