all:
	rpcgen generala.x
	gcc -c generala_client.c -std=c99
	gcc -c generala_server.c
	gcc -c generala_clnt.c
	gcc -c generala_svc.c
	gcc -c generala_xdr.c
	gcc -lrt -lnsl -o generala_client generala_client.o generala_clnt.o generala_xdr.o
	gcc -lnsl -o generala_server generala_server.o generala_svc.o generala_xdr.o

clean:
	rm -f *.o
	rm -f generala_client
	rm -f generala_server
	rm -f generala_clnt.c
	rm -f generala_svc.c
	rm -f generala_xdr.c
	rm -f generala_.h
