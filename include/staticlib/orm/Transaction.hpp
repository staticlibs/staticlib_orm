/* 
 * File:   Transaction.hpp
 * Author: alex
 *
 * Created on May 28, 2016, 5:56 PM
 */

#ifndef STATICLIB_ORM_TRANSACTION_HPP
#define	STATICLIB_ORM_TRANSACTION_HPP

#include <string>

#include "staticlib/pimpl.hpp"
#include "staticlib/serialization/JsonValue.hpp"

#include "staticlib/orm/OrmException.hpp"

namespace staticlib {
namespace orm {

class Connection;

class Transaction : public staticlib::pimpl::PimplObject {
protected:
    /**
     * Implementation class
     */
    class Impl;    
    
public:
    /**
     * PIMPL-specific constructor
     * 
     * @param pimpl impl object
     */
    PIMPL_CONSTRUCTOR(Transaction)
    
    void commit();
    
// private api
    Transaction(void* /* soci::session* */ session);
    
    
};

} // namespace
}

#endif	/* STATICLIB_ORM_TRANSACTION_HPP */

