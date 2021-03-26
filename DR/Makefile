#
# FILE: Makefile
# PROJECT: A3
# PROGRAMMER: MARIA MALININA
# FIRST VERSION: 2021-03-20
# DESCRIPTION: This file is a Makefile to compile all the source codes. It also has ability to 
# clean bin and obj directory
# this makefile will compile and and all source
# found in the "DR" directory.  This represents a sample
# development directory structure and project
# 
# =======================================================
#                  DR Example
# =======================================================
# FINAL BINARY Target
./bin/DR : ./obj/dr.o ./obj/drSubFunctions.o 
	cc ./obj/dr.o ./obj/drSubFunctions.o -o ./bin/DR
#
# =======================================================
#                     Dependencies
# =======================================================                     
./obj/dr.o : ./src/dr.c ./inc/dr.h
	cc -c ./src/dr.c -o ./obj/dr.o

./obj/drSubFunctions.o : ./src/drSubFunctions.c ./inc/dr.h
	cc -c ./src/drSubFunctions.c -o ./obj/drSubFunctions.o

#
# =======================================================
# Other targets
# =======================================================                     
all : ./bin/DR

clean:
	rm -rf ./bin/*
	rm -f ./obj/*.o
