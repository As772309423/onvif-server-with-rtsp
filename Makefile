GSOAP_ROOT = ./
CCO = gcc
CC = $(CCO) -g -DWITH_NONAMESPACES  -DSOAP_DEBUG -DDEBUG
INCLUDE = -I$(GSOAP_ROOT)
CPP_OBJS = soapC.cpp stdsoap2.cpp
SERVER_OBJS = soapC.o stdsoap2.o duration.o wsaapi.o onvif_server_interface.o soapClient.o soapServer.o onvif_server.o


all: server 
server:
	$(CC) $(INCLUDE) soapC.c -o soapC.o
	$(CC) $(INCLUDE) stdsoap2.c -o stdsoap2.o
	$(CC) $(INCLUDE) duration.c -o duration.o
	$(CC) $(INCLUDE) wsaapi.c -o wsaapi.o
	$(CC) $(INCLUDE) soapClient.c -o soapClient.o
	$(CC) $(INCLUDE) soapServer.c -o soapServer.o
	$(CC) $(INCLUDE) onvif_server_interface.c -o onvif_server_interface.o
	$(CC) $(INCLUDE) onvif_server.c -o onvif_server.o
	$(CC) $(SERVER_OBJS) -o deviceserver $(INCLUDE) -lpthread -lm

arm:
	$(CCO) *.c -o deviceserver $(INCLUDE) -lpthread -lm
	
test:
	$(CCO) onvif_server.c -o deviceserver -lpthread -lm -libsv_onvif.a

clean: 
	rm -f *.o deviceprobe  deviceserver onvif
