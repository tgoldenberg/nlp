var path = require('path');
var fs = require('fs');
var countsFile = '../gene.counts';
var geneFile = '../gene.dev';

function splitFile(fileName){
  var file = fs.readFileSync(fileName, "utf8");
  var lines = file.split("\n");
  var result = [];
  for(i=0; i<lines.length; i++){
    var line = lines[i];
    if (!! line ) {
      result.push(line);
    }
  }
  return result;
}

function Tagger(){
  this.n = 3;
  this.emissionCounts = {};
  this.ngramCounts = [{}, {}, {}];
  this.allStates = [];
  this.neTagCounts = {};
}

Tagger.prototype.readCounts = function(iterator, output){
  this.n = 3;
  this.emissionCounts = {};
  this.ngramCounts = [{}, {}, {}];
  this.allStates = [];
  for(i=0; i<iterator.length; i++){
    var line = iterator[i];
    var parts = line.split(" ");
    var count = parseInt(parts[0]);
    if (parts[1] === "WORDTAG") {
      var  neTag = parts[2];
      var word = parts[3];
      var key = word + ':' + neTag
      this.emissionCounts[key] = count;
      if (this.allStates.indexOf(neTag) == -1) {
        this.allStates.push(neTag)
      }
      if (this.neTagCounts[neTag] > 0) {
        this.neTagCounts[neTag] += count;
      } else {
        this.neTagCounts[neTag] = 1;
      }
    } else if (!! parts[1].match(/GRAM$/)) {
      var n = parseInt(parts[1].split("-GRAM")[0]);
      var end = parts.length
      var ngram = parts.slice(2, end).join(" ");
      this.ngramCounts[n-1][ngram] = count;
    }
  }
}

Tagger.prototype.countTag = function(word) {
  var count = 0;
  var c = 0;
  var rare = true;
  var neTag;
  var allStates = this.allStates
  var emissionCounts = this.emissionCounts;
  var neTagCounts = this.neTagCounts;
  allStates.forEach(function(tag){
    var key = word + ':' + tag;
    if (emissionCounts[key] > 0) {
      rare = false;
      c = emissionCounts[key] / neTagCounts[tag];
    }
    if (count < c) {
      count = c;
      neTag = tag;
    }
  })
  if (rare == true ) {
     return '0';
  } else {
    return neTag;
  }
  return count
}

Tagger.prototype.writeTags = function(iterator, output){
  for(i=0; i<iterator.length; i++){
    var word = iterator[i];
    console.log('WORD', word);
    var tag = this.countTag(word);
    console.log('TAG', tag);
  }
}

var tagger = new Tagger();
var countsIterator = splitFile(countsFile);
var geneIterator = splitFile(geneFile);
tagger.readCounts(countsIterator);
tagger.writeTags(geneIterator);
