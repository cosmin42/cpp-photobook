cmake_minimum_required(VERSION 3.27)

project(pblib)

add_library(pblib SHARED
    include/PBLog.h
    include/util/Traits.h
)

set_target_properties(pblib PROPERTIES LINKER_LANGUAGE CXX)