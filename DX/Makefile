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
#                  DX Example
# =======================================================
# FINAL BINARY Target
./bin/DX : ./obj/dx.o ./obj/dxSubFunctions.o 
	cc ./obj/dx.o ./obj/dxSubFunctions.o -o ./bin/DX
#
# =======================================================
#                     Dependencies
# =======================================================                     
./obj/dx.o : ./src/dx.c ./inc/dx.h
	cc -c ./src/dx.c -o ./obj/dx.o

./obj/dxSubFunctions.o : ./src/dxSubFunctions.c ./inc/dx.h
	cc -c ./src/dxSubFunctions.c -o ./obj/dxSubFunctions.o

#
# =======================================================
# Other targets
# =======================================================                     
all : ./bin/DX

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o
