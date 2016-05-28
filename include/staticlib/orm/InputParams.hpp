/* 
 * File:   InputParams.hpp
 * Author: alex
 *
 * Created on May 28, 2016, 5:57 PM
 */

#ifndef STATICLIB_ORM_INPUTPARAMS_HPP
#define	STATICLIB_ORM_INPUTPARAMS_HPP

#include "staticlib/ranges/range_adapter.hpp"
#include "staticlib/serialization/JsonValue.hpp"

namespace staticlib {
namespace orm {

class InputParams : public staticlib::ranges::range_adapter<InputParams, staticlib::serialization::JsonValue> {
    
public:
    InputParams(const InputParams&) = delete;
    
    InputParams& operator=(const InputParams&) = delete;
    
    virtual ~InputParams() = default;
    
protected:
    virtual bool compute_next() = 0;
    
};

} // namespace
}

#endif	/* STATICLIB_ORM_INPUTPARAMS_HPP */

