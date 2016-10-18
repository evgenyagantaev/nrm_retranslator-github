TARGET=biometria_thermo
LIBS=-lbcm2835 -lncurses -lpthread -lm

all: $(TARGET)

$(TARGET): main.c udp_send_thread.c metab_udp_thread.c init.c
	gcc main.c -o $(TARGET) $(LIBS) -g

clean:
	rm -rf *.o $(TARGET)
