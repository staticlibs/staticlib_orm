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
 * File:   connection.hpp
 * Author: alex
 *
 * Created on May 28, 2016, 5:56 PM
 */

#ifndef STATICLIB_ORM_CONNECTION_HPP
#define	STATICLIB_ORM_CONNECTION_HPP

#include <string>
#include <memory>
#include <vector>

#include "staticlib/pimpl.hpp"
#include "staticlib/json.hpp"

#include "staticlib/orm/orm_exception.hpp"
#include "staticlib/orm/transaction.hpp"

namespace staticlib {
namespace orm {

/**
 * This class manages database connection. connection lifetime is bound to RAII,
 * it will be closed on instance destruction. connections are NOT thread-safe.
 */
class connection : public sl::pimpl::object {
protected:
    /**
     * Implementation class
     */
    class impl;
public:
    /**
     * PIMPL-specific constructor
     * 
     * @param pimpl impl object
     */
    PIMPL_CONSTRUCTOR(connection)
    
    /**
     * Constructor, takes DB URL in SOCI format, see examples: https://github.com/staticlibs/external_soci/blob/160b1ff6d7012b7b9969ae7f4bed655bb6087ff0/test/soci_test.cpp#L44
     * 
     * @param url DB connection URL in SOCI format
     */
    connection(std::string url);

    /**
     * Starts a transaction bound to this connection. transactions are optional
     * for queries execution, their exact behaviour ("auto-commit" mode etc)
     * depends on a database.
     * 
     * @return transaction bound to this connection
     */
    transaction start_transaction();

    /**
     * Executes a "SELECT" query to the database
     * 
     * @param sql SQL query text
     * @param param parameters object, it will be treated as a "param_name -> param_value" mapping 
     *              if that object has a type "OBJECT". It will be treated as a list of positional
     *              parameters if that object has a type of "ARRAY".
     *              Otherwise it will be treated as a "param_value".
     *              "NULL_T" params object will be ignored.
     * @return list of query results as a vector of "json_value" objects - one object 
     *         for each returned row
     */
    std::vector<sl::json::value> query(std::string sql, const sl::json::value& param = sl::json::value{});

    /**
     * Executes a DML or DDL query to the database
     * 
     * @param sql SQL query text
     * @param param parameters object, it will be treated as a "param_name -> param_value" mapping 
     *              if that object has a type "OBJECT". It will be treated as a list of positional
     *              parameters if that object has a type of "ARRAY".
     *              Otherwise it will be treated as a "param_value".
     *              "NULL_T" params object will be ignored.
     */
    void execute(std::string sql, const sl::json::value& param = sl::json::value{});

};

} // namespace
}

#endif	/* STATICLIB_ORM_CONNECTION_HPP */

