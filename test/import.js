"use strict";

var path = require("path"), fs = require("fs");
var mapnik = require("mapnik"), mongodb = require("mongodb");

var server = new mongodb.Server("localhost", 27017),
    connector = new mongodb.Db("gis", server, { w: 1 });

var nReady = 0;

connector.open(function(err, db) {
    if (err)
        return console.log("mongodb connect error:", err.message);

    [ "points", "linestrings", "polygons" ].forEach(function(name) {
        db.collection(name, function(err, collection) {
            if (err) {
                connector.close();
                return console.log("collection selecting error:", err.message);
            }

            collection.ensureIndex({ loc: "2dsphere" }, function(err) {
                if (err) {
                    connector.close();
                    return console.log("collection selecting error:", err.message);
                }

                importShp(name, collection);
            });
        });
    });
});

function importShp(name, collection) {
    var dataSource = new mapnik.Datasource({
            type: "shape",
            file: path.join("shp", name)
        }),
        featureSet = dataSource.featureset(), feature;

    (function next(feature) {
        if (!feature) {
            if (++nReady === 3) {
                console.log("done...");
                connector.close();
            }

            return;
        }

        var json = JSON.parse(feature.toJSON()), doc = json.properties;
        doc.loc = json.geometry;
        collection.insert(doc, function(err) { next(featureSet.next()); });
    })(featureSet.next());
}
