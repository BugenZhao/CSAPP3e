#!/usr/bin/env python3

import os
import sys

content = '<em>BTiny Adder:</em> '
query = os.getenv('QUERY_STRING')
try:
    if len(query) != 0:
        num_strs = query.split('&')
        num_ints = map(lambda s: int(s, base=0), num_strs)
        content += '%s = %d' % (' + '.join(num_strs), sum(num_ints))
    else:
        content += 'no arguments'
except:
    content += 'invalid argument'
content_len = len(content)

sys.stdout.write('Content-length: %d\r\n' % content_len)
sys.stdout.write('Content-type: text/html\r\n')
sys.stdout.write('\r\n')
sys.stdout.write(content)
