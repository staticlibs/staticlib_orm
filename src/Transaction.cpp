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

