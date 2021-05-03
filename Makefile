
.POSIX:
.SUFFIXES:
.PHONY: all clean test

BIN = bel scheme std unit

CXXFLAGS = -std=c++11 -Wall -Wextra -Wno-unused-parameter -Wno-class-memaccess
#CXXFLAGS += -O2
CXXFLAGS += -g -O0

all: $(BIN)

clean:
	rm -f $(BIN)

%: %.cpp lisp.hpp Makefile
	$(CXX) $(CXXFLAGS) -o $@ $<

lisp.hpp: make_lib.py src/lisp.fwd src/lisp.def
	./make_lib.py --hack src/lisp.fwd src/lisp.def

test:
	./std unit
	./std load bel.lisp test.bel
	./std load std.lisp test.std.lisp
	./unit
