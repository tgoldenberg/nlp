var path = require("path")
var fs = require("fs")
var fileName = '../gene.train'; // replace with ARGV

// Iterate over training data
function splitFile(fileName) {
  var file = fs.readFileSync(fileName, "utf8")
  var lines = file.split("\n")
  var result = [];
  for (i=0; i<lines.length; i++) {
    var line = lines[i];
    var words = line.split(" ");
    var tag = words.splice(1, words.length-1);
    result.push({words: words.join(" "), tag: tag.join(" ")})
  }
  return result;
}

function Replacer(){
  this.counts = {};
}

Replacer.prototype.wordCount = function(fileName, outputfile){
  var iterator = splitFile(fileName);
  for (i=0; i<iterator.length; i++){
    var line = iterator[i];
    if (!! line.words ){
      if (this.counts[line.words] > 0 ){
        this.counts[line.words] += 1;
      } else {
        this.counts[line.words] = 1;
      }
    }
  }
  var keys = Object.keys(this.counts)
  for (i=0; i<keys.length; i++) {
    var key = keys[i];
    var count = this.counts[key];
    if (count > 4) {
      delete this.counts[key];
    }
  }
}

Replacer.prototype.replaceRare = function(fileName, outputFile){
  var iterator = splitFile(fileName);
  for(i=0; i<iterator.length; i++){
    var line = iterator[i];
    if (! line.words ) {
      console.log('\n');
    } else {
      if (this.counts[line.words] > 0) {
        console.log('_RARE_', line.tag);
      } else {
        console.log(line.words, line.tag);
      }
    }
  }
}

var replacer = new Replacer();
replacer.wordCount(fileName);
replacer.replaceRare(fileName);
