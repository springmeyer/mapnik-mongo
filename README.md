# mongo mapnik plugin

This is a experimental connector to MongoDB data in the form of a Mapnik C++ plugin.
MongoDB supports a Point, a LineString and a Polygon geometries since version 2.4.

# Setup

1) Install mongodb - you need libmongoclient (c++ lib)

Recommended to install from https://github.com/mongodb/mongo.git

    git clone https://github.com/mongodb/mongo.git
    cd mongo
    scons install .

I had to edit a few paths in SConstruct to get it working....

More details at http://www.mongodb.org/pages/viewpage.action?pageId=21266598

2) Start mongodb

In another terminal:

    mkdir db
    cd db
    mongod --dbpath .

3) Import a shapefile

In another terminal:

    cd test/
    python import.py

4) Run test.py

    cd test/
    python test.py


# Trouble

If you get:

    RuntimeError: field not found, expected type 2

It likely means you don't have a proper 2d index created.
