#ifndef COTTON_INIT_ERROR_H
#define COTTON_INIT_ERROR_H


/*
error_t:

S = status
e = error

SEEEEEEE

1 bit: success/fail
7 bits: simple error message (like fiel not found etc)

*/

typedef error_t unsigned char;

#define ERROR_STAT(x) ((unsigned char)((x >> 7) & 1))
#define ERROR_ERR(x) ((unsigned char)(x & 0b01111111))

#define ERROR_MAKE(x, y) ((unsigned char)(((x & 1) << 7) | (y & 0b01111111)))

// idk what errors to add yet
// #define ERROR_

#endif