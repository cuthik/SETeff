
CC=g++ -Wall -ggdb -Isrc -std=c++11

all: scripts/runtest

obj/SETeff.o: src/SETeff.h src/SETeff.cpp
	@mkdir -p obj
	${CC} -c -o obj/SETeff.o src/SETeff.cpp

obj/runtest.o: scripts/runtest.cpp
	@mkdir -p obj
	${CC} -c -o obj/runtest.o scripts/runtest.cpp

scripts/runtest: obj/SETeff.o obj/runtest.o
	${CC} -o scripts/runtest obj/runtest.o obj/SETeff.o

test: scripts/runtest
	./scripts/runtest


clean:
	rm -rf obj
	rm scripts/runtest
