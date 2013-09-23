###############################
# Makefile for Cursed Space
###############################

CC = gcc
LIBS = -lncurses
TARGET = cursedSpace


all: $(TARGET)
	@echo 
	@echo "**************************************"
	@echo "All files were successfully built"
	@echo "Lets go shoot some aliens :)"
	@echo "**************************************"
	@echo 

OBJS = $(patsubst %.c, %.o,$(wildcard *.c))

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

clean:
	rm $(TARGET) $(OBJS)
