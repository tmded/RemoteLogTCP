# RemoteLogTCP
RemoteLog but its TCP
This project is based on Muirey'ss [RemoteLog](https://github.com/Muirey03/RemoteLog). This project differs from Muirey's as it uses TCP instead of UDP this allows it to do some cool stuff with process information as all the data will arrive in order and failed packets will be retransmitted. It also comes with a few performance enhancing features like not creating and destroying a socket every log.
It uses [netstring](https://en.wikipedia.org/wiki/Netstring) to format the messages as TCP is streambased so seperating messages is not as easy as packets in UDP.

The high level ObjectiveC tcp interface means this can work with hostnames, ipv4 addresses and ipv6 addresses.

It wraps the sending of the messages in an async block. It does the string formatting outside the async block however, this is because I had some issues where objects wouldn't exist later on. The script is for python3 but can work on python 2 by changing the `_thread` import int just `thread`. 
