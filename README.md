Staticlibs Object-Relational Mapping library
============================================

[![travis](https://travis-ci.org/staticlibs/staticlib_orm.svg?branch=master)](https://travis-ci.org/staticlibs/staticlib_orm)
[![appveyor](https://ci.appveyor.com/api/projects/status/github/staticlibs/staticlib_orm?svg=true)](https://ci.appveyor.com/project/staticlibs/staticlib-orm)

This project is a part of [Staticlibs](http://staticlibs.net/).

This project implements a lightweight [ORM](https://en.wikipedia.org/wiki/Object-relational_mapping) 
on top of the [SOCI](https://github.com/staticlibs/external_soci) database connection library.

Database records are mapped to [sl::json::value](http://staticlibs.net/staticlib_json/docs/html/classstaticlib_1_1json_1_1value.html)
objects from [staticlib_json](https://github.com/staticlibs/staticlib_json) project. 
Input parameters also being read from `sl::json::value`s , both named and positional parameters are supported.

Library does not support bulk operations and result-set streaming. If such features are required
it may be better to talk with DB-specific connect library directly without `staticlib_orm`and SOCI layers.

Link to the [API documentation](http://staticlibs.github.io/staticlib_orm/docs/html/namespacestaticlib_1_1orm.html).

Usage example
-------------

Open connection to SQlite database:

    auto conn = sl::orm::connection{"sqlite://test.db"};

Create a table, insert a record using named parameters:

    conn.execute("create table t1 (foo varchar, bar int)");
    conn.execute("insert into t1 values(:foo, :bar)", {
        {"foo", "bbb"},
        {"bar", 42}
    });

Insert multiple records as a single transaction:

    auto tran = conn.start_transaction();
    conn.execute("insert into t1 values('aaa', 41)");
    conn.execute("insert into t1 values('ccc', 43)");
    tran.commit();

Select records:

    // here res is a vector containing two records
    auto res = conn.query("select foo, bar from t1 where foo = :foo or bar = :bar order by bar", {
        {"foo", "ccc"}, 
        {"bar", 42}
    });

How to build
------------

[CMake](http://cmake.org/) is required for building.

[pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/) utility is used for dependency management.
For Windows users ready-to-use binary version of `pkg-config` can be obtained from [tools_windows_pkgconfig](https://github.com/staticlibs/tools_windows_pkgconfig) repository.
See [StaticlibsPkgConfig](https://github.com/staticlibs/wiki/wiki/StaticlibsPkgConfig) for Staticlibs-specific details about `pkg-config` usage.

To build the library on Windows using Visual Studio 2013 Express run the following commands using
Visual Studio development command prompt 
(`C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\Shortcuts\VS2013 x86 Native Tools Command Prompt`):

    git clone --recursive https://github.com/staticlibs/external_jansson.git
    git clone --recursive https://github.com/staticlibs/external_sqlite.git
    git clone --recursive https://github.com/staticlibs/external_libpq.git
    git clone --recursive https://github.com/staticlibs/external_soci.git
    git clone https://github.com/staticlibs/staticlib_config.git
    git clone https://github.com/staticlibs/staticlib_io.git
    git clone https://github.com/staticlibs/staticlib_ranges.git
    git clone https://github.com/staticlibs/staticlib_serialization.git
    git clone https://github.com/staticlibs/staticlib_pimpl.git
    cd staticlib_orm
    mkdir build
    cd build
    cmake .. 
    msbuild staticlib_orm.sln

Cloning of [external_jansson](https://github.com/staticlibs/external_jansson.git) and 
[external_sqlite](https://github.com/staticlibs/external_sqlite.git) is not required on Linux - 
system libraries will be used instead.

To build on other platforms using GCC or Clang with GNU Make:

    cmake .. -DCMAKE_CXX_FLAGS="--std=c++11"
    make

See [StaticlibsToolchains](https://github.com/staticlibs/wiki/wiki/StaticlibsToolchains) for 
more information about the CMake toolchains setup and cross-compilation.

License information
-------------------

This project is released under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0).

Changelog
---------

**2018-01-01**

 * version 1.0.2
 * vs2017 support

**2017-05-19**

 * version 1.0.1
 * deps changes

**2016-06-01**

 * version 1.0.0
