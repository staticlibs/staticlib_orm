/*
 * Copyright 2016, alex at staticlibs.net
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * File:   orm_test.cpp
 * Author: alex
 *
 * Created on May 28, 2016, 7:10 PM
 */

#include "staticlib/orm.hpp"
#include "staticlib/serialization.hpp"

#include <iostream>

#include "staticlib/config/assert.hpp"
#include "staticlib/utils.hpp"

namespace so = staticlib::orm;
namespace ss = staticlib::serialization;
namespace su = staticlib::utils;

void test_simple() {
    // init
    auto conn = so::Connection{"sqlite://test.db"};
    conn.execute("drop table if exists t1");
    conn.execute("create table t1 (foo varchar, bar int)");
    
    // insert
    conn.execute("insert into t1 values(:foo, :bar)", {
        {"foo", "aaa"},
        {"bar", 41}
    });
    conn.execute("insert into t1 values(:foo, :bar)", {
        {"foo", "bbb"},
        {"bar", 42}
    });
    conn.execute("insert into t1 values(:foo, :bar)",{
        {"foo", "ccc"},
        {"bar", 43}
    });
    
    // select
    std::vector<ss::JsonValue> res = conn.query("select foo, bar from t1 where foo = :foo or bar = :bar order by bar", {
        {"foo", "ccc"}, 
        {"bar", 42}
    });
    slassert(2 == res.size());
    slassert(ss::JsonType::OBJECT == res[0].get_type());        
    slassert("bbb" == res[0].get("foo").get_string());
    slassert(42 == res[0].get("bar").get_integer());
    slassert(ss::JsonType::OBJECT == res[1].get_type());
    slassert("ccc" == res[1].get("foo").get_string());
    slassert(43 == res[1].get("bar").get_integer());
    
}

void test_transaction() {
    // init
    auto conn = so::Connection{"sqlite://test.db"};
    conn.execute("drop table if exists t1");
    conn.execute("create table t1 (foo varchar, bar int)");
    
    // rollback
    {
        auto tran = conn.start_transaction();
        conn.execute("insert into t1 values(:foo, :bar)",{
            {"foo", "aaa"},
            {"bar", 41}
        });
    }
    {
        std::vector<ss::JsonValue> res = conn.query("select count(*) as cc from t1");
        slassert(1 == res.size());
        // sqlite dynamic type
        slassert(ss::JsonType::STRING == res[0].get("cc").get_type());
        slassert(0 == su::parse_uint32(res[0].get("cc").get_string()));
    }

    // commit
    {
        auto tran = conn.start_transaction();
        conn.execute("insert into t1 values(:foo, :bar)",{
            {"foo", "aaa"},
            {"bar", 41}
        });
        tran.commit();
    }
    {
        std::vector<ss::JsonValue> res = conn.query("select count(*) as cc from t1");
        slassert(1 == res.size());
        // sqlite dynamic type
        slassert(ss::JsonType::STRING == res[0].get("cc").get_type());
        slassert(1 == su::parse_uint32(res[0].get("cc").get_string()));
    }
}


int main() {
    try {
        test_simple();
        test_transaction();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

