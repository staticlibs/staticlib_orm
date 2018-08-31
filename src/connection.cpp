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
 * File:   connection.cpp
 * Author: alex
 * 
 * Created on May 28, 2016, 10:52 PM
 */

#include "staticlib/orm/connection.hpp"

#include <cstring>
#include <ctime>
#include <atomic>
#include <array>
#include <list>
#include <vector>

#include "soci.h"
#include "sqlite3/soci-sqlite3.h"
#include "postgresql/soci-postgresql.h"

#include "staticlib/pimpl/forward_macros.hpp"
#include "staticlib/json.hpp"

namespace staticlib {
namespace orm {

namespace { // anonymous

static std::string empty_string_val = std::string();
static soci::indicator null_input_val = soci::i_null;

// workaround for questionalble soci dynamic backed loading logic
// no-op for bundled soci

std::atomic<bool>& static_backend_flag() {
    static std::atomic<bool> flag;
    return flag;
}

soci::sqlite3_backend_factory& static_sqlite_backend_factory() {
    static soci::sqlite3_backend_factory fac;
    return fac;
}

soci::postgresql_backend_factory& static_postgresql_backend_factory() {
    static soci::postgresql_backend_factory fac;
    return fac;
}

} // namespace

class connection::impl : public sl::pimpl::object::impl {
    std::string url;
    soci::session session;
 
public:
    virtual ~impl() STATICLIB_NOEXCEPT { 
        session.close();
    };

    impl(std::string url) :
    url(std::move(url)),
    session([this] {
        impl::initialize_backends();
        return this->url;
    }()) { }

    static void initialize_backends() {
        bool the_false = false;
        if (static_backend_flag().compare_exchange_strong(the_false, true)) {
            soci::dynamic_backends::register_backend("sqlite", static_sqlite_backend_factory());
            soci::dynamic_backends::register_backend("sqlite3", static_sqlite_backend_factory());
            soci::dynamic_backends::register_backend("postgresql", static_postgresql_backend_factory());
        }
    }

    transaction start_transaction(connection&) {
        return transaction(static_cast<void*>(std::addressof(session)));
    }

    std::vector<sl::json::value> query(connection&, std::string sql, const sl::json::value& param) {
        try {
            // init
            soci::statement st{session};
            st.alloc();
            st.prepare(sql);

            // input
            auto integer_refs = std::list<long long>();
            auto real_refs = std::list<double>();
            auto string_refs = std::list<std::string>();
            use_json_value(integer_refs, real_refs, string_refs, st, param);
            
            // output
            soci::row row;        
            st.exchange(soci::into(row));

            // run
            st.define_and_bind();
            st.execute();

            // fetch
            std::vector<sl::json::value> res{};
            while(st.fetch()) {
                res.emplace_back(to_json_value(row));
            }
            return res;
        } catch(const std::exception& e) {
            throw orm_exception(TRACEMSG(std::string() + e.what() + ""
                    "\nError executing query: [" + sql + "]" +
                    " with parameters: [" + param.dumps() + "]"));
        }
    }

    void execute(connection&, std::string sql, const sl::json::value& param) {
        try {
            soci::statement st{session};
            st.alloc();
            st.prepare(sql);

            // in
            auto integer_refs = std::list<long long>();
            auto real_refs = std::list<double>();
            auto string_refs = std::list<std::string>();
            use_json_value(integer_refs, real_refs, string_refs, st, param);

            // run
            st.define_and_bind();
            st.execute(true);
        } catch (const std::exception& e) {
            throw orm_exception(TRACEMSG(std::string() + e.what() + ""
                    "\nError executing query: [" + sql + "]" +
                    " with parameters: [" + param.dumps() + "]"));
        }
    }
    
 private:

    void use_json_value(std::list<long long>& integer_refs, std::list<double>& real_refs,
            std::list<std::string>& string_refs, soci::statement& st, const sl::json::value& param) {
        switch (param.json_type()) {
        case sl::json::type::object:
            for (const sl::json::field& fi : param.as_object()) {
                use_json_field_internal(integer_refs, real_refs, string_refs, st, fi);
            }
            break;
        case sl::json::type::array:
            for (const sl::json::value& va : param.as_array()) {
                use_json_value_internal(integer_refs, real_refs, string_refs, st, va);
            }
            break;
        case sl::json::type::nullt:
            // no-op
            break;
        default:
            use_json_value_internal(integer_refs, real_refs, string_refs, st, param);
            break;
        }
    }
 
