
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
    const char *start = strstr(buffer, part);
    if (start) {
        start += strlen(part);
        const char *end = strchr(start, '\n');
        if (end) {
            int length = end - start;
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


char* extract_body(const char *buffer) {
    const char *start = strchr(buffer, '[');
    const char *end = strchr(buffer, ']');

    if (start && end) {
    	//if (DEBUG) alt_printf("Start marker found: %c\n", start);
    	//if (DEBUG) alt_printf("End marker found: %c\n",end);
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



//vga

void vga_place(int copytype, int block_x, int block_y, int start)
{
	unsigned arg = (copytype << 13) + (block_y << 8) + (block_x << 3) + start;
	*(vga_module + 1) = arg;
	*(vga_module) = 0;
	*vga_module;
}

//void cleanscreen(int* arr){
//    for (int i=0; i<15;i++){
//        for (int j=0; j<15;j++){
//            if((i != arr[0] && j != arr[1]) || (i != arr[2] && j != arr[3]) || (i != arr[4] && j != arr[5]) || (i != arr[6] && j != arr[7])){
//                vga_place(0, i, j, 1);
//            }
//        }
//    }
//}

void cleanscreen(int start){
    for (int i=0; i<15;i++){
        for (int j=0; j<15;j++){
        	vga_place(0, i, j, start);
        }
    }
}

//void cleanscreen(void){
//    for (int i=0; i<15;i++){
//        for (int j=0; j<15;j++){
//        	vga_place(0, i, j, 1);
//        }
//    }
//}

int parse_and_execute_data(char* commands) {
//	alt_printf("a");
    const char delim = ';';
    int start = 1;
    int cmd_len = 0;

    // Calculate the length of the input string
    while (commands[cmd_len] != '\0') {
        cmd_len++;
    }
//    alt_printf("b");

    // Create a buffer for tokens
    char token[cmd_len + 1];
    int token_len = 0;

//    alt_printf("c");

    for (int i = 0; i <= cmd_len; i++) {

        if (commands[i] != delim && commands[i] != '\0') {
            token[token_len++] = commands[i];
        } else {
            token[token_len] = '\0';
//            alt_printf(token);
//            alt_printf("+");

//            if (start == -1 && token[0] == 's') {
            if (token[0] == 's' && token[1] == 'a') {
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
                vga_place(10, block_x, 1, start);
                vga_place(10, block_x, 2, start);
                vga_place(10, block_x, 3, start);
                for (int i = 1; i < block_y; i++) {
                    vga_place(9, block_x, i, start);
                }
            } else if (token[0] == 's') {
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


//void parse_and_execute_data(char* commands, int* player_pos) {
//    const char delim = ';';
//    int start = -1;
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
////            if (DEBUG) alt_printf(token);
////            if (DEBUG) alt_printf("+");
//
//            if (token[0] == 's' && token[1] == 'a') {
//                start = token[1] == 'a' ? 1 : 0;
//            } else if (token[0] == 'p') {
//                int copytype = token[1] - '0';
//                int block_x = token[2] >= 'a' ? token[2] - 'a' + 10 : token[2] - '0';
//                int block_y = token[3] >= 'a' ? token[3] - 'a' + 10 : token[3] - '0';
//
//                if (start != -1) {
//                	if ((copytype >= 1) && (copytype <= 4)){
//						if ((player_pos[copytype * 2 - 2] != block_x) || (player_pos[copytype * 2 - 1] != block_y)) {
//							vga_place(0, player_pos[copytype * 2 - 2], player_pos[copytype * 2 - 1], 1);
//							player_pos[copytype * 2 - 2] = block_x;
//							player_pos[copytype * 2 - 1] = block_y;
//						}
//                	}
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
////                	alt_printf("inside token[1] = f \n");
////                	alt_printf("%c",token[2]);
//            	} else if(token[1] == 'w'){ // back to waiting page
//            		vga_place(0,0,0,0);
//            		start = -1;
//            	}
//            }
//
//
//            token_len = 0;
//        }
//    }
//}

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

//    char command_unregister[32];
//    replace_session_id("post_unregister(\"session_id=0\")", session_id, command_unregister);
//    if (DEBUG) alt_printf("Updated command_unregister: %s\n", command_unregister);

    usleep(500000); // Sleep for 0.5 seconds (500000 microseconds)

//    int* player_list = (int*) malloc(8 * sizeof(int));
//
//    for (int index = 0 ; index < 8; index++){
//    	player_list[index] = -1;
//    }


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
            if (DEBUG) alt_printf("Body: %s\n", body_str);
            cleanscreen(start);
            start = parse_and_execute_data(body_str);
            free(body_str);
        } else {
            if (DEBUG) alt_printf("lost\n");
        }

        // Sleep for a specified interval between requests
        usleep(100000); // Sleep for 0.5 seconds (500000 microseconds)
    }

	while(1);

    return 0;
}

//int main(){
//	char* body_str = "sa;p055;p156;p067;p266;p0cd;p3dd;h13;h22;h31;";
//	if (body_str != NULL) {
//		cleanscreen();
//		parse_and_execute_data("h11;h23;h32;sf1;");
//		usleep(5000000);
//		cleanscreen();
//		parse_and_execute_data("sf1234;");
//		usleep(5000000);
//		cleanscreen();
//		parse_and_execute_data("sw;");
//		usleep(5000000);
//		cleanscreen();
//		parse_and_execute_data("sa;p512;p623;p734;p845;");
//		usleep(5000000);
//		cleanscreen();
//		parse_and_execute_data("sa;p521;p632;p743;p854;");
//		usleep(5000000);
//		cleanscreen();
//		parse_and_execute_data("sf4325;");
//		usleep(5000000);
//		cleanscreen();
//		parse_and_execute_data("sf325;");
//		usleep(5000000);
//		cleanscreen();
//		parse_and_execute_data("sf25;");
//		usleep(5000000);
//	//		cleanscreen();
//	//		parse_and_execute_data("sw;");
//	//		usleep(5000000);
//
//	//		free(body_str);
//	} else {
//		alt_printf("lost\n");
//	}
//
//	while(1);
//
//	return 0;
//}
