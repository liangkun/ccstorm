# Copyright (c) 2015 Shumei Inc. All Rights Reserved.
# Authors: Liang Kun <liangkun@ishumei.com>

SET(RAPID_JSON_SEARCH_PATH
    "${RAPID_JSON_SOURCE_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/../thirdparty/rapidjson")

FIND_PATH(RAPID_JSON_SOURCE_DIR
    NAMES CMakeLists.txt
    PATHS ${RAPID_JSON_SEARCH_PATH})

FIND_PATH(RAPID_JSON_INCLUDE_DIR
    NAMES rapidjson/rapidjson.h
    PATH_SUFFIXES include
    HINTS ${RAPID_JSON_SOURCE_DIR}
    PATHS ${RAPID_JSON_SEARCH_PATH})

INCLUDE(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RapidJSONSrc DEFAULT_MSG
    RAPID_JSON_SOURCE_DIR
    RAPID_JSON_INCLUDE_DIR)
