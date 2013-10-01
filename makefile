#
#	Author	: Arpit Bhayani
#	E-mail 	: arpit.bhayani@students.iiit.ac.in
#	Roll no.: 201305515
#

all: 	 a.out

a.out:   main.o ls.o util.o print.o
	 gcc -Wall main.o ls.o util.o print.o

main.o:	 main.c lsheader.h
	 gcc -Wall -c main.c

ls.o: 	 ls.c lsheader.h
	 gcc -Wall -c ls.c

util.o:	 util.c lsheader.h
	 gcc -Wall -c util.c

print.o: print.c lsheader.h
	 gcc -Wall -c print.c

clean:
	 rm -rf *.o *.h.gch a.out

