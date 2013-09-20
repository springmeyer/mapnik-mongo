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

#include "mongodb_datasource.hpp"
#include "mongodb_featureset.hpp"
#include "connection_manager.hpp"

// mapnik
#include <mapnik/debug.hpp>
#include <mapnik/global.hpp>
#include <mapnik/boolean.hpp>
#include <mapnik/sql_utils.hpp>
#include <mapnik/util/conversions.hpp>
#include <mapnik/timer.hpp>
#include <mapnik/value_types.hpp>

// boost
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <boost/make_shared.hpp>

// stl
#include <string>
#include <algorithm>
#include <set>
#include <sstream>
#include <iomanip>

DATASOURCE_PLUGIN(mongodb_datasource)

using boost::shared_ptr;
using mapnik::attribute_descriptor;

mongodb_datasource::mongodb_datasource(parameters const& params)
    : datasource(params),
      desc_(*params.get<std::string>("type"), "utf-8"),
      type_(datasource::Vector),
      creator_(params.get<std::string>("host", "localhost"),
               params.get<std::string>("port", "27017"),
               params.get<std::string>("dbname", "gis"),
               params.get<std::string>("collection"),
               params.get<std::string>("user"),
               params.get<std::string>("password")),
      persist_connection_(*params.get<mapnik::boolean>("persist_connection", true)),
      extent_initialized_(false) {
    boost::optional<std::string> ext = params.get<std::string>("extent");
    if (ext && !ext->empty())
        extent_initialized_ = extent_.from_string(*ext);

    boost::optional<int> initial_size = params.get<int>("initial_size", 1);
    boost::optional<int> max_size = params.get<int>("max_size", 10);

    ConnectionManager::instance().registerPool(creator_, *initial_size, *max_size);
}

mongodb_datasource::~mongodb_datasource() {
    if (!persist_connection_) {
        shared_ptr< Pool<Connection, ConnectionCreator> > pool = ConnectionManager::instance().getPool(creator_.id());
        if (pool) {
            shared_ptr<Connection> conn = pool->borrowObject();
            if (conn)
                conn->close();
        }
    }
}

const char *mongodb_datasource::name() {
    return "mongodb";
}

mapnik::datasource::datasource_t mongodb_datasource::type() const {
    return type_;
}

layer_descriptor mongodb_datasource::get_descriptor() const {
    return desc_;
}

std::string mongodb_datasource::json_bbox(const box2d<double> &env) const {
    std::ostringstream lookup;

    lookup << "{ geometry: { \"$geoWithin\": { \"$box\": [ [ "
           << std::setprecision(16)
           << env.minx() << ", " << env.miny() << " ], [ "
           << env.maxx() << ", " << env.maxy() << " ] ] } } }";

    return lookup.str();
}

featureset_ptr mongodb_datasource::features(const query &q) const {
    const box2d<double> &box = q.get_bbox();

    shared_ptr< Pool<Connection, ConnectionCreator> > pool = ConnectionManager::instance().getPool(creator_.id());
    if (pool) {
        shared_ptr<Connection> conn = pool->borrowObject();

        if (!conn)
            return featureset_ptr();

        if (conn && conn->isOK()) {
            mapnik::context_ptr ctx = boost::make_shared<mapnik::context_type>();

            boost::shared_ptr<mongo::DBClientCursor> rs(conn->query(json_bbox(box)));
            return boost::make_shared<mongodb_featureset>(rs, ctx, desc_.get_encoding());
        }
    }

    return featureset_ptr();
}

featureset_ptr mongodb_datasource::features_at_point(const coord2d &pt, double tol) const {
    shared_ptr< Pool<Connection, ConnectionCreator> > pool = ConnectionManager::instance().getPool(creator_.id());
    if (pool) {
        shared_ptr<Connection> conn = pool->borrowObject();

        if (!conn)
            return featureset_ptr();

        if (conn->isOK()) {
            mapnik::context_ptr ctx = boost::make_shared<mapnik::context_type>();

            box2d<double> box(pt.x - tol, pt.y - tol, pt.x + tol, pt.y + tol);
            boost::shared_ptr<mongo::DBClientCursor> rs(conn->query(json_bbox(box)));
            return boost::make_shared<mongodb_featureset>(rs, ctx, desc_.get_encoding());
        }
    }

    return featureset_ptr();
}

box2d<double> mongodb_datasource::envelope() const {
    if (extent_initialized_)
        return extent_;
    else {
        // a dumb way :-/
        extent_.init(-180.0, -90.0, 180.0, 90.0);
        extent_initialized_ = true;
    }

    // shared_ptr< Pool<Connection, ConnectionCreator> > pool = ConnectionManager::instance().getPool(creator_.id());
    // if (pool) {
    //     shared_ptr<Connection> conn = pool->borrowObject();

    //     if (!conn)
    //         return extent_;

    //     if (conn->isOK()) {
    //         // query
    //     }
    // }

    return extent_;
}

boost::optional<mapnik::datasource::geometry_t> mongodb_datasource::get_geometry_type() const {
    boost::optional<mapnik::datasource::geometry_t> result;

    shared_ptr< Pool<Connection,ConnectionCreator> > pool = ConnectionManager::instance().getPool(creator_.id());
    if (pool) {
        shared_ptr<Connection> conn = pool->borrowObject();

        if (!conn)
            return result;

        if (conn->isOK()) {
            boost::shared_ptr <mongo::DBClientCursor> rs(conn->query("{ geometry: { \"$exists\": true } }", 1));
            try {
                if (rs->more()) {
                    mongo::BSONObj bson = rs->next();
                    std::string type = bson["geometry"]["type"].String();

                    if (type == "Point")
                        result.reset(mapnik::datasource::Point);
                    else if (type == "LineString")
                        result.reset(mapnik::datasource::LineString);
                    else if (type == "Polygon")
                        result.reset(mapnik::datasource::Polygon);
                }
            } catch(mongo::DBException &de) {
                std::string err_msg = "Mongodb Plugin: ";
                err_msg += de.toString();
                err_msg += "\n";
                throw mapnik::datasource_exception(err_msg);
            }
        }
    }

    return result;
}
