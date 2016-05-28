/* 
 * File:   OrmException.hpp
 * Author: alex
 *
 * Created on May 28, 2016, 11:05 PM
 */

#ifndef STATICLIB_ORM_ORMEXCEPTION_HPP
#define	STATICLIB_ORM_ORMEXCEPTION_HPP

#include "staticlib/config/BaseException.hpp"

namespace staticlib {
namespace orm {

/**
 * Module specific exception
 */
class OrmException : public staticlib::config::BaseException {
public:
    /**
     * Default constructor
     */
    OrmException() = default;

    /**
     * Constructor with message
     * 
     * @param msg error message
     */
    OrmException(const std::string& msg) :
    staticlib::config::BaseException(msg) { }
};

}
} // namespace


#endif	/* STATICLIB_ORM_ORMEXCEPTION_HPP */

