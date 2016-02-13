var path = require('path');
var fs = require('fs');
var fileName = '../gene.train';
var FILTER = [
  ['_NUMERIC_', "[0-9]+"],
  ['_ALL_CAPITALS_', "^[A-Z]+$"],
  ['_LAST_CAPITAL_', "[A-Z]+$"]
];

function splitFile(fileName){
  var file = fs.readFileSync(fileName, 'utf8');
  var lines = file.split('\n');
  var result = lines.map(function(line, idx){
    line = line.trim();
    if (!! line ) {
      var fields = line.split(" ");
      var neTag = fields[fields.length-1];
      var word = fields.splice(0, fields.length-1);
      return {word: word.join(" "), tag: neTag};
    } else {
      return null;
    }
  })
  return result;
}

function Replacer() {
  this.counts = {};
  this.rareWordsFilter = [];
}

Replacer.prototype.setRareWordFilter = function(filter){
  this.rareWordsFilter = filter;
}

Replacer.prototype.wordCount = function(fileName, output){
  var self = this;
  var iterator = splitFile(fileName);
  iterator.forEach(function(line, idx){
    if (!! line && line.word){
      if (self.counts[line.word] > 0) {
        self.counts[line.word] += 1;
      } else {
        self.counts[line.word] = 1;
      }
    }
  })
  // console.log('COUNTS FIRST', this.counts)
  Object.keys(this.counts).forEach(function(key, idx){
    if (self.counts[key] >= 5){
      // console.log('KEY', key)
      delete self.counts[key];
    }
  })
  // console.log('COUNTS SECOND', this.counts)
}

Replacer.prototype.filterRareWord = function(word){
  var self = this;
  var result = word;
  if (this.counts[word] == 0) {
    return word;
  }
  this.rareWordsFilter.forEach(function(filter, idx){
    var mark = filter[0];
    var regex = filter[1];
    var re = new RegExp(regex, 'g');
    if (word.match(re)) {
      return mark;
    }
  })
  return 'RARE' + word;
}

Replacer.prototype.replaceRare = function(fileName, output){
  var self = this;
  var lines = splitFile(fileName);
  lines.forEach(function(line, idx){
    if ( line && !! line.word ) {
      console.log(self.filterRareWord(line.word), line.tag)
    }
  })
}

var replacer = new Replacer();
replacer.setRareWordFilter(FILTER)
replacer.wordCount(fileName);
replacer.replaceRare(fileName);
