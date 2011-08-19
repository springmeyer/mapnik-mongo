
import os
import sys
import pymongo
import mapnik2 as mapnik


# note: only works with point geometries

# http://api.mongodb.org/python/1.1.2%2B/index.html
# http://www.mongodb.org/pages/viewpage.action?pageId=133415


def shp_to_mongo(collection,shapefile,append=False):
    ds = mapnik.Shapefile(file=shapefile)
    if not append:
        print 'Removing features from the collection...'
        collection.remove({})
    idx = 0
    featureset = ds.featureset()
    feat = featureset.next()
    while feat:
        for i in range(0,feat.num_geometries()):
            coord = feat.get_geometry(i).envelope().center()
            loc = {'long': coord.x, 'lat': coord.y }
            collection.insert( {
                                  'name': feat['NAME'],
                                  'id': feat.id(),
                                  'loc': [coord.x,coord.y]
                                  #'long': coord.x,
                                  #'lat': coord.y
                                })
        feat = featureset.next()
        
        idx += 1
    print 'imported %s features' % idx
        
    

if __name__ == "__main__":
    connection = pymongo.connection.Connection('localhost', 27017)
    collection = connection['gisdb']['world']
    if collection:
        shp_to_mongo(collection,'10m_populated_places_simple.shp')