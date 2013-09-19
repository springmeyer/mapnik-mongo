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

// mapnik
#include <mapnik/global.hpp>
#include <mapnik/box2d.hpp>
#include <mapnik/geometry.hpp>
#include <mapnik/feature.hpp>
#include <mapnik/feature_layer_desc.hpp>
#include <mapnik/unicode.hpp>
#include <mapnik/params.hpp>

// std
#include <string>

#include "mongodb_converter.hpp"

using mapnik::feature_ptr;
using mapnik::geometry_type;

void mongodb_converter::convert_geometry(const mongo::BSONElement &loc, feature_ptr feature) {
    std::string type = loc["type"].String();
    std::vector<mongo::BSONElement> coords = loc["coordinates"].Array();

    if (type == "Point")
        convert_point(coords, feature);
    else if (type == "LineString")
        convert_linestring(coords, feature);
    else if (type == "Polygon")
        convert_polygon(coords, feature);
}

void mongodb_converter::convert_point(const std::vector<mongo::BSONElement> &coords, feature_ptr feature) {
    std::auto_ptr<geometry_type> point(new geometry_type(mapnik::Point));
    point->move_to(coords[0].Number(), coords[0].Number());

    feature->paths().push_back(point);
}

void mongodb_converter::convert_linestring(const std::vector<mongo::BSONElement> &coords, feature_ptr feature) {
    std::vector<mongo::BSONElement> point = coords[0].Array();
    int num_points = coords.size();
    std::auto_ptr<geometry_type> line(new geometry_type(mapnik::LineString));

    line->move_to(point[0].Number(), point[1].Number());
    for (int i = 1; i < num_points; ++i) {
        point = coords[i].Array();
        line->line_to(point[0].Number(), point[1].Number());
    }

    feature->paths().push_back(line);
}

void mongodb_converter::convert_polygon(const std::vector<mongo::BSONElement> &coords, feature_ptr feature) {
    std::vector<mongo::BSONElement> ring = coords[0].Array(), point = ring[0].Array();
    int num_points = ring.size();
    int num_interior = coords.size() - 1; // minus exterior
    std::auto_ptr<geometry_type> poly(new geometry_type(mapnik::Polygon));

    poly->move_to(point[0].Number(), point[1].Number());
    for (int i = 1; i < num_points; ++i) {
        point = ring[i].Array();
        poly->line_to(point[0].Number(), point[1].Number());
    }
    poly->close_path();

    for (int r = 0; r < num_interior; ++r) {
        ring = coords[r + 1].Array(); // coords[0] is exterior
        num_points = ring.size();

        point = ring[0].Array();
        poly->move_to(point[0].Number(), point[1].Number());
        for (int i = 1; i < num_points; ++i) {
            point = ring[i].Array();
            poly->line_to(point[0].Number(), point[1].Number());
        }
        poly->close_path();
    }

    feature->paths().push_back(poly);
}
