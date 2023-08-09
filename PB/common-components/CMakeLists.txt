cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_FLAGS "-std=c++20")
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

option(STATIC_CHECK "Statically check the program." OFF)
option(LOG_LEVEL "Log level." OFF)

if (NOT LOG_LEVEL)
    set(LOG_LEVEL None)
endif()

set(LOG_LEVEL_STRING_LIST None Debug Info Warning Error)

if (LOG_LEVEL IN_LIST LOG_LEVEL_STRING_LIST)
    configure_file(../config.h.in ../../include/config.h)
else()
    message(FATAL_ERROR "${LOG_LEVEL} Not in the list of levels ${LOG_LEVEL_STRING_LIST}" )
endif()

if (STATIC_CHECK)
    message("Static check ON")
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")
endif()

project(common-components)

add_library(common-components SHARED
    include/StandardPrinter.h
)

set_target_properties(common-components PROPERTIES LINKER_LANGUAGE CXX)