    void use_json_value_internal(std::list<long long>& integer_refs, std::list<double>& real_refs,
            std::list<std::string>& string_refs, soci::statement& st, const sl::json::value& param) {
        switch (param.json_type()) {
        case sl::json::type::nullt: 
            st.exchange(soci::use(empty_string(), null_input()));
            break;
        case sl::json::type::array:
        case sl::json::type::object: 
        case sl::json::type::boolean:
            string_refs.emplace_back(param.dumps());
            st.exchange(soci::use(string_refs.back()));
            break;
        case sl::json::type::string:
            st.exchange(soci::use(param.as_string()));
            break;
        case sl::json::type::integer:
            integer_refs.emplace_back(static_cast<long long> (param.as_int64()));
            st.exchange(soci::use(integer_refs.back()));
            break;
        case sl::json::type::real:
            real_refs.emplace_back(param.as_double());
            st.exchange(soci::use(real_refs.back()));
            break;
        default:
            throw orm_exception(TRACEMSG(std::string() +
                    "Invalid param type: [" + sl::json::stringify_json_type(param.json_type()) + "]"));
        }
    }

    void use_json_field_internal(std::list<long long>& integer_refs, std::list<double>& real_refs,
            std::list<std::string>& string_refs, soci::statement& st, const sl::json::field& fi) {
        switch (fi.json_type()) {
        case sl::json::type::nullt:
            st.exchange(soci::use(empty_string(), null_input(), fi.name()));
            break;
        case sl::json::type::array:
        case sl::json::type::object:
        case sl::json::type::boolean:
            string_refs.emplace_back(fi.val().dumps());
            st.exchange(soci::use(string_refs.back(), fi.name()));
            break;
        case sl::json::type::string:
            st.exchange(soci::use(fi.as_string(), fi.name()));
            break;
        case sl::json::type::integer:
            integer_refs.emplace_back(static_cast<long long>(fi.as_int64()));
            st.exchange(soci::use(integer_refs.back(), fi.name()));
            break;
        case sl::json::type::real:
            real_refs.emplace_back(fi.as_double());
            st.exchange(soci::use(real_refs.back(), fi.name()));
            break;
        default:
            throw orm_exception(TRACEMSG(std::string() + 
                    "Invalid field type: [" + sl::json::stringify_json_type(fi.json_type()) + "],"
                    " field name: [" + fi.name() + "]"));
        }
    }

    sl::json::value to_json_value(soci::row& row) {
        std::vector<sl::json::field> vec;
        for (std::size_t i = 0; i != row.size(); ++i) {
            const soci::column_properties& props = row.get_properties(i);
            if (soci::i_null == row.get_indicator(i)) {
                vec.emplace_back(props.get_name(),sl::json::value());
                continue;
            }
            switch (props.get_data_type()) {
            case soci::dt_string:
                vec.emplace_back(props.get_name(), row.get<std::string>(i));
                break;
            case soci::dt_double:
                vec.emplace_back(props.get_name(), row.get<double>(i));
                break;
            case soci::dt_integer:
                vec.emplace_back(props.get_name(), row.get<int>(i));
                break;
            case soci::dt_long_long:
                vec.emplace_back(props.get_name(), static_cast<int64_t>(row.get<long long>(i)));
                break;
            case soci::dt_unsigned_long_long:
                vec.emplace_back(props.get_name(), static_cast<uint64_t> (row.get<unsigned long long>(i)));
                break;
            case soci::dt_date:
                std::tm date = row.get<std::tm>(i);
                std::array<char, 128> buf;
                size_t len = std::strftime(buf.data(), buf.size(), "%Y-%m-%dT%H:%M:%SZ", std::addressof(date));
                std::string str(buf.data(), len);
                vec.emplace_back(props.get_name(), str);
                break;
            }
        }
        return sl::json::value(std::move(vec));
    }

    soci::indicator& null_input() {
        return null_input_val;
    }
    
    std::string& empty_string() {
        return empty_string_val;
    }
    
};
PIMPL_FORWARD_CONSTRUCTOR(connection, (std::string), (), orm_exception)
PIMPL_FORWARD_METHOD_STATIC(connection, void, initialize_backends, (), (), orm_exception)
PIMPL_FORWARD_METHOD(connection, transaction, start_transaction, (), (), orm_exception)
PIMPL_FORWARD_METHOD(connection, std::vector<sl::json::value>, query, (std::string)(const sl::json::value&), (), orm_exception)
PIMPL_FORWARD_METHOD(connection, void, execute, (std::string)(const sl::json::value&), (), orm_exception)


} // namespace
}
