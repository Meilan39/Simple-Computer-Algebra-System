#ifndef CONSTANTS
#define CONSTANTS

#include <string.h>
#include <errno.h>
#include "../token/token.h"

typedef enum c_Types c_Types;
char c_buffer[256];

int c_constant(char** s, double* value);
int c_types(int type);

int c_compare(char** s, char hash[][3], int keyCount);

int c_number(char** s);
int c_decimal(char** s);
int c_digit(char** s);
int c_sign(char** s);

char c_period[1][3];
char c_nonzero[9][3];
char c_zero[1][3];

enum c_Types {
    ct_number = 1,
    ct_decimal,
    ct_zero,
    /* terminator */
    ct_terminator
};

#endif