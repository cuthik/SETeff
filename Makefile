

ROOTFLAGS=$(shell root-config --cflags)
ROOTLIBS=$(shell root-config --glibs)


CC=g++ -Wall -ggdb -Isrc -std=c++11 ${ROOTFLAGS}

all: scripts/runtest

obj/ZBLibraryClass.o: src/ZBLibraryClass.h src/ZBLibraryClass.cpp
	@mkdir -p obj
	${CC} -c -o obj/ZBLibraryClass.o src/ZBLibraryClass.cpp

obj/SETeff.o: src/SETeff.h src/SETeff.cpp
	@mkdir -p obj
	${CC} -c -o obj/SETeff.o src/SETeff.cpp

obj/runtest.o: scripts/runtest.cpp
	@mkdir -p obj
	${CC} -c -o obj/runtest.o scripts/runtest.cpp

scripts/runtest: obj/SETeff.o obj/ZBLibraryClass.o obj/runtest.o
	${CC} -o scripts/runtest obj/runtest.o obj/SETeff.o obj/ZBLibraryClass.o ${ROOTLIBS}

test: scripts/runtest
	gdb -ex run ./scripts/runtest


clean:
	rm -rf obj
	rm scripts/runtest
