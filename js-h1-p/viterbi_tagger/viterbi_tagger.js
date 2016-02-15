var path = require('path');
var fs = require('fs');
var countsFile = '../gene.counts';
var geneFile = '../gene.dev';
var filter = [
  ['_NUMERIC_', "[0-9]+"],
  ['_ALL_CAPITALS_', "^[A-Z]+$"],
  ['_LAST_CAPITAL_', "[A-Z]+$"]
];

function splitFile(fileName){
  var file = fs.readFileSync(fileName, 'utf8');
  var lines = file.split("\n");
  return lines.map(function(line, idx){
    return line.trim();
  })
}

function splitSentence(file){
  // console.log('FILE', file)
  var currentSentence = [];
  file.forEach(function(line, idx){
    if (! line ) {
      if (!! currentSentence) {
        return currentSentence;
        currentSentence = [];
      } else {
        console.log('WARNING: empty input stream');
      }
    } else {
      currentSentence.push(line);
    }
  })
  if (!! currentSentence) {
    return currentSentence;
  }
}

function ViterbiTagger(sentence, tagger){
  this.tagger = tagger;
  this.sentence = sentence;
  this.pi = {};
  this.bp = {};
}

ViterbiTagger.prototype.getFactor = function(qArgs, eArgs){
  var q = this.getQ(qArgs);
  var e = this.getE(eArgs);
  return q * e;
}

ViterbiTagger.prototype.getQ = function(qArgs){
  var w = qArgs[0];
  var u = qArgs[1];
  var v = qArgs[2];
  return this.tagger.ngramCounts[this.tagger.n-1][w + ':' + u + ':' + v] / this.tagger.ngramCounts[this.tagger.n - 2][w + ':' + u]
}

ViterbiTagger.prototype.getE = function(eArgs){
  var word = eArgs[0];
  var v = eArgs[1];
  return this.tagger.emissionCounts[word + ':' + v] / this.tagger.neTagCounts[v];
}

ViterbiTagger.prototype.filterRareWord = function(word){
  var done = false;
  var result = word;
  if (this.tagger.wordCounts[word] >= 5){
    done = true;
    result = word;
  }
  var filters = this.tagger.rareWordsFilter;
  filters.forEach(function(filter, idx){
    var mark = filter[0];
    var regex = filter[1];
    var re = new RegExp(regex, 'g');
    if (word.match(re) != null && ! done) {
      done = true;
      result = mark;
    }
  })
  if (! done ) {
    result = '_RARE_'
  }
  return result;
}

ViterbiTagger.prototype.countStepCoeff = function(step){
  var self = this;
  var word = this.filterRareWord(this.sentence[step-1])
  if (step == 1){
    this.tagger.allStates.forEach(function(state, idx){
      self.pi[step + ':' + '*' + ':' + state] = self.getFactor(['*', '*', state], [word, state])
    })
  } else if (step == 2){
    this.tagger.allStates.forEach(function(state, i){
      self.tagger.allStates.forEach(function(nestedState, j){
        self.pi[step + ':' + state + ':' + nestedState] = self.pi[(step - 1) + ':' + '*' + ':' + nestedState] * self.getFactor(['*', state, nestedState], [word, nestedState])
      })
    })
  } else {
    this.tagger.allStates.forEach(function(state1, i){
      self.tagger.allStates.forEach(function(state2, j){
        var argMax = state2;
        self.tagger.allStates.forEach(function(state3, k){
          var pi = self.pi[(step - 1) + ':' + state3 + ':' + state2] * self.getFactor([state3, state2, state1], [word, state3])
          if (pi > self.pi[step + ':' + state2 + ':' + state3]) {
            self.pi[step + ':' + state2 + ':' + state1] = pi;
            self.bp[step + ':' + state2 + ':' + state1] = state3;
          }
        })
      })
    })
  }
}

ViterbiTagger.prototype.makeTagSequence = function(){
  this.countCoeff();
  var self = this;
  var argMax = this.tagger.allStates[this.tagger.allStates.length-1]
  var tagSequence = [argMax, argMax];
  var n = this.sentence.length;
  var maxVal = 0;
  if (n == 1){
    this.tagger.allStates.forEach(function(state, idx){
      var p = self.pi[n + ':' + '*' + ':' + state] * self.getQ(['*', state, 'STOP']);
      if (p > maxVal){
        maxVal = p;
        argMax = state;
      }
    })
    return [argMax];
  } else {
    this.tagger.allStates.forEach(function(state1, i){
      self.tagger.allStates.forEach(function(state2, j){
        var p = self.pi[n + ':' + state1 + ':' + state2] * self.getQ([state1, state2, 'STOP'])
        if (p > maxVal){
          maxVal = p;
          tagSequence = [state1, state2];
        }
      })
    })
    if (n == 2){
      return tagSequence;
    }
    var arr = [n-2, 0, -1];
    arr.forEach(function(index, i){
      var prev = self.bp[(index+2) + ':' + tagSequence[0] + ':' + tagSequence[1]]
      tagSequence.unshift(prev);
    })
    return tagSequence;
  }
}

ViterbiTagger.prototype.countCoeff = function(){
  var n = this.sentence.length;
  for(i=0; i<n; i++){
    this.countStepCoeff(i+1);
  }
}


function Tagger(n){
  this.n = n || 3;
  this.emissionCounts = {};
  this.ngramCounts = [{}, {}, {}];
  this.allStates = [];
  this.wordCounts = {};
  this.neTagCounts = {};
  this.rareWordsFilter = {};
}

Tagger.prototype.setRareWordFilter = function(rareFilter){
  this.rareWordsFilter = rareFilter;
}
Tagger.prototype.readCounts = function(iterator, output){
  var self = this;
  this.n = 3;
  this.emissionCounts = {};
  this.ngramCounts = [{},{},{}];
  this.allStates = [];
  var neTag, word;
  iterator.forEach(function(line, idx){
    var parts = line.split(" ");
    // console.log('PARTS', parts);
    var count = parseInt(parts[0]);
    if (parts[1] === "WORDTAG"){
      neTag = parts[2];
      word = parts[3];
      var key = word + ':' + neTag;
      self.emissionCounts[key] = count;
      if (self.allStates.indexOf(neTag) == -1){
        self.allStates.push(neTag);
      }
      self.neTagCounts[neTag] += count;
      self.wordCounts[word] += count;
    } else if (parts.length > 1 && !! parts[1].match(/GRAM$/)) {
      var n = parseInt(parts[1].split("-GRAM")[0]);
      var end = parts.length
      var ngram = parts.slice(2, end).join(" ");
      self.ngramCounts[n-1][ngram] = count;
    }
  })
}

Tagger.prototype.tagSentence = function(sentence, output){
  var viterbi = new ViterbiTagger(sentence, this);
  // console.log('VITERBI', viterbi)
  return viterbi.makeTagSequence();
}

Tagger.prototype.writeTags = function(iterator, output){
  var self = this;
  // console.log('SELF', this.tagSentence)
  var k = 0;
  iterator.forEach(function(sentence, idx){
    k += 1;
    // console.log('SENTENCE', sentence)
    var tagSentence = self.tagSentence(sentence, output);
    var len = sentence.length;
    for(i=0; i<len; i++){
      console.log('\n', sentence[i], tagSentence[i]);
    }
    console.log('\n');
  })
}

var tagger = new Tagger(3);
tagger.setRareWordFilter(filter);
tagger.readCounts(splitFile(countsFile), null);
tagger.writeTags(splitSentence(splitFile(geneFile)), null);
