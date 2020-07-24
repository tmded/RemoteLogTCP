import logging
import socket
import _thread
HOST = "0.0.0.0" 
#HOST = socket.gethostname() +".local"   --- this also works but idk if its bad maybe uncomment it if stuff doesn't work
PORT = 11909
#rlog_port = 11909

#  https://github.com/euske/python-netstring
#
class NetstringParser(object):

    """
    Decodes a netstring to a list of Python strings.
    >>> parser = NetstringParser()
    >>> parser.feed('3:456,')
    >>> parser.results
    ['456']
    >>> NetstringParser.parse('3:abc,4:defg,')
    ['abc', 'defg']
    """
    
    def __init__(self):
        self.results = []
        self.reset()
        return

    def reset(self):
        self._data = ''
        self._length = 0
        self._parse = self._parse_len
        return
        
    def feed(self, s):
        i = 0
        while i < len(s):
            i = self._parse(s, i)
        return
        
    def _parse_len(self, s, i):
        while i < len(s):
            c = s[i]
            if c < '0' or '9' < c:
                self._parse = self._parse_sep
                break
            self._length *= 10
            self._length += ord(c)-48
            i += 1
        return i
        
    def _parse_sep(self, s, i):
        if s[i] != ':': raise SyntaxError(i)
        self._parse = self._parse_data
        return i+1
        
    def _parse_data(self, s, i):
        n = min(self._length, len(s)-i)
        self._data += s[i:i+n]
        self._length -= n
        if self._length == 0:
            self._parse = self._parse_end
        return i+n
        
    def _parse_end(self, s, i):
        if s[i] != ',': raise SyntaxError(i)
        self.add_data(self._data)
        self.reset()
        return i+1

    def add_data(self, data):
        self.results.append(data)
        return

    @classmethod
    def parse(klass, s):
        self = klass()
        self.feed(s)
        return self.results

def on_new_client(connection, client_address):
    name ="placeholderbruh"
    i=0
    try:
        #print('connection from', client_address)
        # Receive the data in small chunks and retransmit it
        while True:
            data = connection.recv(131072)
            #print(data)
            if len(data) == 0:
                break
            decodeddata = data.decode('utf-8').strip()
            stringarray = NetstringParser.parse(decodeddata)
            #print(stringarray)
            if name == "placeholderbruh":
                name = stringarray[0]  # we can do this because tcp is sequential
                stringarray.pop(0)
                print(name+ " connected")
            for x in stringarray:
                print('%s %i> %s' % (name , i, x))
                i+=1
            
    finally:
        # Clean up the connection
        print(name+" closed")
        connection.close()



# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Bind the socket to the port
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_address = (HOST, PORT)
print('starting up on %s port %s' % server_address)
sock.bind(server_address)
# Listen for incoming connections
sock.listen(1)

while True:
    # Wait for a connection
    #print('waiting for a connection')
    connection, client_address = sock.accept()
    _thread.start_new_thread(on_new_client,(connection,client_address))
