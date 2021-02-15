#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <memory.h>
#include <math.h>

#define TRACE_NUM 20000
#define TRACE_LENGTH 3253
#define AES_PLANETXT_LEN 16
#define S_BOX 16
#define GUESSKEY 256


typedef unsigned int word;
typedef unsigned char byte;


unsigned char getHex(unsigned char ch);
void convertStr2Byte(unsigned char* from, int size, unsigned char* to);

void InvShiftRow(unsigned char* state);
void InvSubByte(unsigned char* state);
void AddRoundKey_1Round(unsigned char* state, unsigned char* key);
void Decrypt_AES_10Round(unsigned char* state, unsigned char* key);

