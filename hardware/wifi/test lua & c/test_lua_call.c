/*
 * getimage.c
 *
 *  Created on: Feb 15, 2023
 *      Author: Patrick
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <system.h>
#include <altera_up_avalon_rs232.h>
#include "altera_avalon_pio_regs.h"
#include "altera_avalon_jtag_uart_fd.h"
#include "altera_up_avalon_rs232_regs.h"
#include "altera_up_avalon_rs232.h"

#define WRITE_FIFO_EMPTY 0x80
#define READ_FIFO_EMPTY 0x0

int send_code(alt_up_rs232_dev* rs232_dev, char * command);
//void get_pixel(alt_up_rs232_dev* rs232_dev, int index);

// int hex7seg(unsigned d) {
//     const unsigned digits[] = { 0x40,  0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10 };
//     return (d < 10) ? digits[d] : 0x3f;
// }

int main(){
	alt_u32 write_FIFO_space;
	alt_u16 read_FIFO_used;
	alt_u8 data_W8;
	alt_u8 data_R8;
	int enter = 0;
	unsigned p_error;


	printf("Enter main\n");

	alt_up_rs232_dev* rs232_dev;
	// open the RS232 UART port
	rs232_dev = alt_up_rs232_open_dev("/dev/rs232_0");


	if (rs232_dev == NULL)
		alt_printf("Error: could not open RS232 UART\n");
	else
		alt_printf("Opened RS232 UART device\n");

	alt_up_rs232_disable_read_interrupt(rs232_dev);


//	char myString[] = "gpio.write(3,gpio.LOW)";
	char myString[] = "dofile(\"esp_wifi.lua\")";
	send_code(rs232_dev, myString);
	char myString2[] = "get_webpage()";
	send_code(rs232_dev, myString2);
//

// just wait
	int count = 0;
	while(count <= 10000){
		count = count + 1;
	}
    printf("%d\n",count);

	alt_up_rs232_enable_read_interrupt(rs232_dev);
//
//
	read_FIFO_used = alt_up_rs232_get_used_space_in_read_FIFO(rs232_dev);
	alt_printf("char stored in read_FIFO: %x\n",read_FIFO_used);

	printf("READING RESULT:\n");
	int trig = 0;
	for(int i=0; i<read_FIFO_used;i++){
		int read_status = alt_up_rs232_read_data(rs232_dev, &data_R8, &p_error);
		alt_printf("%c", data_R8);

	}



	printf("\nexit");
	return 1;
}

int send_code(alt_up_rs232_dev* rs232_dev, char * command){
	printf("SENDING CODE:\n");
	alt_u8 data_W8;
	alt_u32 write_FIFO_space;

	for(int i=0; i<command[i] != '\0'; i++){
		printf("%c", command[i]);
		data_W8 = command[i];

		write_FIFO_space = alt_up_rs232_get_available_space_in_write_FIFO(rs232_dev);
		alt_printf("used in write_FIFO: %x\n",write_FIFO_space);

		if (write_FIFO_space >= WRITE_FIFO_EMPTY) {
			int write_status = alt_up_rs232_write_data(rs232_dev, data_W8);
			alt_printf("%c", data_W8);
			printf("Write Status %d\n", write_status);
		}
	}

	data_W8 = '\n';
	alt_printf("%c", data_W8);
	int write_status = alt_up_rs232_write_data(rs232_dev, data_W8);
	printf("Write Status %d\n", write_status);
}
