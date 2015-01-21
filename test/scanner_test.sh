#!/bin/bash

gcc -Wall -g ../src/scanner.c ../src/utils.c ./scanner_test.c -o scanner

./scanner && valgrind --quiet --leak-check=yes ./scanner

rm scanner
