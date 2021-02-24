import http.server
from http.server import HTTPServer, BaseHTTPRequestHandler
import socketserver

PORT = 8000

Handler = http.server.SimpleHTTPRequestHandler

Handler.extensions_map={
        '.manifest': 'text/cache-manifest',
    '.html': 'text/html',
        '.png': 'image/png',
    '.jpg': 'image/jpg',
    '.svg': 'image/svg+xml',
    '.css': 'text/css',
    '.js':  'application/x-javascript',
    '': 'application/octet-stream', # Default
    }

httpd = socketserver.TCPServer(("localhost", PORT), Handler)

print("serving at port", PORT)
httpd.serve_forever()