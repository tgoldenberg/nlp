import json
tree = json.loads(open("tree.example").readline())
print tree[2][1][1][1]
