#!/usr/bin/python

from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
import os
import sys

PORT_NUMBER = 8000

class myHandler(BaseHTTPRequestHandler):
  def do_GET(self):
    self.send_response(200)
    self.send_header('Content-type','application/json')
    self.end_headers()
#    self.wfile.write("{\n  \"moves\": [\n    \"p|1|1\",\n    \"p|1|2\"\n  ]\n}")

    if ("board" in self.path):
      moves = "{\n  \"moves\": [\n    \"%s\"\n  ]\n}"
      moves_2 = "{\n  \"moves\": [\n    \"%s\",\n    \"%s\"\n  ]\n}"

      cmd = './SESQUIBOT -i ' + '"' + self.escape_dump(self.path[self.path.index("=") + 1:]) + '" ' + (sys.argv[1] if len(sys.argv) > 1 else '--mcts')

      out = os.popen(cmd).read()

      move_list = out.split('$')

      print out
      if len(move_list) == 1:
        self.wfile.write(moves % move_list[0].strip())
      elif len(move_list) == 2:
        self.wfile.write(moves_2 % (move_list[0], move_list[1].strip()))

    return

  def escape_dump(self, dump):
    return dump.replace('$', '\\$')

try:
  server = HTTPServer(('', PORT_NUMBER), myHandler)
  print 'Started httpserver on port ' , PORT_NUMBER
  server.serve_forever()

except KeyboardInterrupt:
  print '^C received, shutting down the web server'
  server.socket.close()
