'use strict';

let fs = require('fs');
let path = require('path');
let JSON = require('json5');

const fileName = "./cfg.counts";
const trainFile = './parse_train_vert.dat';
const output = 'parse_train_out.dat';

fs.writeFileSync(output, '');

let NONTERMINAL = {};
let BINARYRULE = {};
let UNARYRULE = {};

let rareItems = {};
let wordSet = {};

function getRareItems(fileName){
  var file = fs.readFileSync(fileName, 'utf8');
  var lines = file.split("\n");
  lines.forEach((line, idx) => {
    if (! line) {
      return;
    } else {
      var parts = line.split(" ");
      var len = parts.length;
      if (len == 3){
        NONTERMINAL[parts[2]] = parseInt(parts[0]);
      } else if (len == 5){
        BINARYRULE[[parts[2], parts[3], parts[4]].join(':')] = parseInt(parts[0]);
      } else if (len == 4 && parts[1] == "UNARYRULE"){
        UNARYRULE[[parts[2], parts[3]].join(':')] = parseInt(parts[0]);
        if (! wordSet[parts[3]]){
          wordSet[parts[3]] = 1;
        } else {
          wordSet[parts[3]] += 1;
        }
      }
    }
    let keys = Object.keys(wordSet);
    keys.forEach((key, idx) => {
      if (wordSet[key] < 5){
        rareItems[key] = 1;
      }
    });
  });
};

function travelLeafNode(tree){
  let word;
  if (tree.length == 2){
    word = tree[1];
    if (!! rareItems[word]){
      tree[1] = "_RARE_";
    }
    return;
  }
  if (tree.length == 3){
    travelLeafNode(tree[1]);
    travelLeafNode(tree[2]);
  }
  return;
};

function replaceRareWords(){
  var file = fs.readFileSync(trainFile, 'utf8');
  var lines = file.split("\n");
  lines.forEach((line, idx) => {
    if (! line ) {
      return;
    } else {
      var parsedLine = JSON.parse(line);
      travelLeafNode(parsedLine);
      fs.appendFile(output, JSON.stringify(parsedLine) + "\n");
    }
  });
};
getRareItems(fileName);
replaceRareWords();
// console.log('NONTERMINAL', NONTERMINAL);
// console.log('UNARYRULE', UNARYRULE);
// console.log('BINARYRULE', BINARYRULE);
// console.log('rareItems', rareItems);
// console.log('wordSet', wordSet);
