#!/usr/bin/env python3

import cgi
import sys

form = cgi.FieldStorage()
content = ''
try:
    name = form.getvalue('name')
    content += '<em>Hello, %s!</em>' % name
except:
    content += 'invalid argument'
content_len = len(content)

sys.stdout.write('Content-length: %d\r\n' % content_len)
sys.stdout.write('Content-type: text/html\r\n')
sys.stdout.write('\r\n')
sys.stdout.write(content)
