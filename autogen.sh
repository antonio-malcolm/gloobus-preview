#! /bin/sh

autoreconf -v -i
echo Running configure with arguments "$@"
./configure $@
