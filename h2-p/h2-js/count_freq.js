'use strict';

var fs = require('fs');
var path = require('path');

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
}

function Counts(){
  this.unary = {}; // {}
  this.binary = {};
  this.nonterm = {};
};

Counts.prototype.show = function(){
  let outerKeys, innerKeys;
  outerKeys = Object.keys(this.nonterm);
  outerKeys.forEach(function(ok){
    innerKeys = Object.keys(this.nonterm[ok]);
    innerKeys.forEach(function(ik){
      let count = this.nonterm[ok][ik];
      let tag = "NONTERMINAL";
      let sym = ik;
      console.log(count, tag, sym);
    });
  });

  outerKeys = Object.keys(this.unary);
  outerKeys.forEach(function(ok){
    innerKeys = Object.keys(this.unary[ok]);
    innerKeys.forEach(function(ik){
      let count = this.unary[ok][ik];
      let word = this.unary[ik];
      let tag = "UNARYRULE";
      let sym = ok;
      console.log(count, tag, sym, word);
    });
  });

  outerKeys = Object.keys(this.binary);
  outerKeys.forEach(function(ok){
    innerKeys = Object.keys(this.binary[ok])
    innerKeys.forEach(function(ik){
      let count = this.binary[ok][ik];
      let word1 = ik.split(":")[0];
      let word2 = ik.split(":")[1];
      let sym = ok;
      console.log(count, "BINARYRULE", sym, word1, word2);
    });
  });
};

Counts.prototype.count = function(tree){
  console.log('TREE', tree);
  if (! tree) {return;}
  let symbol = tree[0]; // convert to array with JSON
  console.log('SYMBOL', symbol);
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
    if (this.nonterm[symbol][key]){
      this.nonterm[symbol][key] += 1;
    } else {
      this.nonterm[symbol][key] = 1;
    }
    this.count(tree[1]);
    this.count(tree[2]);
  } else if (tree.length == 2){
    word1 = tree[1];
    key = word1;
    if (this.unary[symbol][key]){
      this.unary[symbol][key] += 1;
    } else {
      this.unary[symbol][key] = 1;
    }
  }
};

var fileName = "../parse_train.dat";
var counter = new Counts();
console.log('COUNTER', counter);
var file = splitFile(fileName);
file.forEach(function(line){
  counter.count(line);
});
console.log('FINAL COUNTER', counter);
