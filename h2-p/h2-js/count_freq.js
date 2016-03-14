'use strict';

var fs = require('fs');
var path = require('path');
var JSON = require('json5');
var fileName = "./parse_train.dat";
var output = "./cfg.counts";
fs.writeFileSync(output, '');

function splitFile(fileName){
  var file = fs.readFileSync(fileName, "utf8");
  var lines = file.split("\n");
  var result = [];
  for(var i=0; i<lines.length; i++){
    var line = lines[i];
    if (!! line ) {
      result.push(line);
    }
  }
  return result;
};

function Counts(){
  this.unary = {}; // {}
  this.binary = {};
  this.nonterm = {};
};

Counts.prototype.show = function(){
  let outerKeys, innerKeys;
  outerKeys = Object.keys(this.nonterm);
  outerKeys.forEach((ok) => {
    innerKeys = Object.keys(this.nonterm[ok]);
    innerKeys.forEach((ik) => {
      let count = this.nonterm[ok][ik];
      let tag = "NONTERMINAL";
      let sym = ik;
      fs.appendFileSync(output, [count, tag, sym].join(" "));
      fs.appendFileSync(output, "\n");
    });
  });

  outerKeys = Object.keys(this.unary);
  outerKeys.forEach((ok) => {
    innerKeys = Object.keys(this.unary[ok]);
    innerKeys.forEach((ik) => {
      let count = this.unary[ok][ik];
      let word = ik;
      let tag = "UNARYRULE";
      let sym = ok;
      fs.appendFileSync(output, [count, tag, sym, word].join(" "));
      fs.appendFileSync(output, "\n");
    });
  });

  outerKeys = Object.keys(this.binary);
  outerKeys.forEach((ok) => {
    innerKeys = Object.keys(this.binary[ok])
    innerKeys.forEach((ik) => {
      let count = this.binary[ok][ik];
      let word1 = ik.split(":")[0];
      let word2 = ik.split(":")[1];
      let tag = "BINARYRULE";
      let sym = ok;
      fs.appendFileSync(output, [count, tag, sym, word1, word2].join(" "));
      fs.appendFileSync(output, "\n");
    });
  });
};

Counts.prototype.count = function(tree){
  if (! tree) {return;}
  let symbol = tree[0]; // convert to array with JSON
  if (! this.nonterm[symbol]) {
    this.nonterm[symbol] = {};
  }
  let word1, word2, key;
  if (this.nonterm[symbol][symbol] ){
    this.nonterm[symbol][symbol] += 1;
  } else {
    this.nonterm[symbol][symbol] = 1;
  }
  if (tree.length === 3){
    word1 = tree[1][0];
    word2 = tree[2][0];
    key = [word1, word2].join(':');
    if (! this.binary[symbol]) {
      this.binary[symbol] = {};
    }
    if (this.binary[symbol][key]){
      this.binary[symbol][key] += 1;
    } else {
      this.binary[symbol][key] = 1;
    }
    this.count(tree[1]);
    this.count(tree[2]);
  } else if (tree.length == 2){
    word1 = tree[1];
    key = word1;
    if (! this.unary[symbol]){
      this.unary[symbol] = {};
    }
    if (this.unary[symbol][key]){
      this.unary[symbol][key] += 1;
    } else {
      this.unary[symbol][key] = 1;
    }
  }
};

var counter = new Counts();
console.log('COUNTER', counter);
var file = splitFile(fileName);
file.forEach(function(line){
  let parsedLine = JSON.parse(line);
  counter.count(parsedLine);
});

counter.show();
