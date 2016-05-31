/* 
 * File:   Transaction.cpp
 * Author: alex
 * 
 * Created on May 28, 2016, 11:48 PM
 */

#include "staticlib/orm/Transaction.hpp"

#include "soci.h"

#include "staticlib/pimpl/pimpl_forward_macros.hpp"

namespace staticlib {
namespace orm {

namespace { // anonymous

namespace ss = staticlib::serialization;

} // namespace

class Transaction::Impl : public staticlib::pimpl::PimplObject::Impl {
    soci::session& session;
    soci::transaction transaction;
    
public:

    virtual ~Impl() STATICLIB_NOEXCEPT { };
    
    Impl(void* session) :
    session(*static_cast<soci::session*> (session)),
    transaction(this->session) { }

    void commit(Transaction&) {
        transaction.commit();
    }

};
PIMPL_FORWARD_CONSTRUCTOR(Transaction, (void*), (), OrmException)
PIMPL_FORWARD_METHOD(Transaction, void, commit, (), (), OrmException)

} // namespace
}

