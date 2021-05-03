
.POSIX:
.SUFFIXES:
.PHONY: all clean test

BIN = bel scheme std unit
LISP_SRC = src/config.decl\
src/includes.decl\
src/defines.decl\
src/base.decl\
src/test.decl\
src/error.decl\
src/expr.decl\
src/nil.decl\
src/symbol.decl\
src/keyword.decl\
src/cons.decl\
src/gensym.decl\
src/pointer.decl\
src/fixnum.decl\
src/char.decl\
src/string.decl\
src/stream.decl\
src/builtin.decl\
src/system.decl\
src/lisp.def

CXXFLAGS = -std=c++11 -Wall -Wextra -Wno-unused-parameter -Wno-class-memaccess
#CXXFLAGS += -O2
CXXFLAGS += -g -O0

all: $(BIN)

clean:
	rm -f $(BIN)

%: %.cpp lisp.hpp Makefile
	$(CXX) $(CXXFLAGS) -o $@ $<

lisp.hpp: make_lib.py $(LISP_SRC) Makefile
	./make_lib.py --hack $(LISP_SRC)

test:
	./std unit
	./std load bel.lisp test.bel
	./std load std.lisp test.std.lisp
	./unit
