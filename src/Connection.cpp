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
        switch (param.get_type()) {
        case ss::JsonType::NULL_T: 
            // no-op
            break;
        case ss::JsonType::OBJECT: 
            for (const ss::JsonField& fi : param.get_object()) {
                use_json_field(integer_refs, real_refs, st, fi);
            }
            break;
        case ss::JsonType::ARRAY:
        case ss::JsonType::BOOLEAN:
            st.exchange(soci::use(ss::dump_json_to_string(param)));
            break;
        case ss::JsonType::STRING:
            st.exchange(soci::use(param.get_string()));
            break;
        case ss::JsonType::INTEGER:
            integer_refs.emplace_back(static_cast<long long> (param.get_integer()));
            st.exchange(soci::use(integer_refs.back()));
            break;
        case ss::JsonType::REAL:
            real_refs.emplace_back(param.get_real());
            st.exchange(soci::use(real_refs.back()));
            break;
        default:
            throw OrmException(TRACEMSG(std::string() +
                    "Invalid param type: [" + ss::stringify_json_type(param.get_type()) + "]"));            
        }
    }
    
    void use_json_field(std::list<long long>& integer_refs, std::list<double> real_refs,
            soci::statement& st, const ss::JsonField& fi) {
        switch (fi.get_type()) {
        case ss::JsonType::NULL_T:
            st.exchange(soci::use(empty_string(), null_input(), fi.get_name()));
            break;
        case ss::JsonType::ARRAY:
        case ss::JsonType::OBJECT:
        case ss::JsonType::BOOLEAN:
            st.exchange(soci::use(ss::dump_json_to_string(fi.get_value()), fi.get_name()));
            break;
        case ss::JsonType::STRING:
            st.exchange(soci::use(fi.get_string(), fi.get_name()));
            break;
        case ss::JsonType::INTEGER:
            integer_refs.emplace_back(static_cast<long long>(fi.get_integer()));
            st.exchange(soci::use(integer_refs.back(), fi.get_name()));
            break;
        case ss::JsonType::REAL:
            real_refs.emplace_back(fi.get_real());
            st.exchange(soci::use(real_refs.back(), fi.get_name()));
            break;
        default:
            throw OrmException(TRACEMSG(std::string() + 
                    "Invalid field type: [" + ss::stringify_json_type(fi.get_type()) + "],"
                    " field name: [" + fi.get_name() + "]"));
        }
    }
    
    ss::JsonValue to_json_value(soci::row& row) {
        ss::JsonValue res{std::vector<ss::JsonField>{}};
        std::vector<ss::JsonField>& vec = *res.get_object_ptr().first;
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
