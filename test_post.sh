(echo -en "POST /files/text1.txt HTTP/1.1\nHost: localhost\nConnection: keep-alive\n\nPOSTDATA";sleep 2) | telnet 127.0.0.1 8888
