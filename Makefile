CFLAGS = -g -Wall
CC = gcc
JCC = javac

all: server c_client UDPclient.class UDPmain.class

objects1 = UDPserverMain.o UDPserver.o

objects2 = UDPmain.o UDPclient.o

objects3 = UDPclient.java

objects4 = UDPmain.java

server: $(objects1)
	$(CC) -o server $(objects1)
	
c_client: $(objects2)
	$(CC) -o c_client $(objects2)

UDPclient.class: $(objects3)
	$(JCC) $(objects3)

UDPmain.class: $(objects4)
	$(JCC) $(objects4)

UDPserver.o: UDPserver.c
UDPserverMain.o: UDPserverMain.c

UDPclient.o: UDPclient.c
UDPmain.o: UDPmain.c


.PHONY : clean
clean: 
	rm server c_client *.class $(objects1) $(objects2)
