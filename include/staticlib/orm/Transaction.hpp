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

/**
 * This class manages a database transaction. Transactions are optional
 * for queries execution, their exact behaviour ("auto-commit" mode etc)
 * depends on a database. Transactions lifetime is bound to RAII,
 * it must be committed explicitely, otherwise it will be rolled-back 
 * on instance destruction. Transactions are NOT thread-safe.
 */
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
    
    /**
     * Commits this transaction in the database
     */       
    void commit();
    
    /**
     * Implementation-internal constructor, not a part of a public API
     * 
     * @param session internal
     */
    Transaction(void* /* soci::session* */ session);
    
    
};

} // namespace
}

#endif	/* STATICLIB_ORM_TRANSACTION_HPP */

