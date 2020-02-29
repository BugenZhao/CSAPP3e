#!/usr/bin/env python3

import os
import sys
import time

content = '<em>BTiny Lazybones:</em> '
query = os.getenv('QUERY_STRING')
try:
    t = int(query.split('&')[0])
    time.sleep(t)
    content += '%d seconds' % t
except:
    content += 'invalid argument'
content_len = len(content)

sys.stdout.write('Content-length: %d\r\n' % content_len)
sys.stdout.write('Content-type: text/html\r\n')
sys.stdout.write('\r\n')
sys.stdout.write(content)
