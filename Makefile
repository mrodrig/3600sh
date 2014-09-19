SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)
TARGET = 3600sh

all: $(TARGET)

%.o : %.c
	gcc -std=c99 -O0 -g -lm -Wall -pedantic -Werror -Wextra -c $< -o $@

$(TARGET): $(OBJS)
	gcc -std=c99 -O0 -g -lm -Wall -pedantic -Werror -Wextra -o $@ $(OBJS)

clean:
	rm -rf $(TARGET) *.o *~

test: all
	./test

run: all
	./$(TARGET)
