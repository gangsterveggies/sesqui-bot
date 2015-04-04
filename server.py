#!/usr/bin/python
from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer

PORT_NUMBER = 8000

class myHandler(BaseHTTPRequestHandler):
  def do_GET(self):
    self.send_response(200)
    self.send_header('Content-type','application/json')
    self.end_headers()
    self.wfile.write("{\n  \"moves\": [\n    \"p|1|1\",\n    \"p|1|2\"\n  ]\n}")
#    self.wfile.write("{\n  'moves': [\n    'p|1|1',\n    'm|1|1|3|3'\n  ]\n}")

    if ("board" in self.path):
      print self.path[self.path.index("=") + 1:]
    return

try:
  server = HTTPServer(('', PORT_NUMBER), myHandler)
  print 'Started httpserver on port ' , PORT_NUMBER
  server.serve_forever()

except KeyboardInterrupt:
  print '^C received, shutting down the web server'
  server.socket.close()
