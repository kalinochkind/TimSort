#!/usr/bin/python3
import re

main = open('main.cpp').read()
repl = re.compile(r'#include "([a-z_]+\.h)"')
while True:
    old = main
    main = repl.sub(lambda x:open(x.group(1)).read(), main)
    if old == main:
        break
with open('gen.cpp', 'w') as f:
    f.write(main)
