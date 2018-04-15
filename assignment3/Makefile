a.out: main.o devicedriver.o ftlmgr.o
	gcc -o a.out main.o devicedriver.o ftlmgr.o
main.o: main.c blkmap.h
	gcc -c main.c blkmap.h
devicedriver.o: devicedriver.c blkmap.h
	gcc -c devicedriver.c blkmap.h
ftlmgr.o: ftlmgr.c blkmap.h
	gcc -c ftlmgr.c blkmap.h

clean:
	rm -f *.o
	rm -f a.out
	rm -f *.gch
