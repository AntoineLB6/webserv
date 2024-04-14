#!/usr/bin/env python

import cgi

form = cgi.FieldStorage()


fname = form.getvalue('fname')
lname = form.getvalue('lname')

body = """
<html>
<body>
<h1>Form Data</h1><br>
<h1>Your first name is %s</h1>
<h1>Your last name is %s</h1>
</body>
</html>
""" % (fname, lname)

print(body)