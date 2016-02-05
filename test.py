import nltk
sentence = "Lose 20 lbs. in one month."
tokens = nltk.word_tokenize(sentence)

print tokens

tagged = nltk.pos_tag(tokens)

print tagged

entities = nltk.chunk.ne_chunk(tagged)

print entities
