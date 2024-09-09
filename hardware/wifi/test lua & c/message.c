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
void get_pixel(alt_up_rs232_dev* rs232_dev, int index);

int hex7seg(unsigned d) {
    const unsigned digits[] = { 0x40,  0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10 };
    return (d < 10) ? digits[d] : 0x3f;
}

int main(){
	alt_u32 write_FIFO_space;
	alt_u16 read_FIFO_used;
	alt_u8 data_W8;
	alt_u8 data_R8;
	int enter = 0;
	unsigned p_error;


	printf("Enter main\n");

	IOWR_ALTERA_AVALON_PIO_DATA(PIO_0_BASE, hex7seg(2));

	alt_up_rs232_dev* rs232_dev;
	// open the RS232 UART port
	rs232_dev = alt_up_rs232_open_dev("/dev/rs232_0");


	if (rs232_dev == NULL)
		alt_printf("Error: could not open RS232 UART\n");
	else
		alt_printf("Opened RS232 UART device\n");

	alt_up_rs232_disable_read_interrupt(rs232_dev);

//	char myString[] = "gpio.write(3,gpio.LOW)";
//	char myString[] = "dofile(\"init.lua\")";
//	send_code(rs232_dev, myString);
//	char myString2[] = "check_wifi()";
//	send_code(rs232_dev, myString2);
//
	int index = 3;
	get_pixel(rs232_dev, index);

	int count = 0;
	while(count <= 10000){
		printf("%d\n",count);
		count = count + 1;
	}

	alt_up_rs232_enable_read_interrupt(rs232_dev);
//
//
	read_FIFO_used = alt_up_rs232_get_used_space_in_read_FIFO(rs232_dev);
	alt_printf("char stored in read_FIFO: %x\n",read_FIFO_used);


//
//	char buffer[80];
//	alt_fd * fd = fopen("/dev/rs232_0",buffer);
//
	printf("READING RESULT:\n");
	int trig = 0;
	for(int i=0; i<read_FIFO_used;i++){
		int read_status = alt_up_rs232_read_data(rs232_dev, &data_R8, &p_error);
//		printf("Read Status %d\n", read_status);

//		if(data_R8 == '\n' && trig < 1){
//			//alt_printf("%c", data_R8);
//			trig = trig + 1;
//		}
//		else if(data_R8 != '\n' && trig == 1){
//			alt_printf("%c", data_R8);
//		}
//		else if(data_R8 == '\n' && trig == 1){
//			break;
//		}
//		if(data_R8 == ">"){
//			break;
//		}
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

void get_pixel(alt_up_rs232_dev* rs232_dev, int index){

	char index_str[50];
	itoa(index, index_str, 10);

	char myString1[] = "get_pixel(\"";
	char myString2[] = "\")";
	char result[150];

	int i = 0;
	int j = 0;
	while (myString1[i] != '\0') {
		result[j++] = myString1[i++];
	}
	i = 0;
	while (index_str[i] != '\0') {
		result[j++] = index_str[i++];
	}
	i = 0;
	while (myString2[i] != '\0') {
		result[j++] = myString2[i++];
	}
	result[j] = '\0';

	printf("Concatenated string: %s\n", result);


	send_code(rs232_dev, result);


}