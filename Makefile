EXEC_NAME=SESQUIBOT
CC=g++

# CFLAGS= -Wall -Wno-write-strings -O0 -g
#CFLAGS= -w -Wno-write-strings -O3 -g
CFLAGS= -w -Wno-write-strings -O3 -g -std=c++0x

SRC =                   	\
	main.cpp 		\
	board.cpp		\
	timer.cpp		\
	search.cpp		\
	ab.cpp			\
	mcts.cpp

OBJ =  ${SRC:.cpp=.o}

#------------------------------------------------------------

all: ${EXEC_NAME}

${EXEC_NAME}: ${OBJ}
	${CC} ${CFLAGS} ${CLIBS} -o ${EXEC_NAME} ${OBJ}

%.o: %.cpp
	${CC} ${CFLAGS} -c -o $@ $+

clean:
	rm ${EXEC_NAME} *.o *~ *# -rf
