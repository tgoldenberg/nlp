import json
import datetime
nonterminal_count = {}
nonterminal = []
binaryRule = {}
binaryRule_count = {}
unaryRule_count = {}
unaryRule = {}
word_set = {}

q = {}

def getParameters(FIFE = 'parse_train.counts.out'):
    '''
    Get parameters from proceed counts file containing _RARE_
    '''
    f = open(FIFE, 'r')
    for line in f :
        item = line.split()
        if item[1] == 'NONTERMINAL' :
            nonterminal.append(item[2])
            nonterminal_count[item[2]] = int(item[0])
        elif item[1] == 'BINARYRULE' :
            if binaryRule.get(item[2]) == None :
                binaryRule[item[2]] = [(item[3],item[4])]
            else :
                binaryRule[item[2]].append((item[3],item[4]))
            binaryRule_count[(item[2],item[3],item[4])] = int(item[0])
        elif item[1] == 'UNARYRULE' :
            word_set[item[3]] = True
            if unaryRule.get(item[2]) == None :
                unaryRule[item[2]] = [item[3]]
            else :
                unaryRule[item[2]].append(item[3])
            unaryRule_count[(item[2],item[3])] = int(item[0])

    for rule in binaryRule :
        rule_value_list = binaryRule[rule]
        for rule_value in rule_value_list :
            q[(rule,) + rule_value] = float(binaryRule_count[(rule,) + rule_value]) / float(nonterminal_count[rule])

    for rule in unaryRule :
        rule_value_list = unaryRule[rule]
        for rule_value in rule_value_list :
            q[(rule,rule_value)] = float(unaryRule_count[(rule,rule_value)]) / float(nonterminal_count[rule])


def constructJsonTree(low, high, X, sentence, bp, tree) :
    if low >= high :
        tree.append(X)
        tree.append(sentence[low-1])
        return
    Y,Z,s = bp[(low,high,X)]
    tree.append(X)
    tree.append([])
    tree.append([])
    constructJsonTree(low,s,Y,sentence,bp,tree[-2])
    constructJsonTree(s+1,high,Z,sentence,bp,tree[-1])



def CKY(FIFE = 'parse_dev.dat',PARSE_OUT = 'parse_dev.out') :
    f = open(FIFE, 'r')
    parse_out = open(PARSE_OUT,'w')
    for sentence in f :
        pi = {}
        bp = {}
        words = sentence.split()
        words_len = len(words)
        # Initialization
        for i in range(1,words_len+1) :
            word = words[i-1]
            for X in nonterminal :
                if word_set.get(word) == None :
                    word = '_RARE_'
                if q.get((X,word)) == None :
                    pi[(i,i,X)] = 0.0
                else :
                    pi[(i,i,X)] = q[(X,word)]
        #Algorithm
        for l in range(1,words_len) :
            for i in range(1,words_len + 1 - l) :
                j = i + l
                for X in nonterminal :
                    max_val = 0.0
                    max_backpointers = None
                    if binaryRule.get(X) == None :
                        pi[(i,j,X)] = 0.0
                        continue
                    for rule in binaryRule.get(X) :
                        for s in range(i,j) :
                            Y,Z = rule
                            tmp = q[(X,Y,Z)] * pi[(i,s,Y)] * pi[(s+1,j,Z)]
                            if max_val < tmp :
                                max_val = tmp
                                max_backpointers = (Y,Z,s)
                    if max_backpointers != None :
                        bp[(i,j,X)] = max_backpointers
                    pi[(i,j,X)] = max_val
        tree = []
        constructJsonTree(1,words_len,'SBARQ',words,bp,tree)
        parse_out.write(json.dumps(tree) + '\n')
    parse_out.close()
    f.close()



if __name__ == '__main__' :
    getParameters()
    start = datetime.datetime.now()
    #CKY()
    CKY('parse_test.dat','parse_test.p3.out')
    # CKY()
    end = datetime.datetime.now()
    print end-start
