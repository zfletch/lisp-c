#!/bin/bash

gcc -Wall -g ../src/hash.c ./hash_test.c -o hash

./hash && valgrind --quiet ./hash

rm hash
