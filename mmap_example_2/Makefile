CC = gcc -m64
CFLAGS += -Wall -O2 -pthread -g
OBJS = main.o
TARGET = main

all = $(TARGET)

$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm *.o
	rm $(TARGET)
	find -type f ! -name "*.c" ! -name "Makefile" ! -name "hello_world" -exec rm {} \;