PROGRAM = exchanger
CC = g++
CFLAGS = -L/usr/local/lib -I/usr/local/include -Wall -g
LINKS = -lcrypto -lssl -levent -levent_openssl
SRCS = main.cpp ini_parse.cpp log_logger.cpp fcgi_client.cpp mem_rediscli.cpp exch_server.cpp exch_client.cpp exch_exchanger.cpp

all:
	$(CC) $(CFLAGS) $(LINKS) -o $(PROGRAM) $(SRCS)

clean:
	rm $(PROGRAM)

install: 
	cp ./$(PROGRAM) /usr/local/bin/
	mkdir -p /etc/$(PROGRAM)
	cp ./config/exch.conf /etc/$(PROGRAM)/exch.conf
	cp ./scripts/$(PROGRAM) /etc/init.d/
	chmod a+x /etc/init.d/$(PROGRAM)

uninstall: 
	rm /usr/local/bin/$(PROGRAM)
	rm /etc/init.d/$(PROGRAM)
	rm /etc/$(PROGRAM)/exch.conf
	rmdir /etc/$(PROGRAM)

