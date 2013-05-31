import SimpleHTTPServer
import BaseHTTPServer
import SocketServer
import socket
import thread
import datetime


class ThreadedHttpServer(SocketServer.ThreadingMixIn, BaseHTTPServer.HTTPServer):
    
    def __init__(self, *args, **kwargs):
        self.connections = []
        BaseHTTPServer.HTTPServer.__init__(self, *args, **kwargs)
    
    def get_request(self):
        while True:
            try:
                sock, addr = self.socket.accept()
                sock.settimeout(None)
                self.connections.append((sock, addr, datetime.datetime.now()))
                return (sock, addr)
            except socket.timeout:
                pass    

class RequestHandler(SimpleHTTPServer.SimpleHTTPRequestHandler):
    def do_POST(self):
        self.do_POST()        
    
    def do_GET(s):
        """Respond to a GET request."""
        s.send_response(200)
        s.send_header('Transfer-Encoding', 'chunked')
        s.send_header("Content-type", "text/html")
#         s.send_header("Connection", "close")        
#         s.send_header("Content-length", "3")
        s.end_headers()        
        chunk = ("<html><head><title>Title goes here.</title></head>"
                "<body><p>This is a test.</p>"
                "<p>You accessed path: %s</p>"
                "</body></html>" % (s.path, ))
        s.wfile.write("%X\r\n%s\r\n" % (len(chunk), chunk, ))
        s.wfile.write("0\r\n\r\n")
 
#         s.connection.shutdown(socket.SHUT_RDWR)
#         s.connection.close()
        return
    
    
def supervise(server):
    
    print 'Supervising...'
    
    return


def main():
    RequestHandler.protocol_version = "HTTP/1.1"
    
    server_address = ("0.0.0.0", 80,)
    
    httpd = ThreadedHttpServer(server_address, RequestHandler)
    
    
    thread.start_new(supervise, (httpd, ))
    httpd.serve_forever()
    
    return

if __name__ == '__main__':
    main() 