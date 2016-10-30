#!/usr/bin/python3
import re
import sys

if len(sys.argv) < 2:
    print('Usage: gen.py FILENAME')
    sys.exit(1)
try:
    main = open(sys.argv[1]).read()
except FileNotFoundError:
    print('File {} not found'.format(sys.argv[1]))
    sys.exit(2)
repl = re.compile(r'#include "([a-z_]+\.h)"')
while True:
    old = main
    main = repl.sub(lambda x:open('include/' + x.group(1)).read(), main)
    if old == main:
        break
with open('gen.cpp', 'w') as f:
    f.write(main)
