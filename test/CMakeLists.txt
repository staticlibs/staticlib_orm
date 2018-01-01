# Copyright 2016, alex at staticlibs.net
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required ( VERSION 2.8.12 )

# toolchain setup
set ( STATICLIB_TOOLCHAIN linux_amd64_gcc CACHE STRING "toolchain triplet" )
if ( NOT DEFINED STATICLIB_CMAKE )
    set ( STATICLIB_CMAKE ${CMAKE_CURRENT_LIST_DIR}/../../cmake CACHE INTERNAL "" )    
endif ( )
set ( CMAKE_TOOLCHAIN_FILE ${STATICLIB_CMAKE}/toolchains/${STATICLIB_TOOLCHAIN}.cmake CACHE INTERNAL "" )

# project
project ( staticlib_orm_test CXX )
include ( ${STATICLIB_CMAKE}/staticlibs_common.cmake )

# options
set ( SOCI_ENABLE_POSTGRES ON CACHE BOOL "Enable PostgreSQL backend" )
if ( NOT STATICLIB_TOOLCHAIN MATCHES "linux_[^_]+_[^_]+" )    
    set ( SOCI_POSTGRES_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/external_libpq/libpq/include" CACHE STRING "Path to the PostgreSQL headers" )
endif ( )

# dependencies
if ( NOT DEFINED STATICLIB_DEPS )
    set ( STATICLIB_DEPS ${CMAKE_CURRENT_LIST_DIR}/../../ CACHE INTERNAL "" )    
endif ( )
if ( NOT STATICLIB_TOOLCHAIN MATCHES "linux_[^_]+_[^_]+" )    
    staticlib_add_subdirectory ( ${STATICLIB_DEPS}/external_jansson )
    staticlib_add_subdirectory ( ${STATICLIB_DEPS}/external_sqlite )
    staticlib_add_subdirectory ( ${STATICLIB_DEPS}/external_libpq )
    staticlib_add_subdirectory ( ${STATICLIB_DEPS}/external_soci )
endif ( )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_config )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_support )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_io )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_ranges )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_pimpl )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_json )
staticlib_add_subdirectory ( ${STATICLIB_DEPS}/staticlib_utils )
staticlib_add_subdirectory ( ${CMAKE_CURRENT_LIST_DIR}/../../staticlib_orm )
set ( ${PROJECT_NAME}_DEPS staticlib_orm staticlib_utils )
staticlib_pkg_check_modules ( ${PROJECT_NAME}_DEPS_PC REQUIRED ${PROJECT_NAME}_DEPS )

# tests
set ( ${PROJECT_NAME}_TEST_INCLUDES ${${PROJECT_NAME}_DEPS_PC_INCLUDE_DIRS} )
set ( ${PROJECT_NAME}_TEST_LIBS ${${PROJECT_NAME}_DEPS_PC_LIBRARIES} soci sqlite3 pq jansson staticlib_utils )
if ( STATICLIB_TOOLCHAIN MATCHES "windows_[^_]+_[^_]+" )
    list ( APPEND ${PROJECT_NAME}_TEST_LIBS ws2_32 wtsapi32 secur32 )
endif ( )
set ( ${PROJECT_NAME}_TEST_OPTS ${${PROJECT_NAME}_DEPS_PC_CFLAGS_OTHER} )
staticlib_enable_testing ( ${PROJECT_NAME}_TEST_INCLUDES ${PROJECT_NAME}_TEST_LIBS ${PROJECT_NAME}_TEST_OPTS )
