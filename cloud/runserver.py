"""
webserver.py

Web server for production use, uses a WSGI server to
run the underlying WSGI application (Flask).
"""

HOST = '0.0.0.0' #localhost
PORT = 80        #HTTP

import sys

import wsgiserver
from server.main import app

#process command line arguments
arguments = {}
for arg in sys.argv:
    k, *v = arg.split('=', 1)
    arguments[k] = v

    #process options
    if '--port' in arguments.keys():
        PORT = int(arguments['--port'][0])

print(f"Server starting on host {HOST} and port {PORT}")

#initialize server with given WSGI application, running on configured host and port
server = wsgiserver.WSGIServer(app, host=HOST, port=PORT)
server.start()