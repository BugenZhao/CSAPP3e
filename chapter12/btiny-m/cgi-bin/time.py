#!/usr/bin/env python3

import sys
from datetime import datetime

content = '<em>BTiny Clock:</em> %s' % datetime.now()
content_len = len(content)
sys.stdout.write('Content-length: %d\r\n' % content_len)
sys.stdout.write('Content-type: text/html\r\n')
sys.stdout.write('\r\n')
sys.stdout.write(content)
