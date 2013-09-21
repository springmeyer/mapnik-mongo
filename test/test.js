"use strict";

var fs = require("fs");
var mapnik = require("mapnik");

mapnik.register_datasources("..");
mapnik.register_fonts("/usr/share/fonts/truetype/ttf-dejavu");

var conf = [
    { extent: [ -180,   0,   0,  90 ], file: "1.png" },
    { extent: [    0,   0, 180,  90 ], file: "2.png" },
    { extent: [ -180, -90,   0,   0 ], file: "3.png" },
    { extent: [    0, -90, 180,   0 ], file: "4.png" }
];

var map = new mapnik.Map(512, 256);
map.load("test.xml", function(err, map) {
    if (err)
        return console.log("style load error:", err.message);

    (function step(i) {
        if (!conf[i]) return;
        render(conf[i], function() { setImmediate(function() { step(i + 1) }); });
    })(0);
});

function render(conf, callback) {
    map.zoomToBox(conf.extent);
    console.log("extent:", map.extent, conf.extent);

    var img = new mapnik.Image(512, 256);
    map.render(img, function(err, img) {
        if (err)
            return console.log("rendering error:", err.message);

        img.encode("png", function(err, buffer) {
            if (err)
                return console.log("encoding error:", err.message);

            fs.writeFile(conf.file, buffer, function(err) {
                if (err)
                    return console.log("saving error:", err.message);

                console.log("done:", conf.file);
                return callback && callback();
            });
        });
    });
}
