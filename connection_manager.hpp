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

#ifndef MONGODB_CONNECTION_MANAGER_HPP
#define MONGODB_CONNECTION_MANAGER_HPP

#include "connection.hpp"

// mapnik
#include <mapnik/pool.hpp>
#include <mapnik/utils.hpp>

// boost
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

// stl
#include <string>
#include <sstream>

using mapnik::Pool;
using mapnik::singleton;
using mapnik::CreateStatic;

template <typename T>
class ConnectionCreator {
    boost::optional<std::string> host_, port_;
    boost::optional<std::string> dbname_, collection_;
    boost::optional<std::string> user_, pass_;

public:
    ConnectionCreator(const boost::optional<std::string> &host,
                      const boost::optional<std::string> &port,
                      const boost::optional<std::string> &dbname,
                      const boost::optional<std::string> &collection,
                      const boost::optional<std::string> &user,
                      const boost::optional<std::string> &pass)
        : host_(host), port_(port),
          dbname_(dbname), collection_(collection),
          user_(user), pass_(pass) {}

    T* operator()() const {
        return new T(connection_string(), namespace_string());
    }

    inline std::string id() const {
        return connection_string() + " " + namespace_string();
    }

    inline std::string connection_string() const {
        std::string rs = *host_;

        if (port_ && !port_->empty())
            rs += ":" + *port_;

        return rs;
    }

    inline std::string namespace_string() const {
        return *dbname_ + "." + *collection_;
    }
};

class ConnectionManager : public singleton <ConnectionManager,CreateStatic> {
    friend class CreateStatic<ConnectionManager>;
    typedef Pool<Connection, ConnectionCreator> PoolType;
    typedef std::map<std::string, boost::shared_ptr<PoolType> > ContType;
    typedef boost::shared_ptr<Connection> HolderType;
    ContType pools_;

public:
    bool registerPool(const ConnectionCreator<Connection> &creator, size_t initialSize, size_t maxSize) {
        ContType::const_iterator itr = pools_.find(creator.id());

        if (itr != pools_.end()) {
            itr->second->set_initial_size(initialSize);
            itr->second->set_max_size(maxSize);
        } else
            return pools_.insert(
                std::make_pair(creator.id(), boost::make_shared<PoolType>(creator, initialSize,maxSize))).second;

        return false;

    }

    boost::shared_ptr<PoolType> getPool(std::string const& key) {
        ContType::const_iterator itr = pools_.find(key);

        if (itr != pools_.end())
            return itr->second;

        static const boost::shared_ptr<PoolType> emptyPool;
        return emptyPool;
    }

    ConnectionManager() {}

private:
    ConnectionManager(const ConnectionManager&);
    ConnectionManager &operator=(const ConnectionManager);
};

#endif // MONGODB_CONNECTION_MANAGER_HPP
