#include "sys/alt_stdio.h"
//#include "sys/time.h"
#include "system.h"

#include <altera_up_avalon_rs232_regs.h>
#include <altera_up_avalon_rs232.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>


void vga_place(int copytype, int block_x, int block_y, int start);
void parse_and_execute_data(char* commands);
char* extract_body(const char *buffer);
void send_data(alt_up_rs232_dev* rs232_dev, const char* data);
int read_response(alt_up_rs232_dev *rs232_dev, char *buffer, int buffer_size);
void flush_read_fifo(alt_up_rs232_dev *rs232_dev);


volatile unsigned *vga_module = (volatile unsigned *) VGA_AVALON_0_BASE;


void vga_place(int copytype, int block_x, int block_y, int start)
{
	unsigned arg = (copytype << 11) + (block_y << 6) + (block_x << 1) + start;
	*(vga_module + 1) = arg;
	*(vga_module) = 0;
	*vga_module;
}


void parse_and_execute_data(char* commands) {
    const char* delim = ";";
    char* token;
    char* cmd_str = strdup(commands);
    int start = -1;

    token = strtok(cmd_str, delim);

    while (token != NULL) {
        if (start == -1 && token[0] == 's') {
            start = token[1] == 'a' ? 1 : 0;
        } else if (token[0] == 'p') {
            int copytype = token[1] - '0';
            int block_x = token[2] >= 'a' ? token[2] - 'a' + 10 : token[2] - '0';
            int block_y = token[3] >= 'a' ? token[3] - 'a' + 10 : token[3] - '0';

            if (start != -1) {
                vga_place(copytype, block_x, block_y, start);
            }
        } else if (token[0] == 'h') {
            int block_x = token[1] - '0' + 14;
            int block_y = token[2] - '0' + 1;
            vga_place(8, block_x, 1, start);
            vga_place(8, block_x, 2, start);
            vga_place(8, block_x, 3, start);
            if (start != -1) {
                for (int i = 1; i < block_y; i++) {
                    vga_place(7, block_x, i, start);
                }
            }
        }

        token = strtok(NULL, delim);
    }

    free(cmd_str);
}



//wifi
void send_data(alt_up_rs232_dev* rs232_dev, const char* data) {
    // Calculate the length of the data string
    int length = 0;
    while (data[length] != '\0') {
        length++;
    }

    // Write the data to the RS232 UART core
    int i;
    unsigned available_space;
    for (i = 0; i < length; i++) {
        do {
            available_space = alt_up_rs232_get_available_space_in_write_FIFO(rs232_dev);
        } while (available_space == 0);

        alt_up_rs232_write_data(rs232_dev, data[i]);
        printf("%c", data[i]);
    }

    // Write a newline character to signify the end of the string
    do {
        available_space = alt_up_rs232_get_available_space_in_write_FIFO(rs232_dev);
    } while (available_space == 0);

    alt_up_rs232_write_data(rs232_dev, '\n');
}

void flush_read_fifo(alt_up_rs232_dev *rs232_dev) {
    alt_u8 data, parity_error;
    while (alt_up_rs232_get_used_space_in_read_FIFO(rs232_dev) > 0) {
        alt_up_rs232_read_data(rs232_dev, &data, &parity_error);
    }
}

int read_response(alt_up_rs232_dev *rs232_dev, char *buffer, int buffer_size) {
    int index = 0;
    alt_u8 data, parity_error;

    printf("Waiting for data...\n");

    while (1) {
        // Check if there is data available in the read FIFO
        if (alt_up_rs232_get_used_space_in_read_FIFO(rs232_dev) > 0) {
            // Read one byte of data
            if (alt_up_rs232_read_data(rs232_dev, &data, &parity_error) == 0) {
                printf("%c", data); // Print received data for debugging

                // Ignore certain characters
                if (data != '>' && data != '\r') {
                    buffer[index++] = data;
                }
                // If the ']' character is detected, break the loop
                if (data == ']') {
                    break;
                }

            }
        }
    }

    // Add null terminator to the buffer
    buffer[index] = '\0';
    //printf("Here is the buffer: %s\n",buffer);

    return index;
}

char* extract_body(const char *buffer) {
    const char *start = strchr(buffer, '[');
    const char *end = strchr(buffer, ']');

    if (start && end) {
    	//printf("Start marker found: %c\n", start);
    	//printf("End marker found: %c\n",end);
        start++; // Move past the '[' character
        int length = end - start;
        if (length > 0) {
            char *result = (char *)malloc(length + 1);
            if (result) {
                strncpy(result, start, length);
                result[length] = '\0';
            }
            return result;
        }
    }
    return NULL;
}

int main() {
    alt_up_rs232_dev *rs232_dev = alt_up_rs232_open_dev("/dev/rs232_0");

    if (rs232_dev == NULL) {
        // failed to open device
        return -1;
    }

    // Load the print.lua script
    send_data(rs232_dev, "dofile(\"print.lua\")");
    usleep(1000000); // Wait 0.5 second for the script to load

    for (int i = 0; i < 10; i++) {
        // Call the print_test() function
        send_data(rs232_dev, "get_webpage()");

        // Flush the read FIFO before reading data
        flush_read_fifo(rs232_dev);

        char buffer[1024];
        read_response(rs232_dev, buffer, sizeof(buffer));

        // Extracting the body of the message
        char* body_str = extract_body(buffer);
        if (body_str != NULL) {
            printf("Body: %s\n", body_str);
//            parse_and_execute_data(body_str);
            free(body_str);
        } else {
            printf("lost\n");
        }

        printf("%d\n", i);

        // Sleep for a specified interval between requests
        usleep(700000); // Sleep for 0.5 seconds (500000 microseconds)
    }

    return 0;
}


