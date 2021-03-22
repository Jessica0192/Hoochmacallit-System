#
# FILE: Makefile
# PROJECT: A3
# PROGRAMMER: JESSICA SIM
# FIRST VERSION: 2021-01-25
# DESCRIPTION: This file is a Makefile to compile all the source codes. It also has ability to 
# clean bin and obj directory
#

#
# this makefile will compile and and all source
# found in the "DC" directory.  This represents a sample
# development directory structure and project
# 
# =======================================================
#                  DC Example
# =======================================================
# FINAL BINARY Target
./bin/DC : ./obj/dc.o ./obj/dcSubFunctions.o 
	cc ./obj/dc.o ./obj/dcSubFunctions.o -o ./bin/DC
#
# =======================================================
#                     Dependencies
# =======================================================                     
./obj/dc.o : ./src/dc.c ./inc/dc.h
	cc -c ./src/dc.c -o ./obj/dc.o

./obj/dcSubFunctions.o : ./src/dcSubFunctions.c ./inc/dc.h
	cc -c ./src/dcSubFunctions.c -o ./obj/dcSubFunctions.o

#
# =======================================================
# Other targets
# =======================================================                     
all : ./bin/DC

clean:
	rm -rf ./bin/*
	rm -f ./obj/*.o
