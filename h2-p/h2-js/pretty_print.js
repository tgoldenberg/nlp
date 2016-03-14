'use strict';

var fs = require('fs');
var path = require('path');
var JSON = require('json5');

function Node(name){
  this.name = name;
}

Node.prototype.getName = function(){
  return this.name;
};

function formatTree(tree){
  // convert a tree with strings to one with nodes
  tree[0] = new Node(tree[0]);
  let secondNode;
  if (tree.length == 2){
    tree[1] = new Node(tree[1]);
  } else if (tree.length == 3){
    formatTree(tree[1]);
    formatTree(tree[2]);
  }
};

function prettyPrint(tree){
  formatTree(tree);
  console.log(tree);
};

function init(fileName){
  var file = fs.readFileSync(fileName, 'utf8');
  file.split('\n').forEach(function(line){
    let parsedLine = JSON.parse(line);
    prettyPrint(JSON.parse(line));
  });
};

init('./tree.example');
