#!/usr/bin/python

import cgi

form = cgi.FieldStorage()

fname = form.getfirst('fname')

import os
from http import cookies
cookie = cookies.SimpleCookie()
cookie_string = os.environ.get('HTTP_COOKIE')
if cookie_string:
    cookie.load(cookie_string)

method = os.environ.get('REQUEST_METHOD', None)
if method == "POST" and fname is not None and  not ' ' in fname:
    if 'user' not in cookie:
        cookie['user'] = fname
        print ("Set-Cookie: user=%s\r" % (fname))
    else:
        cookie['user'] = fname
    print("Location: /cgi-bin/cookie_and_session.py")

if 'user' in cookie:
    fname = cookie['user'].value
    body = "<html>"
    body += "<head>"
    body += "<title>Hello</title>"
    body += "</head>"   
    body += "<body>"
    body += "<h2>Hello %s, you are successfully connected!</h2>" % (fname)
    body += "</body>"
    body += "</html>"
else:
    body="""
    <html>
    <head>
        <title>Session Register</title>
    </head>
    <body>
    <h1>Login</h1>
    <form action="/cgi-bin/cookie_and_session.py" method="post">
        <label for="fname">Username</label><br>
        <input type="text" id="fname" name="fname"><br><br>
        <input type="submit" value="Login">
        </form>
    </body>
    </html>
        """
print("\r\n")
print(body)