__author__ = "Tom Goldneberg <thomasagoldenberg@gmail.com>"
__date__ = "$February 3, 2016"

import sys
from collections import defaultdict

def corpus_iterator(corpus_file): # iterate over file lines
    l = corpus_file.readline() #pop off line from file
    while l: # while lines still left
        line = l.strip() # strip whitespace from line
        if line:
            yield line #return line if not null
        else:
            yield None # or return null
        l = corpus_file.readline() # pop off line from file


class Tagger(object):
    def __init__(self, n=3): # ngram count is set to 3
        assert n>=2, "Expecting n>=2" # ngram counts only work if 2 or more
        self.n = n # n = 3
        self.emission_counts = defaultdict(int) # emission_counts = {}
        self.ngram_counts = [defaultdict(int) for i in xrange(self.n)] # ngram_counts = [{}, {}, {}]
        self.all_states = set() # set is array of unique values - all_states = []
        self.ne_tag_counts = defaultdict(int) # ne_tag_counts = {}

    def read_counts(self, iterator, output): # read all the ngram counts
        self.n = 3 # ngram count set to 3
        self.emission_counts = defaultdict(int) # reset emission_counts
        self.ngram_counts = [defaultdict(int) for i in xrange(self.n)] # reset ngram_counts
        self.all_states = set() # reset all_states
        for line in iterator: #iterate through file lines
            # example data - 1 WORDTAG I-GENE nucleic, 5 WORDTAG O erythematosus, 6706 3-GRAM O I-GENE O
            parts = line.strip().split(" ") # split into sections
            count = float(parts[0]) # first part is word count
            if parts[1] == "WORDTAG": # if second part is a Word count
                ne_tag = parts[2]  # tag is 3rd part of string
                word = parts[3] # word is last part
                self.emission_counts[(word, ne_tag)] = count # {('erythematosus', '0'): counts}
                self.all_states.add(ne_tag) # all_states = (['0'])
                self.ne_tag_counts[ne_tag] += count # {'0': 4} add to defaultdict
            elif parts[1].endswith("GRAM"): # if data is a NGRAM count, ex. 6706 3-GRAM O I-GENE O
                n = int(parts[1].replace("-GRAM", "")) # remove -GRAM to just leave the number
                ngram = tuple(parts[2:]) # splice rest of array for ngram sequence
                self.ngram_counts[n-1][ngram] = count # [{}, {}, {('0', 'I-GENE', '0'): 4}]

    def count_tag(self, word): # take a word and find count
        count = 0 #init to zero
        c = 0 # init to zero
        rare = True # assume that line is rare unless otherwise shown
        for tag in self.all_states: # (['0', 'I-GENE']), only two tags
            if self.emission_counts[(word, tag)] > 0: # if combination of word and tag exists
                rare = False # set as not rare
                c = self.emission_counts[(word, tag)]/self.ne_tag_counts[tag] # counts of word and tag together / number of words with tag
            if count < c: # if is less than zero
                count = c # set to count
                ne_tag = tag # '0'
        if True == rare: # if word does not exist in training set
            return self.count_tag('_RARE_') # find count of _RARE_ words
        return ne_tag # return '0', 'I-GENE'

    def write_tags(self, iterator, output):
        for word in iterator: # iterate through lines
            if word: # if not null
                output.write("%s %s\n" %(word, self.count_tag(word))) # write word and either
            else:
                output.write("\n")


def usage():
    print """
    python tagger.py [counts_file] [text2label_file] > [output_file]
        Read in a gene tagged training input file and produce counts.
    """

if __name__ == "__main__":

    if len(sys.argv)!= 3:
        usage()
        sys.exit(2)

    try:
        counts_file = file(sys.argv[1], "r")
        gene_file = file(sys.argv[2], "r")
    except IOError:
        sys.stderr.write("ERROR: Cannot read inputfile %s.\n" % arg)
        sys.exit(1)


    tagger = Tagger(3)

    tagger.read_counts(corpus_iterator(counts_file), sys.stdout)

    tagger.write_tags(corpus_iterator(gene_file), sys.stdout)
