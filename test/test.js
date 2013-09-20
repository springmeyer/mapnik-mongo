"use strict";

var fs = require("fs");
var mapnik = require("mapnik");

mapnik.register_datasources("..");
mapnik.register_fonts("/usr/share/fonts/truetype/ttf-dejavu");

var map = new mapnik.Map(640, 480);
map.load("test.xml", function(err, map) {
    if (err)
        return console.log("style load error:", err.message);

    map.zoomAll();

    var img = new mapnik.Image(640, 480);
    map.render(img, function(err, img) {
        if (err)
            return console.log("rendering error:", err.message);

        img.encode("png", function(err, buffer) {
            if (err)
                return console.log("encoding error:", err.message);

            fs.writeFile("result.png", buffer, function(err) {
                if (err)
                    return console.log("saving error:", err.message);

                console.log("done...");
            });
        });
    });
});
