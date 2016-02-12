var path = require('path');
var fs = require('fs');

var countsFile = fs.readFileSync("../gene.counts", "utf8")
var trainFile = fs.readFileSync("../gene.train", "utf8")
var devFile = fs.readFileSync("../gene.dev", "utf8")
var keyFile = fs.readFileSync("../gene.key", "utf8")
var testFile = fs.readFileSync("../gene.test", "utf8")
