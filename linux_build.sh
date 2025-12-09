#!/bin/sh
# PATH=~/09_System/
# make install
# cp src/d2.h $PATH/include/d2.h

install_path="~/09_System"
make install
test=$(pwd)
cp -f $test/src/d2.h ~/09_System/include/d2.h