import sys


class N:
    val = '0' 
    l = 0
    r = 0
    depth = 0
    usbcode = 0

maxdepth = 0

fh = open(sys.argv[1], 'r')

line = fh.readline()
S = N()
while line:
    print line
    C = S
    line = line.replace('\n', '')
    if not line: 
        line = fh.readline()
        continue
    if line[0] == '#': 
        line = fh.readline()
        continue
    if not line.replace(' ', ''): 
        line = fh.readline()
        continue
    val, cwch, usbcode = line.split(' ')
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
    C.usbcode = usbcode
    line = fh.readline()

fh.close()

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

def levelorder(root, lookup=None):
    q = []
    res = []
    q.append(root)
    i = 0
    while q:
        node = q.pop(0)
        if lookup:
            res.append(lookup(node))
        else:
            res.append(node.val)
        i += 1
        if node.l:
            q.append(node.l)
        if node.r:
            q.append(node.r)
    return res


preamble_h = '''
#ifndef _CWCODE_H_
#define _CWCODE_H_

/* Includes: */
	#include <avr/io.h>
	#include <avr/interrupt.h>
	#include <util/atomic.h>
	#include <limits.h>

    #include <avr/pgmspace.h>
	#include <MyUSB/Common/Common.h>
'''

postamble_h = '''
extern uint16_t cw_table[] PROGMEM;

#endif
'''

preamble_c = '''#include "cw_code.h"

uint16_t cw_table[] PROGMEM = {'''


postamble_c = '''};
'''


fh = open('cw_code.h', 'w')

print >>fh, preamble_h
for val, code in (levelorder(S, lambda a: [a.val, a.usbcode])):
    if val and val != '0':
        print >>fh,"#define %s %s" % (val, code)
print >>fh,postamble_h
fh.close()
fh = open('cw_code.c', 'w')
print >>fh,preamble_c
print >>fh,','.join((levelorder(balancetree(S, maxdepth))))
print >>fh,postamble_c
fh.close()


