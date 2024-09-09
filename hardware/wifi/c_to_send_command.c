#include <altera_up_avalon_rs232_regs.h>
#include <altera_up_avalon_rs232.h>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int starts_with(const char *str, const char *prefix);
char* extract_response_part(const char *buffer, const char *part);
char* extract_body(const char *buffer);

void send_data(alt_up_rs232_dev* rs232_dev, const char* data);
int read_response(alt_up_rs232_dev *rs232_dev, char *buffer, int buffer_size);
void flush_read_fifo(alt_up_rs232_dev *rs232_dev);
long get_time_ms();




//***************************************//
//
//***************************************//



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


long get_time_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}

//int read_response(alt_up_rs232_dev *rs232_dev, char *buffer, int buffer_size) {
//    int index = 0;
//    alt_u8 data, parity_error;
//    long start_time = get_time_ms();
//    long no_data_wait_time = 500; // Wait time of 500 ms with no new data
//    int line_empty = 1;
//
//    printf("Waiting for data...\n");
//
//    while (1) {
//        // Check if there is data available in the read FIFO
//        if (alt_up_rs232_get_used_space_in_read_FIFO(rs232_dev) > 0) {
//            // Read one byte of data
//            if (alt_up_rs232_read_data(rs232_dev, &data, &parity_error) == 0) {
//                printf("%c", data); // Print received data for debugging
//            	//usleep(100);
//                // Check for newline character or buffer overflow
//                if (data == '\r' || index >= buffer_size - 1) {
//                    if (line_empty) {
//                        break; // Stop reading if two newline characters are received consecutively
//                    }
//                    line_empty = 1;
//                } else {
//                    line_empty = 0;
//                }
//
//                // Ignore certain characters
//                if (data != '>' && data != '\r') {
//                	//printf("%s\n", data);
//                    buffer[index++] = data;
//                }
//
//                start_time = get_time_ms(); // Reset the start time when new data is received
//            }
//        } else {
//            // Check for timeout
//            if (get_time_ms() - start_time > no_data_wait_time) {
//                printf("Timeout waiting for data.\n");
//                break;
//            }
//        }
//    }
//    // Add these lines after the while loop in the read_response function
//    //printf("\nBytes read: %d\n", index);
//    //printf("Buffer content: %s\n", buffer);
//
//
//    // Add null terminator to the buffer
//    buffer[index] = '\0';
//
//    return index;
//}

int read_response(alt_up_rs232_dev *rs232_dev, char *buffer, int buffer_size) {
    int index = 0;
    alt_u8 data, parity_error;
    long start_time = get_time_ms();
    long no_data_wait_time = 500; // Wait time of 500 ms with no new data
    int line_empty = 1;
    int prev_data = -1;

    printf("Waiting for data...\n");

    while (1) {
        // Check if there is data available in the read FIFO
        if (alt_up_rs232_get_used_space_in_read_FIFO(rs232_dev) > 0) {
            // Read one byte of data
            if (alt_up_rs232_read_data(rs232_dev, &data, &parity_error) == 0) {
                printf("%c", data); // Print received data for debugging
                //usleep(100);

                // Check for newline character or buffer overflow
                if ((data == '\n' && prev_data == '\r') || index >= buffer_size - 1) {
                    if (line_empty) {
                        break; // Stop reading if two newline characters are received consecutively
                    }
                    line_empty = 1;
                } else {
                    line_empty = 0;
                }

                // Ignore certain characters
                if (data != '>' && data != '\r') {
                    buffer[index++] = data;
                }

                start_time = get_time_ms(); // Reset the start time when new data is received
            }
        } else {
            // Check for timeout
            if (get_time_ms() - start_time > no_data_wait_time) {
                printf("Timeout waiting for data.\n");
                break;
            }
        }
        prev_data = data;
    }

    // Add null terminator to the buffer
    buffer[index] = '\0';

    return index;
}

