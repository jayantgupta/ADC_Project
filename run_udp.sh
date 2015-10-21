gcc -c keyval.c -o keyval.o
gcc -c logfile.c -o logfile.o
gcc -c udpserver.c -o udpserver.o
gcc -c test.c -o mytest.o
gcc mytest.o keyval.o -o mytest.exec
gcc udpserver.o keyval.o logfile.o -o udpserver.exec
gcc udpclient.c logfile.o -o udpclient.exec
