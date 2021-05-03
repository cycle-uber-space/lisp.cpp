#!/usr/bin/env python3

def load_text(path):
    with open(path, "rt") as f:
        return f.read()

def save_text(path, text):
    with open(path, "wt") as f:
        f.write(text)

def load_source(path):
    return f"\n#line 2 \"{path}\"" + load_text(path)

def main():
    text = ""
    text += """
#ifndef _LISP_HPP_
#define _LISP_HPP_
"""
    text += load_source("src/lisp.hpp")
    text += """
#endif /* _LISP_HPP_ */

#ifdef LISP_IMPLEMENTATION

#ifndef _LISP_CPP_
#define _LISP_CPP_
"""
    text += load_source("src/lisp.cpp")
    text += """
#endif /* _LISP_CPP_ */

#endif /* LISP_IMPLEMENTATION */
"""

    save_text("lisp.hpp", text)

if __name__ == "__main__":
    main()
