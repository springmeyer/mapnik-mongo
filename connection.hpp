/*****************************************************************************
 *
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2011 Artem Pavlenko
 *               2013 Oleksandr Novychenko
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

#ifndef MONGODB_CONNECTION_HPP
#define MONGODB_CONNECTION_HPP

// mongo
#include <mongo/client/dbclient.h>
#include <mongo/client/connpool.h>
#include <mongo/client/dbclientcursor.h>

// boost
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

// std
#include <sstream>
#include <iostream>

class Connection {
    boost::scoped_ptr<mongo::ScopedDbConnection> conn_;
    std::string ns_;
    bool closed_;

public:
    Connection(const std::string &connection_str, const std::string ns)
        : ns_(ns), closed_(false) {
        try {
            conn_.reset(mongo::ScopedDbConnection::getScopedDbConnection(connection_str));
        } catch (mongo::DBException &de) {
            std::string err_msg = "Mongodb Plugin: ";
            err_msg += de.toString();
            err_msg += "\n";
            throw mapnik::datasource_exception(err_msg);
        }
    }

    ~Connection() {
        close();
    }

    boost::shared_ptr<mongo::DBClientCursor> query(const std::string &json, int limit = 0, int skip = 0) {
        try {
            mongo::DBClientCursor *ptr = conn_->get()->query(ns_, mongo::Query(json), limit, skip).release();

            if (!ptr)
                throw conn_->get()->getLastError();

            return boost::shared_ptr<mongo::DBClientCursor>(ptr);
        } catch(mongo::DBException &de) {
            std::string err_msg = "Mongodb Plugin: ";
            err_msg += de.toString();
            err_msg += "\n";
            throw mapnik::datasource_exception(err_msg);
        }
    }

    bool isOK() const {
        return (!closed_) && (conn_->ok());
    }

    void close() {
        if (!closed_) {
            conn_->done();
            closed_ = true;
        }
    }
};

#endif // MONGODB_CONNECTION_HPP
