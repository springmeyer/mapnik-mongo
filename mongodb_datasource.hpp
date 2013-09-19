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

#ifndef MONGODB_DATASOURCE_HPP
#define MONGODB_DATASOURCE_HPP

// mapnik
#include <mapnik/datasource.hpp>
#include <mapnik/params.hpp>
#include <mapnik/query.hpp>
#include <mapnik/feature.hpp>
#include <mapnik/box2d.hpp>
#include <mapnik/coord.hpp>
#include <mapnik/feature_layer_desc.hpp>
#include <mapnik/unicode.hpp>
#include <mapnik/value_types.hpp>

// boost
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>

// stl
#include <vector>
#include <string>

#include "connection_manager.hpp"

using mapnik::transcoder;
using mapnik::datasource;
using mapnik::box2d;
using mapnik::layer_descriptor;
using mapnik::featureset_ptr;
using mapnik::feature_ptr;
using mapnik::query;
using mapnik::parameters;
using mapnik::coord2d;

class mongodb_datasource : public datasource {
    const std::string uri_;
    const std::string username_;
    const std::string password_;
    layer_descriptor desc_;
    ConnectionCreator<Connection> creator_;
    bool persist_connection_;
    mutable bool extent_initialized_;
    mutable mapnik::box2d<double> extent_;

    std::string json_bbox(const box2d<double> &env) const;

public:
    mongodb_datasource(const parameters &params);
    ~mongodb_datasource();

    static const char *name();
    mapnik::datasource::datasource_t type() const;
    layer_descriptor get_descriptor() const;

    featureset_ptr features(const query &q) const;
    featureset_ptr features_at_point(coord2d const &pt, double tol = 0) const;
    mapnik::box2d<double> envelope() const;

    boost::optional<mapnik::datasource::geometry_t> get_geometry_type() const;
};

#endif // MONGODB_DATASOURCE_HPP
