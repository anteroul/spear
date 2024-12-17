#!/bin/bash
find . -type f \( -name "*.cc" -o -name "*.hh" \) -exec clang-format -i {} +
