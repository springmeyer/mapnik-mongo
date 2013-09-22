
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

#ifndef MONGODB_FEATURESET_HPP
#define MONGODB_FEATURESET_HPP

// mapnik
#include <mapnik/box2d.hpp>
#include <mapnik/datasource.hpp>
#include <mapnik/feature.hpp>
#include <mapnik/unicode.hpp>

// mongo
#include <mongo/client/dbclientcursor.h>

// boost
#include <boost/scoped_ptr.hpp>

using mapnik::Featureset;
using mapnik::box2d;
using mapnik::feature_ptr;
using mapnik::transcoder;
using mapnik::context_ptr;

class mongodb_featureset : public mapnik::Featureset {
    boost::shared_ptr<mongo::DBClientCursor> rs_;
    context_ptr ctx_;
    boost::scoped_ptr<mapnik::transcoder> tr_;
    mapnik::value_integer feature_id_;

public:
    mongodb_featureset(const boost::shared_ptr<mongo::DBClientCursor> &rs,
                       const context_ptr &ctx,
                       const std::string &encoding);
    ~mongodb_featureset();

    feature_ptr next();
};

#endif // MONGODB_FEATURESET_HPP
