#/*
#* FILE          : makefile
#* PROJECT       : SENG2030 - Assignment #1
#* PROGRAMMER    : Maria Malinina
#* FIRST VERSION : 2020-02-01
#* DESCRIPTION   :
#* This file contains the operations for compiling all source
#* found in the "cryptoMagic" directory.  This represents a sample
#* development directory structure and project.
#/
# FINAL BINARY Target
./bin/data_reader : ./obj/data_reader.o
	cc ./obj/data_reader.o -o ./bin/data_reader

#
# =======================================================
#                     Dependencies
# =======================================================

./obj/data_reader.o : ./src/data_reader.c 
	cc -c ./src/data_reader.c -o ./obj/data_reader.o

#
# =======================================================
# Other targets
# =======================================================
all : ./bin/data_reader

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o
