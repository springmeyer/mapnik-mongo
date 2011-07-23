#ifndef mongo_FEATURESET_HPP
#define mongo_FEATURESET_HPP

// mapnik
#include <mapnik/datasource.hpp>

// boost
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp> // needed for wrapping the transcoder

// mongo
#include <mongo/client/dbclient.h>

// extend the mapnik::Featureset defined in include/mapnik/datasource.hpp
class mongo_featureset : public mapnik::Featureset
{
   public:
      // this constructor can have any arguments you need
      mongo_featureset(boost::shared_ptr<mongo::DBClientCursor> rs_,
                       std::string const& encoding);
      
      // desctructor
      virtual ~mongo_featureset();
      
      // mandatory: you must expose a next() method, called when rendering
      mapnik::feature_ptr next();
   
   private:
      // members are up to you, but these are recommended
      mutable int count_;
      boost::shared_ptr<mongo::DBClientCursor> rs_;
      boost::scoped_ptr<mapnik::transcoder> tr_;
};

#endif // mongo_FEATURESET_HPP
