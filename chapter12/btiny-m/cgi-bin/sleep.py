#!/usr/bin/env python3

import cgi
import sys
import time

form = cgi.FieldStorage()
content = '<em>BTiny Lazybones:</em> '
try:
    t = int(form.getvalue('time'))
    time.sleep(t)
    content += '%d seconds' % t
except:
    content += 'invalid argument'
content_len = len(content)

sys.stdout.write('Content-length: %d\r\n' % content_len)
sys.stdout.write('Content-type: text/html\r\n')
sys.stdout.write('\r\n')
sys.stdout.write(content)
