
SRC = main.c
OBJ = ${SRC:.c=.o}

TARGET = timetable
LIBS = -lncurses

# no optimization, add symbols for gdb
CFLAGS += -O0 -ggdb

all: ${TARGET}

timetable: ${OBJ}
	${CC} -o $@  ${OBJ} ${LIBS}

clean:
	rm ${TARGET} ${OBJ}

new: clean all
