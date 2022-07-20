


import cgi
import sys
import os
from unicodedata import name

form = cgi.FieldStorage()
data = sys.stdin.buffer.read()

type = os.getenv("CONTENT_TYPE")
index = type.find("boundary=")
index = index + 9
boundary = type[index:]
boundary = bytes("--" + boundary + "--", encoding="utf-8")

#filename
i = 0
start = data.find(b"filename=\"")
if start > 0:
	start += 10
	end = data.find(b"\"", start)
	filename = data[start:end]
else:
	filename = "default"
path = b"web_page/42_upload/upload/"
filename = path + filename
#file type
start = data.find(b"Content-Type:")
if start >= 0:
	start += 14
	end = data.find(b"/", start)
	filetype = data[start:end]
else:
	filetype = "NULL"

if filetype == "text":
	flags = "wt"
else:
	flags = "wb"
strfilename = str(filename, "utf-8")
f = open(strfilename, flags)


#body-
start = data.find(b"\n", end)
start += 3
end = data.find(boundary)
f.write(data[start:end - 2])
f.close()
print ("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n")
