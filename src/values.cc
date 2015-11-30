// Copyright (c) 2015 SHUMEI Inc. All rights reserved.
// Authors: Liang Kun <liangkun@ishumei.com>.

#include "storm/values.h"

namespace storm {

CrtAllocator &g_CrtAllocator(internal::json::g_CrtAllocator);

}  // namespace storm
