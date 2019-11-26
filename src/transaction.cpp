/*
 * Copyright 2016, alex at staticlibs.net
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* 
 * File:   transaction.cpp
 * Author: alex
 * 
 * Created on May 28, 2016, 11:48 PM
 */

#include "staticlib/orm/transaction.hpp"

#ifdef SOCI_HAVE_BOOST
#undef SOCI_HAVE_BOOST
#endif //SOCI_HAVE_BOOST
#include "soci.h"

#include "staticlib/pimpl/forward_macros.hpp"

namespace staticlib {
namespace orm {

class transaction::impl : public sl::pimpl::object::impl {
    soci::session& session;
    soci::transaction tran;
    
public:

    virtual ~impl() STATICLIB_NOEXCEPT { };
    
    impl(void* session) :
    session(*static_cast<soci::session*> (session)),
    tran(this->session) { }

    void commit(transaction&) {
        tran.commit();
    }

};
PIMPL_FORWARD_CONSTRUCTOR(transaction, (void*), (), orm_exception)
PIMPL_FORWARD_METHOD(transaction, void, commit, (), (), orm_exception)

} // namespace
}

