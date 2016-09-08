#ifndef CHARS_H
#define	CHARS_H

//nbChars includes the sign character!
//nbChars must be >= 2
u8 s16_to_chars_decimal (char *dest, u8 nbChars, s16 data) ;

//nbChars includes the sign character!
//nbChars must be >= 2
u8 s8_to_chars_decimal (char *dest, u8 nbChars, s8 data) ;

//nbChars must be >= 1
u8 u32_to_chars_decimal (char *dest, u8 nbChars, u32 data);

void u8_to_chars_hexadecimal (char *dest, u8 data) ;

#endif	/* CHARS_H */
