#!/usr/bin/python

# Import modules for CGI handling
import cgi
# Create instance of FieldStorage
form = cgi.FieldStorage()

# Get data from fields
fname = form.getvalue('fname')

# get cookies
import os
import Cookie
cookie = Cookie.SimpleCookie()
cookie_string = os.environ.get('HTTP_COOKIE')
cookie.load(cookie_string)


# Check if the cookie exists
body = "aa"

method = os.environ.get('REQUEST_METHOD', None)
if method == "POST" and  not ' ' in fname:
    cookie['user'] = fname
    print ("Set-Cookie: user=%s\r" % (fname))
if cookie.has_key('user'):
    fname = cookie['user'].value



if not cookie.has_key('user'):
    body="""
    <!doctype html>
<head>
    <title>Session Register</title>
</head>
<form action="/cgi-bin/scriptPOST.php" method="post">
      <label for="fname">Prenom:</label><br>
      <input type="text" id="fname" name="fname"><br>
      <label for="lname">Nom:</label><br>
      <input type="text" id="lname" name="lname"><br><br>
      <input type="submit" value="Envoyer">
    </form>
</html>
    """

else:
    body = "<html>"
    body += "<head>"
    body += "<title>Hello - Second CGI Program</title>"
    body += "</head>"   
    body += "<body>"
    body += "<h2>Hello %s</h2>" % (fname)
    body += "</body>"
    body += "</html>"

# print "Content-type:text/html\r"
# print "Content-Length: %d\r" % len(body)
# print "\r"
print(body)