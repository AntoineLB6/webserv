#!/usr/bin/python

import cgi
 
form = cgi.FieldStorage()
 
fname = form.getvalue('fname')
lname = form.getvalue('lname')

body = """
<html>
<body>
<h1>Form Data</h1>
<p>Prenom: %s</p>
<p>Nom: %s</p>
</body>
</html>
""" % (fname, lname)

print ("Content-type: text/html\r")
print ("Conten-Length: %d\r" % len(body))
print ("\r")
print(body)