//#include <mongo/client/dbclient.h>

#include "mongo_datasource.hpp"
#include "mongo_featureset.hpp"

using mapnik::datasource;
using mapnik::parameters;

using namespace mongo;

DATASOURCE_PLUGIN(mongo_datasource)

//http://blog.mxunit.org/2010/11/simple-geospatial-queries-with-mongodb.html

mongo_datasource::mongo_datasource(parameters const& params, bool bind)
   : datasource(params),
     type_(datasource::Vector),
     desc_(*params_.get<std::string>("type"), *params_.get<std::string>("encoding","utf-8")),
     extent_()
{
    if (bind)
    {
        this->bind();
    }
}

void mongo_datasource::bind() const
{
    if (is_bound_) return;
    
    // every datasource must have some way of reporting its extent
    // in this case we are not actually reading from any data so for fun
    // let's just create a world extent in Mapnik's default srs:
    // '+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs' (equivalent to +init=epsg:4326)
    // see http://spatialreference.org/ref/epsg/4326/ for more details
    extent_.init(-179.9,-89.9,179.9,89.9);
    
    is_bound_ = true;
}

mongo_datasource::~mongo_datasource() { }

// This name must match the plugin filename, eg 'mongo.input'
std::string const mongo_datasource::name_="mongo";

std::string mongo_datasource::name()
{
    return name_;
}

int mongo_datasource::type() const
{
    return type_;
}

mapnik::box2d<double> mongo_datasource::envelope() const
{
    if (!is_bound_) bind();

    return extent_;
}

mapnik::layer_descriptor mongo_datasource::get_descriptor() const
{
    if (!is_bound_) bind();
   
    return desc_;
}

mapnik::featureset_ptr mongo_datasource::features(mapnik::query const& q) const
{
    if (!is_bound_) bind();
    
    mapnik::box2d<double> const& e = q.get_bbox();

    DBClientConnection c;

    try {
        c.connect("localhost");
        std::cout << "connected ok" << std::endl;
    } catch( DBException &e ) {
        throw mapnik::datasource_exception("Mongodb Plugin: could not connect to localhost");
    }
      
    // if the query box intersects our world extent then query for features
    if (extent_.intersects(q.get_bbox()))
    {

        std::ostringstream lookup;
        lookup << "{loc : {'$within' : {'$box' : [["
          << std::setprecision(16)
          << e.minx() << "," << e.miny() << "],["
          << e.maxx() << "," << e.maxy() << "]]}}}";
        
        std::clog << "query: " << lookup.str() << "\n";

        boost::shared_ptr<DBClientCursor> rs (c.query("gisdb.world", lookup.str()));

        return mapnik::featureset_ptr(new mongo_featureset(rs, desc_.get_encoding()));
    }


    // otherwise return an empty featureset pointer
    return mapnik::featureset_ptr();
}

mapnik::featureset_ptr mongo_datasource::features_at_point(mapnik::coord2d const& pt) const
{
    if (!is_bound_) bind();

    // features_at_point is rarely used - only by custom applications,
    // so for this sample plugin let's do nothing...
    return mapnik::featureset_ptr();
}
