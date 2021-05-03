
.POSIX:
.SUFFIXES:
.PHONY: all clean test

BIN = bel scheme std

CXXFLAGS = -std=c++11 -Wall -Wextra -Wno-unused-parameter -Wno-class-memaccess -O2

all: $(BIN)

clean:
	rm -f $(BIN)

%: %.cpp lisp.hpp Makefile
	c++ $(CXXFLAGS) -o $@ $<

test:
	./std unit
	./std load bel.lisp test.bel
	./std load test.std
