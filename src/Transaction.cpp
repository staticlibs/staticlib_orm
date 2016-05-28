/* 
 * File:   Transaction.cpp
 * Author: alex
 * 
 * Created on May 28, 2016, 11:48 PM
 */

#include "staticlib/orm/Transaction.hpp"

#include "staticlib/pimpl/pimpl_forward_macros.hpp"

namespace staticlib {
namespace orm {

namespace { // anonymous

namespace ss = staticlib::serialization;

} // namespace

class Transaction::Impl : public staticlib::pimpl::PimplObject::Impl {
public:

    ResultSet query(Transaction&, std::string sql, const ss::JsonValue& param) {
        (void) sql;
        (void) param;
        throw OrmException("");
    }

    ResultSet query(Transaction&, std::string sql, const InputParams& paramsRange) {
        (void) sql;
        (void) paramsRange;
        throw OrmException("");
    }

    void execute(Transaction&, std::string sql, const ss::JsonValue& param) {
        (void) sql;
        (void) param;   
    }

    void execute(Transaction&, std::string sql, const InputParams& paramsRange) {
        (void) sql;
        (void) paramsRange;
    }

    void commit(Transaction&) {
        
    }

};
PIMPL_FORWARD_METHOD(Transaction, ResultSet, query, (std::string)(const ss::JsonValue&), (), OrmException)
PIMPL_FORWARD_METHOD(Transaction, ResultSet, query, (std::string)(const InputParams&), (), OrmException)
PIMPL_FORWARD_METHOD(Transaction, void, execute, (std::string)(const ss::JsonValue&), (), OrmException)
PIMPL_FORWARD_METHOD(Transaction, void, execute, (std::string)(const InputParams&), (), OrmException)
PIMPL_FORWARD_METHOD(Transaction, void, commit, (), (), OrmException)

} // namespace
}

