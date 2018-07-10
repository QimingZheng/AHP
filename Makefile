###########################################
#Makefile for simple programs
###########################################
INC= 
LIB= -lstdc++ 
CC= mpic++
CC_FLAG=-Wall -std=c++0x

OBJ= ./bin/BSP
PRG= ./src/BSP

$(PRG) : ./src/*.cpp ./src/*.h
		$(CC) $(CC_FLAG) $(INC) $(LIB) -o $(OBJ) $(PRG).cpp
			
.SUFFIXES: .c .o .cpp
	.cpp.o:
		$(CC) $(CC_FLAG) $(INC) -c $*.cpp -o $*.o

.PHONY : clean
clean:
	@echo "Removing linked and compiled files......"
		rm -f $(OBJ) $(PRG1) $(PRG2) $(PRG3) 
