__author__ = "Tom Goldenberg <thomasagoldenberg@gmail.com>"
__date__ = "$February 3, 2016"

import sys
from collections import defaultdict

# iterate over file lines
def corpus_iterator(corpus_file):
    l = corpus_file.readline() # pop off a line of the file
    while l: # while there are lines left
        line = l.strip() # strip the whitespace from the line
        if line: # if line is not null
            fields = line.split(" ") #split string into an array
            ne_tag = fields[-1] # set end word as GENE tag
            word = " ".join(fields[:-1]) # actual word is preceding part of string
            yield(word, ne_tag) #yield the string and its tag
        else:
            yield (None, None) # yield null if null
        l = corpus_file.readline() # pop off another line from the file

class Replacer(object):
    def __init__(self): # initialize class
        self.counts = defaultdict(int) # counts = {} with default value set to zero

    def word_count(self, corpus_file, output): # assign each word to a count in count object
        iterator = corpus_iterator(corpus_file) # iterate through file lines
        for word, ne_tag in iterator: # refer to word and tag
            if word: # if word is not null
                self.counts[word] += 1 # word count adds 1
        for word in self.counts.keys(): # iterate through
            if self.counts[word] > 4: # if word counts is greater than 4
                del self.counts[word] # delete the word field from the object
        print self.counts

    def replace_rare(self, corpus_file, output):
        iterator = corpus_iterator(corpus_file) # iterate through file lines
        for word, ne_tag in iterator: # access word and tag
            if word is None: # if word is null
                output.write('\n'); # write empty line
            else:
                if self.counts[word] > 0: # if count is greater than zero
                    output.write('%s %s\n' %('_RARE_', ne_tag)) # replace the word with _RARE_
                else: # if count is zero
                    output.write('%s %s\n' %(word, ne_tag)) # write the word and tag

def usage():
    print """
    python replace_rare.py [input_file] > [output_file]
        Read in a gene tagged training input file and
        output its content with rare words replaced with _RARE_.
    """

if __name__ == "__main__":

    if len(sys.argv) != 2: # command should have 2 arguments - input and output files
        usage() # exit the program if no args
        sys.exit(2)

    try:
        input1 = file(sys.argv[1], "r") # assign 1st arg and 2nd arg to files
        input2 = file(sys.argv[1], "r")
    except IOError: # write error if cannot process files
        sys.stderr.write("ERROR: Cannot read inputfile %s.\n" % arg)
        sys.exit(1)

    replacer = Replacer() # initialize class Replacer with defaultdict self.counts

    replacer.word_count(input1, sys.stdout) # create counter of word frequency

    replacer.replace_rare(input2, sys.stdout) # replace rare words with _RARE_
