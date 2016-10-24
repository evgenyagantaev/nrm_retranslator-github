#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define DATA_BUFFER_LENGTH 16

void uart_open_port();

void uart_push_data(uint8_t data);
uint8_t uart_pop_data();
uint8_t uart_read_byte();
void uart_data_receiving_thread(void *parameter);

int set_interface_attribs (int fd, int speed, int parity);
void set_blocking (int fd, int should_block);












