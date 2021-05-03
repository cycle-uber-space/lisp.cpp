#!/usr/bin/env python3
import sys, re

g_match = None

def re_match(pattern, text):
    global g_match
    g_match = re.match(pattern, text)
    return g_match

def re_group(number):
    global g_match
    return g_match.group(number)

def load_text(path):
    with open(path, "rt") as f:
        return f.read()

def save_text(path, text):
    with open(path, "wt") as f:
        f.write(text)

def split_text(text):
    return text.split("\n")

def join_lines(lines):
    return "\n".join(lines)

def load_lines(path):
    return split_text(load_text(path))

def save_lines(path, lines):
    save_text(join_lines(lines))

def load_source(path):
    return f"\n#line 2 \"{path}\"" + load_text(path)

def assemble(srcs):
    text = ""
    text += """
#ifndef _LISP_HPP_
#define _LISP_HPP_
"""
    for src in srcs:
        if src.endswith(".hpp"):
            text += load_source(src)
    text += """
#endif /* _LISP_HPP_ */

#ifdef LISP_IMPLEMENTATION

#ifndef _LISP_CPP_
#define _LISP_CPP_
"""
    for src in srcs:
        if src.endswith(".cpp"):
            text += load_source(src)
    text += """
#endif /* _LISP_CPP_ */

#endif /* LISP_IMPLEMENTATION */
"""
    return text

def hack_lines(lines):
    ID = "[_a-zA-Z][_a-zA-Z0-9]*"
    for line in lines:
        if re_match(fr"^( *)let ({ID}) = ([^;]+);$", line):
            indent = re_group(1)
            name = re_group(2)
            code = re_group(3)
            yield f"{indent}auto const {name} = {code};"
        elif re_match(fr"^( *)var ({ID}) = ([^;]+);$", line):
            indent = re_group(1)
            name = re_group(2)
            code = re_group(3)
            yield f"{indent}auto {name} = {code};"
        else:
            yield line

def hack_text(text):
    return join_lines(hack_lines(split_text(text)))

def hack_file(path):
    save_text(path, hack_text(load_text(path)))

def main(args):
    srcs = list()
    hack = False
    undo = False
    for arg in args:
        if arg == "--hack":
            hack = True
        elif arg == "--undo":
            undo = True
        else:
            srcs.append(arg)

    text = assemble(srcs)

    if hack:
        text = hack_text(text)

    save_text("lisp.hpp", text)

    if undo:
        for src in srcs:
            hack_file(src)

if __name__ == "__main__":
    main(sys.argv[1:])
