(echo -en "GET /files/text1.txt HTTP/1.1\nHost: localhost\nConnection: keep-alive\r\n\r\n"; sleep 2; echo -en "GET /files/text1.txt HTTP/1.1\nHost: localhost\nConnection: Close\r\n\r\n") | telnet 127.0.0.1 8888
