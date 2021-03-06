
.POSIX:
.SUFFIXES:
.PHONY: all clean test

BIN = bel scheme std unit
LISP_SRC = src/config.decl\
src/includes.decl\
src/defines.decl\
src/base.decl\
src/type.decl\
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
src/float.decl\
src/char.decl\
src/string.decl\
src/stream.decl\
src/builtin.decl\
src/number.decl\
src/core.decl\
src/backquote.decl\
src/print.decl\
src/read.decl\
src/closure.decl\
src/env.decl\
src/eval.decl\
src/lang.decl\
src/HashMap.impl\
src/HashSet.impl\
src/test.impl\
src/error.impl\
src/expr.impl\
src/base.impl\
src/type.impl\
src/fixnum.impl\
src/float.impl\
src/char.impl\
src/symbol.impl\
src/keyword.impl\
src/cons.impl\
src/string.impl\
src/stream.impl\
src/gensym.impl\
src/pointer.impl\
src/builtin.impl\
src/number.impl\
src/core.impl\
src/backquote.impl\
src/print.impl\
src/read.impl\
src/closure.impl\
src/env.impl\
src/eval.impl\
src/lang.impl

CXXFLAGS = -std=c++11 -Wall -Wextra
#CXXFLAGS += -O2
CXXFLAGS += -g -O0

all: $(BIN)

clean:
	rm -f $(BIN)

%: %.cpp lisp.hpp Makefile
	$(CXX) $(CXXFLAGS) -o $@ $<

lisp.hpp: make_lib.py $(LISP_SRC) Makefile
	./make_lib.py --name lisp -o lisp.hpp --hack $(LISP_SRC)

test:
	./std unit
	./std load bel.lisp test.bel
	./std load std.lisp test.std.lisp
	./unit
