#! /usr/bin/python
import cgi, cgitb 
form = cgi.FieldStorage()


print ("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-type: text/html\r\n\r\n")
user_name = form.getvalue('user')


print "<html>"
print "<head>"
print "<title>Hello - Second CGI Program</title>"
print "</head>"
print "<body>"
print "<h2>Hello %s ! How are you</h2>" % (user_name)
print "</body>"
print "</html>"



