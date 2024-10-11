import http.server
import socketserver
import os

PORT = 8000

class HTTP_handler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header('Access-Control-Allow-Origin', '*')
        super().end_headers()


os.chdir('C:/Users/Alanis/Documents/CS/Icarus/testing')
handler = HTTP_handler
with socketserver.TCPServer(("", PORT), handler) as httpd:
    print(f"Serving at port {PORT}")
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass

    print("Server stopped.")
    httpd.server_close()
    print("Server closed.")

