#/*
#* FILE          : makefile
#* PROJECT       : SENG2030 - Assignment #1
#* PROGRAMMER    : Maria Malinina
#* FIRST VERSION : 2020-02-01
#* DESCRIPTION   :
#* This file contains the operations for compiling all source
#* found in the "cryptoMagic" directory.  This represents a sample
#* development directory structure and project.
#*/
# FINAL BINARY Target
./bin/DRr : ./obj/DR.o
	cc ./obj/DR.o -o ./bin/DR

./bin/DR : ./obj/DR.o 
	cc ./obj/DR.o -o ./bin/DR
#
# =======================================================
#                     Dependencies
# =======================================================          

./obj/DR.o : ./src/DR.c ./inc/data_reader.h
	cc -c ./src/DR.c -o ./obj/DR.o

#
# =======================================================
# Other targets
# =======================================================                     
all : ./bin/DR

clean:
	rm -rf ./bin/*
	rm -f ./obj/*.o
	

