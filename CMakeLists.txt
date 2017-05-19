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

# project
project ( staticlib_orm CXX )
set ( ${PROJECT_NAME}_STATICLIB_VERSION 1.0.1 )
set ( ${PROJECT_NAME}_DESCRIPTION "Staticlibs ORM library" )
set ( ${PROJECT_NAME}_URL https://github.com/staticlibs/staticlib_orm )
include ( ${CMAKE_CURRENT_LIST_DIR}/resources/macros.cmake )

# docs
option ( ${PROJECT_NAME}_ENABLE_DOCS "Generate doxyfile and exit build" OFF )
if ( ${PROJECT_NAME}_ENABLE_DOCS )
    configure_file ( ${CMAKE_CURRENT_LIST_DIR}/resources/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile )
    return ( )
endif ( )

# check deplibs cache
if ( STATICLIB_USE_DEPLIBS_CACHE )
    set ( ${PROJECT_NAME}_CACHED_LIB_PATH ${STATICLIB_DEPLIBS_CACHE_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${PROJECT_NAME}${CMAKE_STATIC_LIBRARY_SUFFIX} )
    if ( EXISTS ${${PROJECT_NAME}_CACHED_LIB_PATH} )
        add_library( ${PROJECT_NAME} STATIC IMPORTED GLOBAL )
        set_target_properties( ${PROJECT_NAME} PROPERTIES IMPORTED_LOCATION ${${PROJECT_NAME}_CACHED_LIB_PATH} )
        message ( STATUS "Using cached library: [${${PROJECT_NAME}_CACHED_LIB_PATH}]" )
        return ( )
    endif ( )
endif ( )

# standalone build
if ( NOT DEFINED CMAKE_LIBRARY_OUTPUT_DIRECTORY )
    set ( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )
endif ( )
if ( NOT DEFINED STATICLIB_TOOLCHAIN )
    if ( NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" ) 
        staticlib_orm_add_subdirectory ( ${CMAKE_CURRENT_LIST_DIR}/../external_jansson )
        staticlib_orm_add_subdirectory ( ${CMAKE_CURRENT_LIST_DIR}/../external_sqlite )
    endif (  )
    staticlib_orm_add_subdirectory ( ${CMAKE_CURRENT_LIST_DIR}/../external_soci )
    staticlib_orm_add_subdirectory ( ${CMAKE_CURRENT_LIST_DIR}/../staticlib_config )
    staticlib_orm_add_subdirectory ( ${CMAKE_CURRENT_LIST_DIR}/../staticlib_support )
    staticlib_orm_add_subdirectory ( ${CMAKE_CURRENT_LIST_DIR}/../staticlib_io )
    staticlib_orm_add_subdirectory ( ${CMAKE_CURRENT_LIST_DIR}/../staticlib_pimpl )
    staticlib_orm_add_subdirectory ( ${CMAKE_CURRENT_LIST_DIR}/../staticlib_ranges )
    staticlib_orm_add_subdirectory ( ${CMAKE_CURRENT_LIST_DIR}/../staticlib_json )
endif ( )

# deps
set ( ${PROJECT_NAME}_DEPS_PUBLIC
        staticlib_config
        staticlib_support
        staticlib_pimpl
        staticlib_json )
set ( ${PROJECT_NAME}_DEPS_PRIVATE 
        jansson 
        soci
        sqlite3 )
staticlib_orm_pkg_check_modules ( ${PROJECT_NAME}_DEPS_PUBLIC_PC REQUIRED ${PROJECT_NAME}_DEPS_PUBLIC )
staticlib_orm_pkg_check_modules ( ${PROJECT_NAME}_DEPS_PRIVATE_PC REQUIRED ${PROJECT_NAME}_DEPS_PRIVATE )

# library
file ( GLOB_RECURSE ${PROJECT_NAME}_SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp )
file ( GLOB_RECURSE ${PROJECT_NAME}_HEADERS ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp )
add_library ( ${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SRC} ${${PROJECT_NAME}_HEADERS} )
source_group ( "src" FILES ${${PROJECT_NAME}_SRC} )
source_group ( "include" FILES ${${PROJECT_NAME}_HEADERS} )
target_include_directories ( ${PROJECT_NAME} BEFORE PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/include
        ${${PROJECT_NAME}_DEPS_PUBLIC_PC_INCLUDE_DIRS}
        ${${PROJECT_NAME}_DEPS_PRIVATE_PC_INCLUDE_DIRS} )
target_compile_options ( ${PROJECT_NAME} PRIVATE 
        ${${PROJECT_NAME}_DEPS_PUBLIC_PC_CFLAGS_OTHER}
        ${${PROJECT_NAME}_DEPS_PRIVATE_PC_CFLAGS_OTHER} )
if ( TARGET external_jansson )
    add_dependencies ( ${PROJECT_NAME} external_jansson )
endif ( )

# pkg-config
set ( ${PROJECT_NAME}_PC_CFLAGS "-I${CMAKE_CURRENT_LIST_DIR}/include" )
set ( ${PROJECT_NAME}_PC_LIBS "-L${CMAKE_LIBRARY_OUTPUT_DIRECTORY} -l${PROJECT_NAME}" )
staticlib_orm_list_to_string ( ${PROJECT_NAME}_PC_REQUIRES "" ${PROJECT_NAME}_DEPS_PUBLIC )
staticlib_orm_list_to_string ( ${PROJECT_NAME}_PC_REQUIRES_PRIVATE "" ${PROJECT_NAME}_DEPS_PRIVATE )
configure_file ( ${CMAKE_CURRENT_LIST_DIR}/resources/pkg-config.in 
        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/pkgconfig/${PROJECT_NAME}.pc )
