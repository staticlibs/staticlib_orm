/* 
 * File:   ResultSet.hpp
 * Author: alex
 *
 * Created on May 28, 2016, 5:57 PM
 */

#ifndef STATICLIB_ORM_RESULTSET_HPP
#define	STATICLIB_ORM_RESULTSET_HPP

#include "staticlib/ranges/range_adapter.hpp"
#include "staticlib/pimpl.hpp"
#include "staticlib/serialization/JsonValue.hpp"

#include "staticlib/orm/OrmException.hpp"

namespace staticlib {
namespace orm {

class ResultSet : public staticlib::pimpl::PimplObject,
        public staticlib::ranges::range_adapter<ResultSet, staticlib::serialization::JsonValue> {
protected:
    class Impl;

public:

    ResultSet(std::nullptr_t, std::unique_ptr<staticlib::pimpl::PimplObject::Impl> pimpl) STATICLIB_NOEXCEPT :
    staticlib::pimpl::PimplObject(nullptr, std::forward<std::unique_ptr<staticlib::pimpl::PimplObject::Impl>>(pimpl)) { }
        
    ResultSet(const ResultSet&) = delete;

    ResultSet& operator=(const ResultSet&) = delete;
    
    ResultSet(ResultSet&& other);
    
    ResultSet& operator=(ResultSet&& other);
    
    virtual ~ResultSet() STATICLIB_NOEXCEPT;

protected:
    bool compute_next();
    
};

} // namespace
}


#endif	/* STATICLIB_ORM_RESULTSET_HPP */

