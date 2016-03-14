import json
NONTERMINAL = {}
BINARYRULE = {}
UNARYRULE = {}

rareItems = {}
word_set = {}
def getRareItems(FIFE = 'cfg.counts'):
    f = open(FIFE, 'r')
    for i in f :
        item = i.split();
        itemLen = len(item)
        if itemLen == 3 :
            NONTERMINAL[item[2]] = int(item[0])
        elif itemLen == 5 :
            BINARYRULE[str([item[2],item[3],item[4]])] = int(item[0])
        elif itemLen == 4 and item[1] == 'UNARYRULE':
            UNARYRULE[str([item[2],item[3]])] = int(item[0])
            if word_set.get(item[3]) == None :
                word_set[item[3]] = int(item[0])
            else:
                word_set[item[3]] += int(item[0])
    for k in word_set :
        if word_set[k] < 5 :
            rareItems[k] = 1
    f.close()


def travelLeafNode(tree) :
    global rareItems
    if len(tree) == 2 :
        word = tree[1]
        if rareItems.get(word) != None :
            tree[1] = '_RARE_'
        return
    if len(tree) == 3 :
        travelLeafNode(tree[1])
        travelLeafNode(tree[2])
    return

def replaceRateWords( TRAINFILE = 'parse_train_vert.dat', OUTPUTFILE = "proceed_parse_train.p3.dat") :
    f = open(TRAINFILE, "r")
    outputF = open(OUTPUTFILE, "w")
    for jsonItem in f :
        jsonTree = json.loads(jsonItem)
        travelLeafNode(jsonTree)
        outputF.write(json.dumps(jsonTree) + "\n")
    f.close()
    outputF.close()

import count_cfg_freq

if __name__ == '__main__' :
    getRareItems()
    replaceRateWords()
    count_cfg_freq.main('proceed_parse_train.p3.dat')
