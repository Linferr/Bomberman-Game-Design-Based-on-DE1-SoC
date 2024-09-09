int read_line(alt_up_rs232_dev *rs232_dev, char *buffer, int buffer_size) {
    int index = 0;
    alt_u8 data, parity_error;

    while (1) {
        // Check if there is data available in the read FIFO
        if (alt_up_rs232_get_used_space_in_read_FIFO(rs232_dev) > 0) {
            // Read one byte of data
            if (alt_up_rs232_read_data(rs232_dev, &data, &parity_error) == 0) {
                // Check for newline character or buffer overflow
                if (data == '\n' || index >= buffer_size - 1) {
                    break;
                }
                // Ignore carriage return character
                if (data != '\r') {
                    buffer[index++] = data;
                }
            }
        }
    }

    // Add null terminator to the buffer
    buffer[index] = '\0';

    return index;
}

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

int main() {
    alt_up_rs232_dev *rs232_dev = alt_up_rs232_open_dev("/dev/rs232_0");

    if (rs232_dev == NULL) {
        // failed to open device
        return -1;
    }

    usleep(1000000); // 1 second
    send_data(rs232_dev, "get_test()");

    usleep(1000000);

    char buffer[1024];
    while (1) {
        int line_length = read_line(rs232_dev, buffer, sizeof(buffer));
        if (line_length == 0) {
            break;
        }
        printf("Received: %s\n", buffer);
    }

    return 0;
}
