###########################################
#Makefile for simple programs
###########################################
INC= 
LIB= -lstdc++ 
CC= mpic++
CC_FLAG=-Wall -std=c++0x

OBJ=
PRG1= ./src/BSP
PRG2= ./src/HSP
PRG3= ./src/AHP

$(PRG2) : ./src/*.cpp ./src/*.h
		$(CC) $(CC_FLAG) $(INC) $(LIB) -o $@ $@.cpp
$(PRG3) : ./src/*.cpp ./src/*.h
		$(CC) $(CC_FLAG) $(INC) $(LIB) -o $@ $@.cpp
			
#.SUFFIXES: .c .o .cpp
#	.cpp.o:
#		$(CC) $(CC_FLAG) $(INC) -c $*.cpp -o $*.o

.PHONY : clean
clean:
	@echo "Removing linked and compiled files......"
		rm -f $(OBJ) $(PRG1) $(PRG2) $(PRG3) 
