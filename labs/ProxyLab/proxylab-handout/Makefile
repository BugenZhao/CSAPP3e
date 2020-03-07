# Makefile for Proxy Lab 
#
# You may modify this file any way you like (except for the handin
# rule). You instructor will type "make" on your specific Makefile to
# build your proxy from sources.

CC = gcc
CFLAGS = -g -Wall -I.
LDFLAGS = -lpthread

macos:
	@echo "Go to Linux and run 'make linux' or 'make benchmark'"

linux: proxy

rio_p.o: rio_p.c
	$(CC) $(CFLAGS) -c rio_p.c

bzcsapp.o: bzcsapp.c bzcsapp.h
	$(CC) $(CFLAGS) -c bzcsapp.c

sbuf.o: sbuf.c sbuf.h
	$(CC) $(CFLAGS) -c sbuf.c

csapp.o: csapp.c csapp.h
	$(CC) $(CFLAGS) -c csapp.c

proxy.o: proxy.c proxy.h csapp.h
	$(CC) $(CFLAGS) -c proxy.c

proxy: proxy.o csapp.o sbuf.o bzcsapp.o rio_p.o
	$(CC) $(CFLAGS) proxy.o csapp.o sbuf.o bzcsapp.o rio_p.o -o proxy $(LDFLAGS)

tiny:
	make -C tiny

benchmark: clean proxy tiny
	./driver.sh

# Creates a tarball in ../proxylab-handin.tar that you can then
# hand in. DO NOT MODIFY THIS!
handin:
	(make clean; cd ..; tar cvf $(USER)-proxylab-handin.tar proxylab-handout --exclude tiny --exclude nop-server.py --exclude proxy --exclude driver.sh --exclude port-for-user.pl --exclude free-port.sh --exclude ".*")

clean:
	rm -f *~ *.o proxy core *.tar *.zip *.gzip *.bzip *.gz
	rm -rf .noproxy .proxy
	make -C tiny clean

curl:
	-curl -v --proxy http://localhost:10188 http://localhost:1018/index.html