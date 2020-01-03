#!/bin/bash
find ./client/src -iname *.h* -o -iname *.c* | xargs clang-format -style=WebKit -i -verbose
find ./plugin -iname *.h* -o -iname *.c* | xargs clang-format -style=WebKit -i -verbose
