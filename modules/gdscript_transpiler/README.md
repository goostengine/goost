# GDScript Transpiler

This is a very early and experimental implementation of general-purpose
GDScript-to-X language transpiler which aims to generate sources which could be
natively understood by other compilers.

Current languages: 
* C++:
  - supports somewhat useable basic header generation from `class_name` scripts
    (given type hints are provided)
  - generates stub implementations for each method (far from perfect but alright
    to speed up manual translation).
