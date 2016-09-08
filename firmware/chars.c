#include "pic24_all.h"

#define C_CHAR_0 (u8)(0x30)
#define C_CHAR_A_MINUS10 (u8)(0x37)

u8  u8_dividers[3]  = {1, 10, 100};
u16 u16_dividers[5] = {1, 10, 100, 1000, 10000};
u32 u32_dividers[6] = {1, 10, 100, 1000, 10000, 100000};

//nbChars includes the sign character!
//nbChars must be >= 2
//returns 0 if everything was OK, and 1 if there was an error (not enough characters to code the data)
u8 s16_to_chars_decimal (char *dest, u8 nbChars, s16 data) {
    char *tmp;
    u8 dividerIndex = nbChars -1;
    u16 tmpData;
    u16 currentFigure;
    
    tmp = dest;
    
    // handle data sign character, and initialize temporary working variable
    if (data >=0 ) {
        *tmp = '+';
        tmpData = data;
    } else {
        *tmp = '-';
        tmpData = -data;
    }
    tmp++;
    // convert number
    do {
        dividerIndex--;
        currentFigure = (tmpData / u16_dividers[dividerIndex]);
        if (currentFigure >= 10) return 1;
        *tmp = C_CHAR_0 + (u8)(currentFigure);
        tmpData -= currentFigure * u16_dividers[dividerIndex];
        tmp++;
    } while (dividerIndex > 0);
    return 0;
}

//nbChars includes the sign character!
//nbChars must be >= 2
//returns 0 if everything was OK, and 1 if there was an error (not enough characters to code the data)
u8 s8_to_chars_decimal (char *dest, u8 nbChars, s8 data) {
    char *tmp;
    u8 dividerIndex = nbChars -1;
    u8 tmpData;
    u8 currentFigure;
    
    tmp = dest;
    
    // handle data sign character, and initialize temporary working variable
    if (data >=0 ) {
        *tmp = '+';
        tmpData = data;
    } else {
        *tmp = '-';
        tmpData = -data;
    }
    tmp++;
    // convert number
    do {
        dividerIndex--;
        currentFigure = (tmpData / u8_dividers[dividerIndex]);
        if (currentFigure >= 10) return 1;
        *tmp = C_CHAR_0 + (u8)(currentFigure);
        tmpData -= currentFigure * u8_dividers[dividerIndex];
        tmp++;
    } while (dividerIndex > 0);
    return 0;
}

//nbChars must be >= 1
//returns 0 if everything was OK, and 1 if there was an error (not enough characters to code the data)
u8 u32_to_chars_decimal (char *dest, u8 nbChars, u32 data) {
    char *tmp;
    u8 dividerIndex = nbChars;
    u32 tmpData;
    u32 currentFigure;
    
    tmp = dest;
    
    // initialize temporary working variable
    tmpData = data;
    // convert number
    do {
        dividerIndex--;
        currentFigure = (tmpData / u32_dividers[dividerIndex]);
        if (currentFigure >= 10) return 1;
        *tmp = C_CHAR_0 + (u8)(currentFigure);
        tmpData -= currentFigure * u32_dividers[dividerIndex];
        tmp++;
    } while (dividerIndex > 0);
    return 0;
}

void u8_to_chars_hexadecimal (char *dest, u8 data) {
    char *tmp;
    u8 figure;
    tmp = dest;
    // first character
    figure = data >> 4;
    *tmp = (figure >= 10) ? (C_CHAR_A_MINUS10 + figure) : (C_CHAR_0 + figure);
    tmp++;
    //second character
    figure = data & 0x0F;
    *tmp = (figure >= 10) ? (C_CHAR_A_MINUS10 + figure) : (C_CHAR_0 + figure);
}
