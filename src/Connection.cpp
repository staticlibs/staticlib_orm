/* 
 * File:   Connection.cpp
 * Author: alex
 * 
 * Created on May 28, 2016, 10:52 PM
 */

#include "staticlib/orm/Connection.hpp"

#include "staticlib/pimpl/pimpl_forward_macros.hpp"

namespace staticlib {
namespace orm {

namespace { // anonymous
 
namespace ss = staticlib::serialization;

} // namespace

class Connection::Impl : public staticlib::pimpl::PimplObject::Impl {

public:    
    Impl(std::string url) {
        (void) url;
    }

    Transaction start_transaction(Connection&) {
        throw OrmException("");
    }

    ResultSet query(Connection&, std::string sql, const ss::JsonValue& param) {
        (void) sql;
        (void) param;
        throw OrmException("");
    }

    void execute(Connection&, std::string sql, const ss::JsonValue& param) {
        (void) sql;
        (void) param;
    }

    
};
PIMPL_FORWARD_CONSTRUCTOR(Connection, (std::string), (), OrmException)
PIMPL_FORWARD_METHOD(Connection, Transaction, start_transaction, (), (), OrmException)
PIMPL_FORWARD_METHOD(Connection, ResultSet, query, (std::string)(const ss::JsonValue&), (), OrmException)
PIMPL_FORWARD_METHOD(Connection, void, execute, (std::string)(const ss::JsonValue&), (), OrmException)


} // namespace
}
