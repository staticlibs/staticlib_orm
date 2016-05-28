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

#include "staticlib/orm/InputParams.hpp"
#include "staticlib/orm/OrmException.hpp"
#include "staticlib/orm/ResultSet.hpp"

namespace staticlib {
namespace orm {

class Connection;

class Transaction : public staticlib::pimpl::PimplObject {
    friend class Connection;
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
    
    ResultSet query(std::string sql, const staticlib::serialization::JsonValue& param = 
            staticlib::serialization::JsonValue{});

    ResultSet query(std::string sql, const InputParams& paramsRange);

    void execute(std::string sql, const staticlib::serialization::JsonValue& param =
            staticlib::serialization::JsonValue{});

    void execute(std::string sql, const InputParams& paramsRange);

    void commit();
    
};

} // namespace
}

#endif	/* STATICLIB_ORM_TRANSACTION_HPP */

