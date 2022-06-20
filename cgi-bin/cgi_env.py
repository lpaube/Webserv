import os
print ("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-type: text/html\r\n\r\n")
print("<html>")
print("<head>")
print("<title>Hello - Second CGI Program</title>")
print("</head>")
print("<body>")
print ("Environment")
for param in os.environ.keys():
	print ("<b>%20s</b>: %s\n" % (param, os.environ[param]))
print("</body>")
print("</html>")


