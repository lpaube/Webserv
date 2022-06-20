


import cgi
import sys
import os
from unicodedata import name

form = cgi.FieldStorage()
data = sys.stdin.readlines()

type = os.getenv("CONTENT_TYPE")
index = type.find("boundary=")
index = index + 9
boundary = type[index:]
boundary = "--" + boundary + "--"

#filename
i = 0
start = data[1].find("filename=\"")
if start > 0:
	start += 10
	end = data[1].find("\"", start)
	filename = data[1][start:end]
else:
	filename = "default"
path = "cgi-bin/upload/"
filename = path + filename
#file type
start = data[2].find("Content-Type:")
if start >= 0:
	start += 14
	end = data[2].find("/")
	filetype = data[2][start:end]
else:
	filetype = "NULL"

if filetype == "text":
	flags = "wt"
else:
	flags = "wb"
f = open(filename, flags)

#body-
start = 4
while True:
	if data[start + 1].find(boundary) >= 0:
		break
	else:
		if filetype == "text":
			f.write(data[start])
		else:
			byte_array = bytes(data[start], encoding='utf-8')
			f.write(byte_array)
	start += 1

f.close()
print ("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n")
