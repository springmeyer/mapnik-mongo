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

#ifndef MONGODB_CONVERTER_HPP
#define MONGODB_CONVERTER_HPP

// mapnik
#include <mapnik/datasource.hpp>

// mongo
#include <mongo/client/dbclientcursor.h>

// std
#include <vector>

class mongodb_converter {
public:
    static void convert_geometry(const mongo::BSONElement &loc, mapnik::feature_ptr feature);

    static void convert_point(const std::vector<mongo::BSONElement> &coords, mapnik::feature_ptr feature);
    static void convert_linestring(const std::vector<mongo::BSONElement> &coords, mapnik::feature_ptr feature);
    static void convert_polygon(const std::vector<mongo::BSONElement> &coords, mapnik::feature_ptr feature);
};

#endif // MONGODB_CONVERTER_HPP
