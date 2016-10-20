.PHONY: all clean


vpath %.h ./inc
vpath %.c ./src
vpath %.o ./obj

FINAL_TARGET=nrm_retranslator
LIBS=-lbcm2835 -lncurses -lpthread -lm
INCLUDES=-I./inc
OBJECTS=main.o uart_obj.o logger_obj.o 

all:$(FINAL_TARGET)

$(FINAL_TARGET): $(OBJECTS)
	gcc $^ -o $(FINAL_TARGET) $(LIBS) -g -Wall

main.o: main.c main.h 
	gcc $< -c -o obj/main.o $(INCLUDES) $(LIBS) -g -Wall -O0
uart_obj.o: uart_obj.c uart_obj.h 
	gcc $< -c -o obj/uart_obj.o $(INCLUDES) $(LIBS) -g -Wall -O0
logger_obj.o: logger_obj.c logger_obj.h 
	gcc $< -c -o obj/logger_obj.o $(INCLUDES) $(LIBS) -g -Wall -O0

clean:$(OBJECTS) $(FINAL_TARGET)
	rm -rf $^ 
