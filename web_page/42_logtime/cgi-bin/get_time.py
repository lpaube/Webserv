#! /usr/bin/python
import cgi
import sys
form = cgi.FieldStorage()
import subprocess


print ("HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-type: text/html\r\n\r\n")
user_name = form.getvalue('user')
args = ("web_page/42_logtime/cgi-bin/Logtime-42/logtime42", "-c", "web_page/42_logtime/cgi-bin/Logtime-42/config.json", "-l", user_name)
popen = subprocess.Popen(args, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
popen.wait()
output = popen.stdout.read()
error = popen.stderr.read()

if popen.returncode != 0:
    print(error.decode("utf-8"))
else:
    print(output.decode("utf-8"))
