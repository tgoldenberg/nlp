__author__ = "Tom Goldenberg <thomasagoldenberg@gmail.com>"
__date__ = "$Feb 4, 2016"

import sys
from collections import defaultdict
import re

def corpus_iterator(corpus_file):
    l = corpus_file.readline()
    while l:
        line = l.strip()
        if line:
            fields = line.split(" ")
            ne_tag = fields[-1]
            word = " ".join(fields[:-1])
            yield(word, ne_tag)
        else:
            yield(None, None)
        l = corpus_file.readline()

class Replacer(object):
    def __init__(self):
        self.counts = defaultdict(int)
        self.rare_words_filter = {}

    def set_rare_word_filter(self, filter):
        self.rare_words_filter = filter

    def word_count(self, corpus_file, output):
        iterator = corpus_iterator(corpus_file)
        for word, ne_tag in iterator:
            if word:
                self.counts[word] += 1
        for word in self.counts.keys():
            if self.counts[word] >= 5:
                del self.counts[word]

    def filter_rare_word(self, word):
        if self.counts[word] == 0:
            return word
        for [mark, regex] in self.rare_words_filter:
            if re.search(regex, word):
                return mark
        return '_RARE_'

    def replace_rare(self, corpus_file, output):
        iterator = corpus_iterator(corpus_file)
        for word, ne_tag in iterator:
            if word is None:
                output.write('\n');
            else:
                output.write('%s %s\n' %(self.filter_rare_word(word), ne_tag ))

def usage():
    print """
    python replace_rare.py [input_file] > [output_file]
        Read in a gene tagged training input file and
        output its content with rare words replaced with _RARE_
    """

if __name__ == "__main__":
    if len(sys.argv)!=2:
        usage()
        sys.exit(2)

    try:
        input1 = file(sys.argv[1], "r")
        input2 = file(sys.argv[1], "r")
    except IOError:
        sys.stderr.write("ERROR: Cannot read inputfile %s.\n" %arg)
        sys.exit(1)

    replacer = Replacer()
    replacer.set_rare_word_filter([['_NUMERIC_', "[0-9]+"], ['_ALL_CAPITALS_', "^[A-Z]+$"], ['_LAST_CAPITAL_', "[A-Z]+$"]])

    replacer.word_count(input1, sys.stdout)

    replacer.replace_rare(input2, sys.stdout)
