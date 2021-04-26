
.POSIX:
.SUFFIXES:
.PHONY: all clean

all: lisp

clean:
	rm -f lisp

lisp: lisp.cpp
	c++ -Wall -Wextra -Wno-unused-parameter -O2 -o $@ $<
