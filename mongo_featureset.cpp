#include "mongo_featureset.hpp"
#include <mapnik/geometry.hpp>

using namespace mongo;

mongo_featureset::mongo_featureset(boost::shared_ptr<mongo::DBClientCursor> rs, std::string const& encoding)
  : count_(0),
    rs_(rs),
    tr_(new mapnik::transcoder(encoding)) { }

mongo_featureset::~mongo_featureset() { }

mapnik::feature_ptr mongo_featureset::next()
{
    if (rs_->more())
    {

        // create a new feature
        mapnik::feature_ptr feature(new mapnik::Feature(count_));

        // http://api.mongodb.org/cplusplus/1.7.5-pre-/index.html
        // http://www.mongodb.org/pages/viewpage.action?pageId=133409
        // https://github.com/mongodb/mongo/blob/master/bson/bsondemo/bsondemo.cpp
        BSONObj p = rs_->next();
        BSONElement loc = p["loc"];

        // create an attribute pair of key:value
        UnicodeString ustr = tr_->transcode(p["name"].String().c_str());
        boost::put(*feature,"key",ustr);
        
        // create a new point geometry
        mapnik::geometry_type * pt = new mapnik::geometry_type(mapnik::Point);
        
        // we use path type geometries in Mapnik to fit nicely with AGG and Cairo
        // here we stick an x,y pair into the geometry using move_to()
        vector<BSONElement> v = loc.Array();
        double x = v[0].Number();
        double y = v[1].Number();
        pt->move_to(x,y);
        
        // add the geometry to the feature
        feature->add_geometry(pt);
        
        // increment to count so that we only return one feature
        ++count_;
        
        // return the feature!
        return feature;
    }
    
    // otherwise return an empty feature_ptr
    return mapnik::feature_ptr();
}

