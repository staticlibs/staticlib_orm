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
 * File:   Connection.cpp
 * Author: alex
 * 
 * Created on May 28, 2016, 10:52 PM
 */

#include "staticlib/orm/Connection.hpp"

#include <cstring>
#include <ctime>
#include <array>
#include <list>
#include <vector>

#include "soci.h"

#include "staticlib/pimpl/pimpl_forward_macros.hpp"
#include "staticlib/serialization.hpp"

namespace staticlib {
namespace orm {

namespace { // anonymous
 
namespace ss = staticlib::serialization;

} // namespace

class Connection::Impl : public staticlib::pimpl::PimplObject::Impl {
    std::string url;
    soci::session session;
    
public:    
    virtual ~Impl() STATICLIB_NOEXCEPT { 
        session.close();
    };
    
    Impl(std::string url) :
    url(std::move(url)),
    session(this->url) { }

    Transaction start_transaction(Connection&) {
        return Transaction(static_cast<void*>(std::addressof(session)));
    }

    std::vector<ss::JsonValue> query(Connection&, std::string sql, const ss::JsonValue& param) {
        try {
            // init
            soci::statement st{session};
            st.alloc();
            st.prepare(sql);

            // input
            std::list<long long> integer_refs{};
            std::list<double> real_refs{};
            use_json_value(integer_refs, real_refs, st, param);
            
            // output
            soci::row row;        
            st.exchange(soci::into(row));

            // run
            st.define_and_bind();
            st.execute();

            // fetch
            std::vector<ss::JsonValue> res{};
            while(st.fetch()) {
                res.emplace_back(to_json_value(row));
            }
            return res;
        } catch(const std::exception& e) {
            throw OrmException(TRACEMSG(std::string() + e.what() + ""
                    "\nError executing query: [" + sql + "]" +
                    " with parameters: [" + ss::dump_json_to_string(param) + "]"));
        }
    }

    void execute(Connection&, std::string sql, const ss::JsonValue& param) {
        try {
            soci::statement st{session};
            st.alloc();
            st.prepare(sql);

            // in
            std::list<long long> integer_refs{};
            std::list<double> real_refs{};
            use_json_value(integer_refs, real_refs, st, param);

            // run
            st.define_and_bind();
            st.execute(true);
        } catch (const std::exception& e) {
            throw OrmException(TRACEMSG(std::string() + e.what() + ""
                    "\nError executing query: [" + sql + "]" +
                    " with parameters: [" + ss::dump_json_to_string(param) + "]"));
        }
    }
    
 private:

    void use_json_value(std::list<long long>& integer_refs, std::list<double> real_refs,
            soci::statement& st, const ss::JsonValue& param) {
        switch (param.type()) {
        case ss::JsonType::OBJECT:
            for (const ss::JsonField& fi : param.as_object()) {
                use_json_field_internal(integer_refs, real_refs, st, fi);
            }
            break;
        case ss::JsonType::ARRAY:
            for (const ss::JsonValue& va : param.as_array()) {
                use_json_value_internal(integer_refs, real_refs, st, va);
            }
            break;
        case ss::JsonType::NULL_T:
            // no-op
            break;
        default:
            use_json_value_internal(integer_refs, real_refs, st, param);
            break;
        }
    }
     
    void use_json_value_internal(std::list<long long>& integer_refs, std::list<double> real_refs,
            soci::statement& st, const ss::JsonValue& param) {
        switch (param.type()) {
        case ss::JsonType::NULL_T: 
            st.exchange(soci::use(empty_string(), null_input()));
            break;
        case ss::JsonType::ARRAY:
        case ss::JsonType::OBJECT: 
        case ss::JsonType::BOOLEAN:
            st.exchange(soci::use(ss::dump_json_to_string(param)));
            break;
        case ss::JsonType::STRING:
            st.exchange(soci::use(param.as_string()));
            break;
        case ss::JsonType::INTEGER:
            integer_refs.emplace_back(static_cast<long long> (param.as_int64()));
            st.exchange(soci::use(integer_refs.back()));
            break;
        case ss::JsonType::REAL:
            real_refs.emplace_back(param.as_double());
            st.exchange(soci::use(real_refs.back()));
            break;
        default:
            throw OrmException(TRACEMSG(std::string() +
                    "Invalid param type: [" + ss::stringify_json_type(param.type()) + "]"));            
        }
    }
    
    void use_json_field_internal(std::list<long long>& integer_refs, std::list<double> real_refs,
            soci::statement& st, const ss::JsonField& fi) {
        switch (fi.type()) {
        case ss::JsonType::NULL_T:
            st.exchange(soci::use(empty_string(), null_input(), fi.name()));
            break;
        case ss::JsonType::ARRAY:
        case ss::JsonType::OBJECT:
        case ss::JsonType::BOOLEAN:
            st.exchange(soci::use(ss::dump_json_to_string(fi.value()), fi.name()));
            break;
        case ss::JsonType::STRING:
            st.exchange(soci::use(fi.as_string(), fi.name()));
            break;
        case ss::JsonType::INTEGER:
            integer_refs.emplace_back(static_cast<long long>(fi.as_int64()));
            st.exchange(soci::use(integer_refs.back(), fi.name()));
            break;
        case ss::JsonType::REAL:
            real_refs.emplace_back(fi.as_double());
            st.exchange(soci::use(real_refs.back(), fi.name()));
            break;
        default:
            throw OrmException(TRACEMSG(std::string() + 
                    "Invalid field type: [" + ss::stringify_json_type(fi.type()) + "],"
                    " field name: [" + fi.name() + "]"));
        }
    }
    
    ss::JsonValue to_json_value(soci::row& row) {
        ss::JsonValue res{std::vector<ss::JsonField>{}};
        std::vector<ss::JsonField>& vec = *res.as_object_mutable().first;
        for (std::size_t i = 0; i != row.size(); ++i) {
            const soci::column_properties& props = row.get_properties(i);
            switch (props.get_data_type()) {
            case soci::dt_string:
                vec.emplace_back(props.get_name(), row.get<std::string>(i, ""));
                break;
            case soci::dt_double:
                vec.emplace_back(props.get_name(), row.get<double>(i, -1));
                break;
            case soci::dt_integer:
                vec.emplace_back(props.get_name(), row.get<int>(i, -1));
                break;
            case soci::dt_long_long:
            case soci::dt_unsigned_long_long:
                vec.emplace_back(props.get_name(), row.get<int64_t>(i, -1));
                break;
            case soci::dt_date:
                std::tm empty;
                std::memset(std::addressof(empty), 0, sizeof(empty));
                std::tm date = row.get<std::tm>(i, empty);
                std::array<char, 128> buf;
                size_t len = std::strftime(buf.data(), buf.size(), "%Y-%m-%dT%H:%M:%SZ", std::addressof(date));
                std::string str(buf.data(), len);
                vec.emplace_back(props.get_name(), str);
                break;
            }     
        }
        return std::move(res);
    }
    
    soci::indicator& null_input() {
        static soci::indicator ind = soci::i_null;
        return ind;
    }
    
    std::string& empty_string() {
        static std::string res;
        return res;
    }
    
};
PIMPL_FORWARD_CONSTRUCTOR(Connection, (std::string), (), OrmException)
PIMPL_FORWARD_METHOD(Connection, Transaction, start_transaction, (), (), OrmException)
PIMPL_FORWARD_METHOD(Connection, std::vector<ss::JsonValue>, query, (std::string)(const ss::JsonValue&), (), OrmException)
PIMPL_FORWARD_METHOD(Connection, void, execute, (std::string)(const ss::JsonValue&), (), OrmException)


} // namespace
}
