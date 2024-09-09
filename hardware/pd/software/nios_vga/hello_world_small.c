#include "sys/alt_stdio.h"
//#include "sys/time.h"
#include "system.h"

#include <altera_avalon_pio_regs.h>
#include <altera_up_avalon_rs232_regs.h>
#include <altera_up_avalon_rs232.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>



//char* extract_response_part(const char *buffer, const char *part);
//char* extract_body(const char *buffer);
//void send_data(alt_up_rs232_dev* rs232_dev, const char* data);
//int read_response(alt_up_rs232_dev *rs232_dev, char *buffer, int buffer_size);
//void flush_read_fifo(alt_up_rs232_dev *rs232_dev);

volatile unsigned *vga_module = (volatile unsigned *) VGA_AVALON_0_BASE;
//
//void vga_place(int copytype, int block_x, int block_y, int start);
//void parse_and_execute_data(char* commands);

int DEBUG = 1;

typedef struct {
    int x;
    int y;
} Coordinate;

Coordinate block_coordinates[4];

// HEX
int hex7seg(unsigned d) {
    const unsigned digits[] = { 0x40,  0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10 };
    return (d < 10) ? digits[d] : 0x3f;
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
        if (DEBUG) alt_printf("%c", data[i]);
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

    //if (DEBUG) alt_printf("Waiting for data...\n");

    while (1) {
        // Check if there is data available in the read FIFO
        if (alt_up_rs232_get_used_space_in_read_FIFO(rs232_dev) > 0) {
            // Read one byte of data
            if (alt_up_rs232_read_data(rs232_dev, &data, &parity_error) == 0) {
//                if (DEBUG) alt_printf("%c", data); // Print received data for debugging

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
    // if (DEBUG) alt_printf("Here is the buffer: %s\n",buffer);

    return index;
}

char* extract_response_part(const char *buffer, const char *part) {
    char *start = strstr(buffer, part);
    if (start) {
        start += strlen(part);
        char *end = strchr(start, '\n');
        if (end) {
//            int length = end - start;
//            char *result = (char *)malloc(length + 1);
//            if (result) {
//                strncpy(result, start, length);
//                result[length] = '\0';
//            }
//            return result;
        	*end = '\0';
        	return (start);
        }
    }
    return NULL;
}


char* extract_body(const char *buffer) {
    char *start = strchr(buffer, '[');
    char *end = strchr(buffer, ']');

    if (start && end) {
    	//if (DEBUG) alt_printf("Start marker found: %c\n", start);
    	//if (DEBUG) alt_printf("End marker found: %c\n",end);
//        start++; // Move past the '[' character
//        int length = end - start;
//        if (length > 0) {
//            char *result = (char *)malloc(length + 1);
//            if (result) {
//                strncpy(result, start, length);
//                result[length] = '\0';
//            }
//            return result;
//        }

    	//change end to terminating character
    	*end = '\0';
    	return (start + 1);
    }
    return NULL;
}



//vga

void vga_place(int copytype, int block_x, int block_y, int start)
{
	unsigned arg = (copytype << 13) + (block_y << 8) + (block_x << 3) + start;
	*(vga_module + 1) = arg;
	*(vga_module) = 0;
	*vga_module;
}

//void cleanscreen(int start){
//    for (int i = 0; i < 15; i++){
//        for (int j = 0; j < 15; j++){
//            vga_place(0, i, j, start);
//        }
//    }
//}

void cleanscreen(int start){
    for (int i = 0; i < 15; i++){
        for (int j = 0; j < 15; j++){
            int should_clean = 1;
            for (int k = 0; k < 4; k++) {
                if (block_coordinates[k].x == i && block_coordinates[k].y == j) {
                    should_clean = 0;
                    break;
                }
            }
            if (should_clean == 1) {
                vga_place(0, i, j, start);
            }
        }
    }
}

//int parse_and_execute_data(char* commands) {
//    const char delim = ';';
//    int start = 1;
//    int cmd_len = 0;
//
//    // Calculate the length of the input string
//    while (commands[cmd_len] != '\0') {
//        cmd_len++;
//    }
//
//    // Create a buffer for tokens
//    char token[cmd_len + 1];
//    int token_len = 0;
//
//    for (int i = 0; i <= cmd_len; i++) {
//
//        if (commands[i] != delim && commands[i] != '\0') {
//            token[token_len++] = commands[i];
//        } else {
//            token[token_len] = '\0';
//
//            if (token[0] == 's' && token[1] == 'a') {
//                start = token[1] == 'a' ? 1 : 0;
//            } else if (token[0] == 'p') {
//                int copytype = token[1] - '0';
//                int block_x = token[2] >= 'a' ? token[2] - 'a' + 10 : token[2] - '0';
//                int block_y = token[3] >= 'a' ? token[3] - 'a' + 10 : token[3] - '0';
//
//                if (start != -1) {
//                	vga_place(copytype, block_x, block_y, start);
////                	if (copytype >= 1 && copytype <= 4) {
////                		if ((block_coordinates[copytype - 1].x != block_x) || (block_coordinates[copytype - 1].y != block_y)){
////                        	if (block_coordinates[copytype - 1].x >= 0 && block_coordinates[copytype - 1].y >= 0){
////                        		vga_place(0, block_coordinates[copytype - 1].x, block_coordinates[copytype - 1].y, start);
////                        	}
////                			vga_place(copytype, block_x, block_y, start);
////                			block_coordinates[copytype - 1].x = block_x;
////                			block_coordinates[copytype - 1].y = block_y;
////                		}
////                		block_coordinates[copytype - 1].visited = 1;
////                    }
////                	else{
////                		vga_place(copytype, block_x, block_y, start);
////                	}
//                }
//            } else if (token[0] == 'h') {
//                int block_x = token[1] - '0' + 14;
//                int block_y = token[2] - '0' + 1;
//                vga_place(10, block_x, 1, start);
//                vga_place(10, block_x, 2, start);
//                vga_place(10, block_x, 3, start);
//                for (int i = 1; i < block_y; i++) {
//                    vga_place(9, block_x, i, start);
//                }
//            } else if (token[0] == 's') {
//            	if (token[1] == 'f'){
//            		if (token[2] == '1') vga_place(0,0,0,2);
//            		else if (token[2] == '2') vga_place(0,0,0,3);
//            		else if (token[2] == '3') vga_place(0,0,0,4);
//            		else if (token[2] == '4') vga_place(0,0,0,5);
//            	} else if(token[1] == 'w'){ // back to waiting page
//            		vga_place(0,0,0,0);
//            		start = 0;
//            	}
//            }
//
//            token_len = 0;
//        }
//    }
//
//    return start;
//}
int parse_and_execute_data(char* commands) {
    const char delim = ';';
    int start = 1;
    int cmd_len = 0;

    // Calculate the length of the input string
    while (commands[cmd_len] != '\0') {
        cmd_len++;
    }

    // Create a buffer for tokens
    char token[cmd_len + 1];
    int token_len = 0;

    for (int i = 0; i <= cmd_len; i++) {

        if (commands[i] != delim && commands[i] != '\0') {
            token[token_len++] = commands[i];
        } else {
            token[token_len] = '\0';

            if (token[0] == 's' && token[1] == 'a') {
                start = token[1] == 'a' ? 1 : 0;
            } else if (token[0] == 'p') {
                for (int i = 1; i < 5; i++){
                    int block_x = token[2*i-1] >= 'a' ? token[2*i-1] - 'a' + 10 : token[2*i-1] - '0';
                    int block_y = token[2*i] >= 'a' ? token[2*i] - 'a' + 10 : token[2*i] - '0';

                    if ((block_coordinates[i - 1].x != block_x) || (block_coordinates[i - 1].y != block_y)){
                    	vga_place(0, block_coordinates[i - 1].x, block_coordinates[i - 1].y, start);
                    	block_coordinates[i - 1].x = block_x;
                    	block_coordinates[i - 1].y = block_y;
                    }
                    if (start != -1 && block_x != 15 && block_y != 15) {
                        vga_place(i, block_x, block_y, start);
                    }
                }

            } else if (token[0] == 'h') {
                for(int j = 15; j < 19; j++){
                    vga_place(10, j, 1, start);
                    vga_place(10, j, 2, start);
                    vga_place(10, j, 3, start);
                }

                for (int i = 1; i < 5; i++){
                    int health = token[i] - '0';

                    for (int k = 1; k <= health; k++) {
                        vga_place(9, i+14, k, start);
                    }
                }

            } else if (token[0] == 'b') { //b - variable length, consisting of 'txy' type and xy coordinate information
                for(int j = 1; j < token_len; j=j+3){
                    int copytype = token[j] - '0';
                    int block_x = token[j+1] >= 'a' ? token[j+1] - 'a' + 10 : token[j+1] - '0';
                    int block_y = token[j+2] >= 'a' ? token[j+2] - 'a' + 10 : token[j+2] - '0';

                    if (start != -1) {
                        vga_place(copytype, block_x, block_y, start);
                    }
                }

            }else if (token[0] == 'e') { //e - variable length, consisting of 'txy' type and xy coordinate information
                for(int j = 1; j < token_len; j=j+3){
                    int bombtype = token[j] - '0';
                    int block_x = token[j+1] >= 'a' ? token[j+1] - 'a' + 10 : token[j+1] - '0';
                    int block_y = token[j+2] >= 'a' ? token[j+2] - 'a' + 10 : token[j+2] - '0';

                    if (start != -1 && bombtype == 5) {
                        for(int i = block_x - 1; i <= block_x+1; i++){
                            for(int k = block_y - 1; k <= block_y + 1; k++){
                            	if (i >= 0 && i <= 14 && k >= 0 && k <=14){
                            		vga_place(8, i, k, start);
                            	}
                            }
                        }
                    }else if (start != -1 && bombtype == 6) {
                        for(int i = 0; i <= 14; i++){
                            vga_place(8, block_x, i, start);
                        }
                    }else if (start != -1 && bombtype == 7) {
                        for(int i = 0; i <= 14; i++){
                            vga_place(8, i, block_y, start);
                        }
                    }
                }

            }else if (token[0] == 's') {
            	if (token[1] == 'f'){
            		if (token[2] == '1') vga_place(0,0,0,2);
            		else if (token[2] == '2') vga_place(0,0,0,3);
            		else if (token[2] == '3') vga_place(0,0,0,4);
            		else if (token[2] == '4') vga_place(0,0,0,5);
            	} else if(token[1] == 'w'){ // back to waiting page
            		vga_place(0,0,0,0);
            		start = 0;
            	}
            }

            token_len = 0;
        }
    }

    return start;
}


int find_session_id(const char *cookie_str) {
    const char *ptr = cookie_str;
    int session_id = 0;
    int found = 0;

    while (*ptr) {
        if ((*ptr == 's') && (*(ptr + 1) == 'e') && (*(ptr + 2) == 's') && (*(ptr + 3) == 's') &&
            (*(ptr + 4) == 'i') && (*(ptr + 5) == 'o') && (*(ptr + 6) == 'n') && (*(ptr + 7) == '_') &&
            (*(ptr + 8) == 'i') && (*(ptr + 9) == 'd') && (*(ptr + 10) == '=')) {
            ptr += 11;
            found = 1;
            break;
        }
        ptr++;
    }

    if (found) {
        while (*ptr >= '0' && *ptr <= '9') {
            session_id = session_id * 10 + (*ptr - '0');
            ptr++;
        }
    }

    return session_id;
}

void replace_session_id(const char *template, int session_id, char *output) {
    char *command_ptr = output;
    const char *ptr = template;

    while (*ptr) {
        if ((*ptr == 's') && (*(ptr + 1) == 'e') && (*(ptr + 2) == 's') && (*(ptr + 3) == 's') &&
            (*(ptr + 4) == 'i') && (*(ptr + 5) == 'o') && (*(ptr + 6) == 'n') && (*(ptr + 7) == '_') &&
            (*(ptr + 8) == 'i') && (*(ptr + 9) == 'd') && (*(ptr + 10) == '=')) {
            const char *session_id_str = "session_id=";
            while (*session_id_str) {
                *command_ptr++ = *session_id_str++;
            }
            ptr += 11;

            int temp_session_id = session_id;
            int length = 0;
            while (temp_session_id > 0) {
                length++;
                temp_session_id /= 10;
            }
            temp_session_id = session_id;
            for (int i = 1; i <= length; i++) {
                command_ptr[length - i] = (char)((temp_session_id % 10) + '0');
                temp_session_id /= 10;
            }
            command_ptr += length;

            // Skip the digits in the template string
            while (*ptr >= '0' && *ptr <= '9') {
                ptr++;
            }
        } else {
            *command_ptr++ = *ptr++;
        }
    }
    *command_ptr = '\0';
}



int main() {
	static unsigned int hex0;
	static unsigned int hex1;
	static unsigned int hex2;
	static unsigned int digits[3] = {-1,-1,-1};

	hex0 = 0xff;
	hex1 = 0xff;
	hex2 = 0xff;
	IOWR_ALTERA_AVALON_PIO_DATA(HEX0_BASE, hex2 << 14 | hex1 << 7 | hex0 );
	IOWR_ALTERA_AVALON_PIO_DATA(HEX1_BASE, 0xff << 14 | 0xff << 7 | 0xff );

	for (int i = 0; i < 4; i++){
		block_coordinates[i].x = 15;
		block_coordinates[i].y = 15;
	}

	int start = 0;
	vga_place(0,0,0,0);
	int session_id = 0;
    alt_up_rs232_dev *rs232_dev = alt_up_rs232_open_dev("/dev/rs232_0");

    if (rs232_dev == NULL) {
        // failed to open device
        return -1;
    }

    // Load the print.lua script
    send_data(rs232_dev, "dofile(\"print.lua\")");
    usleep(1000000); // Wait 0.5 second for the script to load

    // Register and get session_id
    send_data(rs232_dev, "post_register()");
    flush_read_fifo(rs232_dev);
    char buffer[1024];
    read_response(rs232_dev, buffer, sizeof(buffer));
    char* cookie_str = extract_response_part(buffer, "Set-Cookie: ");
    session_id = find_session_id(cookie_str);
    alt_printf("session_id: %x\n", session_id);

    char command_render[32];
    replace_session_id("get_render(\"session_id=0\")", session_id, command_render);

    int j = 0;
    if (session_id == 0){
    	digits[0] = 0;
    }
	while (session_id !=0 && j < 3){
		int temp = session_id % 10;
		digits[j] = temp;
		j++;
		session_id = session_id/10;
	}
	hex0 = digits[0] != -1? hex7seg(digits[0]): 0xff;
	hex1 = digits[1] != -1? hex7seg(digits[1]): 0xff;
	hex2 = digits[2] != -1? hex7seg(digits[2]): 0xff;
	IOWR_ALTERA_AVALON_PIO_DATA(HEX0_BASE, hex2 << 14 | hex1 << 7 | hex0 );

    usleep(500000); // Sleep for 0.5 seconds (500000 microseconds)

    while(1) {
    	//if (DEBUG) alt_printf("a");
    	//if (DEBUG) alt_printf("Updated command_render: %s\n", command_render);
//         // Call the print_test() function
//    	send_data(rs232_dev, "get_webpage()");
//    	send_data(rs232_dev, "post_register()");
        send_data(rs232_dev, command_render);
////   	send_data(rs232_dev, "post_unregister(\"session_id=27\")");
//
//        // Flush the read FIFO before reading data
        flush_read_fifo(rs232_dev);
//
//        char buffer[1024];
        read_response(rs232_dev, buffer, sizeof(buffer));

        // Extracting the body of the message
        char* body_str = extract_body(buffer);
        if (body_str != NULL) {
            alt_printf("Body: %s\n", body_str);
            cleanscreen(start);
            //alt_printf("%s", body_str);
            start = parse_and_execute_data(body_str);
            //alt_printf("a\n");
        } else {
            alt_printf("lost\n");
        }

        // Sleep for a specified interval between requests
        usleep(100000); // Sleep for 0.5 seconds (500000 microseconds)
    }

	while(1);

    return 0;
}

//int main() {
//	static unsigned int hex0;
//	static unsigned int hex1;
//	static unsigned int hex2;
//	static unsigned int digits[3] = {-1, -1, -1};
//
//	hex0 = 0xff;
//	hex1 = 0xff;
//	hex2 = 0xff;
//	IOWR_ALTERA_AVALON_PIO_DATA(HEX0_BASE, hex2 << 14 | hex1 << 7 | hex0 );
//	IOWR_ALTERA_AVALON_PIO_DATA(HEX1_BASE, 0xff << 14 | 0xff << 7 | 0xff );
//
//    int session_id = 56;
//    alt_printf("session_id: %x\n", session_id);
//
//    int j = 0;
//    if (session_id == 0){
//    	digits[0] = 0;
//    }
//	while (session_id !=0 && j < 3){
//		int temp = session_id % 10;
//		digits[j] = temp;
//		j++;
//		session_id = session_id/10;
//	}
//	hex0 = digits[0] != -1? hex7seg(digits[0]): 0xff;
//	hex1 = digits[1] != -1? hex7seg(digits[1]): 0xff;
//	hex2 = digits[2] != -1? hex7seg(digits[2]): 0xff;
//	IOWR_ALTERA_AVALON_PIO_DATA(HEX0_BASE, hex2 << 14 | hex1 << 7 | hex0 );
//
//    usleep(500000); // Sleep for 0.5 seconds (500000 microseconds)
//
//	while(1);
//
//    return 0;
//}

//int main(){
//		for (int i = 0; i < 4; i++){
//        	block_coordinates[i].x = 15;
//        	block_coordinates[i].y = 15;
//		}
//		int start = 0;
//		cleanscreen(start);
//		start = parse_and_execute_data("sw;");
//		usleep(100000);
//		cleanscreen(start);
//		start = parse_and_execute_data("sa;p11223344;h1231;b512523534545;e512523534545;");
//		usleep(100000);
//		cleanscreen(start);
//		start = parse_and_execute_data("sa;pff112233;h0123;b612713514715;b612713514715;");
//		usleep(100000);
//		cleanscreen(start);
//		start = parse_and_execute_data("sa;pff112233;h0123;b612713514715;");
//		usleep(100000);
//		cleanscreen(start);
//		start = parse_and_execute_data("sa;pff112233;h0123;b612713514715;");
//		usleep(100000);
//		cleanscreen(start);
//		start = parse_and_execute_data("sa;pff112233;h0123;b612713514715;");
//		usleep(100000);
//		cleanscreen(start);
//		start = parse_and_execute_data("sa;pff112233;h0123;b612713514715;");
//		usleep(100000);
//		for (int i = 0; i < 20; i++){
//			cleanscreen(start);
//			start = parse_and_execute_data("sa;pff112233;h0123;b612713514715;");
//			usleep(100000);
//		}
//		cleanscreen(start);
//		start = parse_and_execute_data("sa;pff11ff22;h0102;b712;e612;");
//		usleep(100000);
//		cleanscreen(start);
//		start = parse_and_execute_data("sa;p1122ff33;h0031;b612712614555;e712;");
//		usleep(100000);
//		cleanscreen(start);
//		start = parse_and_execute_data("sa;p11ffffff;");
//		usleep(100000);
//		cleanscreen(start);
//		start = parse_and_execute_data("sa;p12233445;");
//		usleep(100000);
//		cleanscreen(start);
//		start = parse_and_execute_data("sa;pffffffff;h1111;");
//		usleep(100000);
//		cleanscreen(start);
//		start = parse_and_execute_data("sa;p55ff66ff;h1230;b521;");
//		usleep(100000);
//		cleanscreen(start);
//		start = parse_and_execute_data("h1230;sf1234;");
//		usleep(100000);
//	while(1);
//
//	return 0;
//}