int starts_with(const char *str, const char *prefix) {
    while (*prefix) {
        if (*str++ != *prefix++) {
            return 0;
        }
    }
    return 1;
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
    const char *start = strstr(buffer, "\r\n\r\n");

    // If the standard sequence is not found, try other newline sequences
    if (!start) {
        start = strstr(buffer, "\n\r\n\r");
    }
    if (!start) {
        start = strstr(buffer, "\n\n");
    }

    if (start) {
        // Add the appropriate number of bytes depending on the newline sequence found
        if (strncmp(start, "\r\n\r\n", 4) == 0) {
            start += 4;
        } else if (strncmp(start, "\n\r\n\r", 4) == 0) {
            start += 4;
        } else if (strncmp(start, "\n\n", 2) == 0) {
            start += 2;
        }

        int length = strlen(start);
        //printf("Debug: Body length: %d\n", length); // Debugging print statement
        if (length > 0) {
            char *result = (char *)malloc(length + 1);
            if (result) {
                strncpy(result, start, length);
                result[length] = '\0';
                //printf("Debug: Extracted body: %s\n", result); // Debugging print statement
            }
            return result;
        }
    }
    return NULL;
}


//***********************************************
// test one request and parsing
//***********************************************

//int main() {
//    alt_up_rs232_dev *rs232_dev = alt_up_rs232_open_dev("/dev/rs232_0");
//
//    if (rs232_dev == NULL) {
//        // failed to open device
//        return -1;
//    }
//
//    // Load the print.lua script
//    send_data(rs232_dev, "dofile(\"print.lua\")");
//    usleep(1000000); // Wait 1 second for the script to load
//
//    // Call the print_test() function
//    send_data(rs232_dev, "get_webpage()");
//    //send_data(rs232_dev, "print_test()");
//
//    // Flush the read FIFO before reading data
//    flush_read_fifo(rs232_dev);
//
//    char buffer[4096];
//	read_response(rs232_dev, buffer, sizeof(buffer));
//    //printf("\n");
//    //printf("Received: %s\n", buffer);
//
//		printf("\n");
//		printf("Test for parsing\n");
//
//		int content_length = -1;
//
//		    // Extracting the "Content-Length" part from the buffer
//		    char* content_length_str = extract_response_part(buffer, "Content-Length: ");
//		    if (content_length_str != NULL) {
//		        content_length = atoi(content_length_str);
//
//		        printf("Content-Length: %d\n", content_length);
//		        free(content_length_str);
//		    }
//
//		    // Extracting the "Date" part from the buffer
//		    char* date_str = extract_response_part(buffer, "Date: ");
//		    if (date_str != NULL) {
//		        printf("Date: %s\n", date_str);
//		        free(date_str);
//		    }
//
//		    // Extracting the body of the message
//		    char* body_str = extract_body(buffer);
//		    if (body_str != NULL) {
//		        printf("Body: %s\n", body_str);
//		        free(body_str);
//		    } else {
//		        printf("Body not found.\n");
//		    }
//
//
//
//    return 0;
//}


//********************************************************
//
//***********************************************************

int main() {
    alt_up_rs232_dev *rs232_dev = alt_up_rs232_open_dev("/dev/rs232_0");

    if (rs232_dev == NULL) {
        // failed to open device
        return -1;
    }

    // Load the print.lua script
    send_data(rs232_dev, "dofile(\"print.lua\")");
    usleep(1000000); // Wait 1 second for the script to load

    // Continuously send GET requests and process the received data
//    while (1) {
//        // Call the print_test() function
//        send_data(rs232_dev, "get_webpage()");
//
//        // Flush the read FIFO before reading data
//        flush_read_fifo(rs232_dev);
//
//        char buffer[4096];
//        read_response(rs232_dev, buffer, sizeof(buffer));
//
//         //Extracting the body of the message
//        	char* body_str = extract_body(buffer);
//        	if (body_str != NULL) {
//        		printf("Body: 1-------1\n");
//        		free(body_str);
//            }
//
//        // Process the received data, e.g., render on VGA
//        // ...
//
//        // Sleep for a specified interval between requests
//        usleep(100000); // Sleep for 0.5 seconds (500000 microseconds)
//    }

    for(int i = 0; i<10; i++) {
            // Call the print_test() function
            send_data(rs232_dev, "get_webpage()");

            // Flush the read FIFO before reading data
            flush_read_fifo(rs232_dev);

            char buffer[4096];
            read_response(rs232_dev, buffer, sizeof(buffer));

             //Extracting the body of the message
            	char* body_str = extract_body(buffer);
            	if (body_str != NULL) {
            		printf("Body: %d\n",i);
            		free(body_str);
                }

            // Process the received data, e.g., render on VGA
            // ...

            // Sleep for a specified interval between requests
            usleep(100000); // Sleep for 0.5 seconds (500000 microseconds)
        }

    return 0;
}


