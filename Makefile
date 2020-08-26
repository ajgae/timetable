
SRC = main.c
OBJ = ${SRC:.c=.o}

TARGET = timetable
LIBS = -lncurses

# no optimization, add symbols for gdb
CFLAGS += -O0 -ggdb -fsanitize=address
LDFLAGS += -fsanitize=address

all: ${TARGET}

timetable: ${OBJ}
	${CC} -o $@  ${OBJ} ${LIBS} ${LDFLAGS}

clean:
	rm ${TARGET} ${OBJ}

new: clean all

run: new
	./timetable
