PROGRAM	=soukoban
OBJS	=soukoban.o
SRCS	=$(OBJS:%.o=%.c)
CC		=gcc
LD		=gcc
CFLAGS	=-g -Wall
LDFLAGS	=-lcurses
VPATH	=../bin/:../bin/obj
BINPATH	=../bin/
OBJPATH	=../bin/obj/

all:$(PROGRAM)

clean:
	rm -rf $(BINPATH)$(PROGRAM) $(OBJPATH)*.o

$(PROGRAM):$(OBJS)
	$(LD) $(OBJPATH)$(OBJS) -o $(BINPATH)$@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $(OBJPATH)$@

demo:
	$(BINPATH)$(PROGRAM)

