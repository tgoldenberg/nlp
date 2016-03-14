'use strict';

let fs = require('fs');
let path = require('path');
let JSON = require('json5');

const fileName = "./cfg.counts";

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
      console.log('PARTS', parts);
    }
  });
};

getRareItems(fileName);
