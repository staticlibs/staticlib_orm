/* 
 * File:   Connection.hpp
 * Author: alex
 *
 * Created on May 28, 2016, 5:56 PM
 */

#ifndef STATICLIB_ORM_CONNECTION_HPP
#define	STATICLIB_ORM_CONNECTION_HPP

#include <string>
#include <memory>

#include "staticlib/pimpl.hpp"
#include "staticlib/serialization/JsonValue.hpp"

#include "staticlib/orm/OrmException.hpp"
#include "staticlib/orm/Transaction.hpp"

namespace staticlib {
namespace orm {

class Connection : public staticlib::pimpl::PimplObject {
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
    PIMPL_CONSTRUCTOR(Connection)
    
    Connection(std::string url);

    Transaction start_transaction();

    std::vector<staticlib::serialization::JsonValue> query(std::string sql, const staticlib::serialization::JsonValue& param =
            staticlib::serialization::JsonValue{});

    void execute(std::string sql, const staticlib::serialization::JsonValue& param =
            staticlib::serialization::JsonValue{});

};

} // namespace
}

#endif	/* STATICLIB_ORM_CONNECTION_HPP */

