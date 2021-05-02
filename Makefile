
.POSIX:
.SUFFIXES:
.PHONY: all clean test

BIN = bel scheme std

all: $(BIN)

clean:
	rm -f $(BIN)

%: %.cpp lisp.hpp Makefile
	c++ -std=c++11 -Wall -Wextra -Wno-unused-parameter -Wno-class-memaccess -O2 -o $@ $<

test:
	./std unit
	./std load bel.lisp test.bel
	./std load test.std
