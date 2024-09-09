#include <stdio.h>
#include <stdint.h>

typedef struct {
    uint8_t segment1;
    uint8_t segment2;
    uint8_t segment3[4];
} BitInfo;

void process_array(uint16_t input, BitInfo *info) {
    info->segment1 = input & 0b111;           // Extract the first 3 bits
    info->segment2 = (input >> 3) & 0b1111;   // Extract the next 4 bits
    for (int i = 0; i < 4; i++) {
        info->segment3[i] = (input >> (7 + 2 * i)) & 0b11; // Extract 2 bits for each value in segment 3
    }
}

int main() {
    uint16_t input = 0b101010101010111; // 15-bit input
    BitInfo info;

    process_array(input, &info);

    printf("Segment 1: %d\n", info.segment1);
    printf("Segment 2: %d\n", info.segment2);
    printf("Segment 3 values:\n");
    for (int i = 0; i < 4; i++) {
        printf("Value %d: %d\n", i + 1, info.segment3[i]);
    }

    return 0;
}


// void process_array(const uint8_t input[15], BitInfo *info) {
//     uint16_t bitValue = 0;
//     for (int i = 0; i < 15; i++) {
//         bitValue |= input[i] << (14 - i);
//     }

//     info->segment1 = bitValue & 0b111;           // Extract the first 3 bits
//     info->segment2 = (bitValue >> 3) & 0b1111;   // Extract the next 4 bits
//     for (int i = 0; i < 4; i++) {
//         info->segment3[i] = (bitValue >> (7 + 2 * i)) & 0b11; // Extract 2 bits for each value in segment 3
//     }
// }

// int main() {
//     uint8_t input[15] = {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1}; // 15-element input array
//     BitInfo info;

//     process_array(input, &info);

//     printf("Segment 1: %d\n", info.segment1);
//     printf("Segment 2: %d\n", info.segment2);
//     printf("Segment 3 values:\n");
//     for (int i = 0; i < 4; i++) {
//         printf("Value %d: %d\n", i + 1, info.segment3[i]);
//     }

//     return 0;
// }