import sys


class N:
    val = '0' 
    l = 0
    r = 0
    depth = 0

maxdepth = 0

line = sys.stdin.readline()
S = N()
while line:
    C = S
    line = line.replace('\n', '')
    val, cwch = line.split(' ')
    print val
    print cwch
    if len(cwch) > maxdepth:
        maxdepth = len(cwch)
        print "\"%s\"is %d long" % (cwch, len(cwch))
    for ch in cwch:
        if ch == '.':
            if C.l == 0:
                C.l = N()
                C.l.depth = C.depth + 1
            C = C.l
        if ch == '-':
            if C.r == 0:
                C.r = N()
                C.r.depth = C.depth + 1
            C = C.r
    C.val = val
    line = sys.stdin.readline()


def balancetree(node, depth):
    cdepth = 0
    q = [node]
    while q:
        cnode = q.pop(0)
        if cnode.depth == depth:
            break
        if cnode.l == 0:
            cnode.l = N()
            cnode.l.depth = cnode.depth + 1
        if cnode.r == 0:
            cnode.r = N()
            cnode.r.depth = cnode.depth + 1
        q.append(cnode.l)
        q.append(cnode.r)
    return node

def inorder(node):
    if node.l != 0:
        inorder(node.l)
    print node.val
    if node.r != 0:
        inorder(node.r)

def levelorder(root):
    q = []
    res = []
    q.append(root)
    i = 0
    while q:
        node = q.pop(0)
        res.append(node.val)
        i += 1
        if node.l:
            q.append(node.l)
        if node.r:
            q.append(node.r)
    return res

print "maxdepth is %d" % maxdepth
print (levelorder(balancetree(S, maxdepth)))
print ','.join((levelorder(balancetree(S, maxdepth))))
