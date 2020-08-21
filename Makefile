
SRC = main.c
OBJ = ${SRC:.c=.o}

TARGET = timetable
LIBS = -lncurses

# no optimization, add symbols for gdb
# and check for memory leaks at the end of execution
CFLAGS += -O0 -ggdb -fsanitize=address -Wall -Wextra
LDFLAGS += -fsanitize=address

all: ${TARGET}

timetable: ${OBJ}
	${CC} -o $@  ${OBJ} ${LIBS} ${LDFLAGS}

clean:
	rm -f ${TARGET} ${OBJ}

new: clean all
