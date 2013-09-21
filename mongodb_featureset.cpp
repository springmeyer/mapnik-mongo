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
#include <mapnik/debug.hpp>
#include <mapnik/wkb.hpp>
#include <mapnik/unicode.hpp>
#include <mapnik/value_types.hpp>
#include <mapnik/feature_factory.hpp>
#include <mapnik/util/conversions.hpp>
#include <mapnik/util/trim.hpp>
#include <mapnik/global.hpp> // for int2net


// boost
#include <boost/cstdint.hpp> // for boost::int16_t
#include <boost/scoped_array.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp> // needed for wrapping the transcoder

// stl
#include <sstream>
#include <string>

#include "mongodb_featureset.hpp"
#include "mongodb_converter.hpp"

using mapnik::geometry_type;
using mapnik::byte;
using mapnik::geometry_utils;
using mapnik::feature_factory;
using mapnik::context_ptr;

mongodb_featureset::mongodb_featureset(const boost::shared_ptr<mongo::DBClientCursor> &rs,
                                       const context_ptr &ctx,
                                       const std::string &encoding)
    : rs_(rs),
      ctx_(ctx),
      tr_(new transcoder(encoding)),
      feature_id_(0) {
}

mongodb_featureset::~mongodb_featureset() {
}

feature_ptr mongodb_featureset::next() {
    while (rs_->more()) {
        mapnik::feature_ptr feature(new mapnik::Feature(ctx_, feature_id_));

        try {
            mongo::BSONObj bson = rs_->nextSafe();
            mongo::BSONElement geom = bson["geometry"];
            mongo::BSONElement prop = bson["properties"];

            if (geom.type() != mongo::Object)
                continue;

            mongodb_converter::convert_geometry(bson["geometry"], feature);

            if (prop.type() == mongo::Object)
                for (mongo::BSONObjIterator i = prop.Obj().begin(); i.more(); ) {
                    mongo::BSONElement e = i.next();
                    std::string name(e.fieldName());

                    switch (e.type()) {
                    case mongo::String:
                        feature->put_new(name, tr_->transcode(e.String().c_str()));
                        break;

                    case mongo::NumberDouble:
                        feature->put_new(name, e.Double());
                        break;

                    case mongo::NumberLong:
                        feature->put_new<mapnik::value_integer>(name, e.Long());
                        break;

                    case mongo::NumberInt:
                        feature->put_new<mapnik::value_integer>(name, e.Int());
                        break;

                    default:
                        std::cout << "ignored" << std::endl;
                        break;
                    }
                }
        } catch(mongo::DBException &de) {
            std::string err_msg = "Mongodb Plugin: ";
            err_msg += de.toString();
            err_msg += "\n";
            throw mapnik::datasource_exception(err_msg);
        }

        ++feature_id_;
        return feature;
    }

    return feature_ptr();
}
