#include <stdbool.h>
#include <stdint.h>
#include <string.h>

bool isNumber(const char* number) {
    for(size_t i = 0; i < strlen(number); i++) {
        if(number[i] < '0' || number[i] > '9') return false;
    }
    return true;
}