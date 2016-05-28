/* 
 * File:   ResultSet.cpp
 * Author: alex
 * 
 * Created on May 28, 2016, 11:48 PM
 */

#include "staticlib/orm/ResultSet.hpp"

#include "staticlib/pimpl/pimpl_forward_macros.hpp"

namespace staticlib {
namespace orm {

//namespace { // anonymous
//
//namespace ss = staticlib::serialization;
//
//} // namespace

class ResultSet::Impl : public staticlib::pimpl::PimplObject::Impl {

public:

    bool compute_next(ResultSet&) {
        return true;
    }
    
};
PIMPL_FORWARD_METHOD(ResultSet, bool, compute_next, (), (), OrmException)

} // namespace
}


