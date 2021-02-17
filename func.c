#include "header.h"


unsigned char getHex(unsigned char ch)
{
	unsigned char hex = 0;

	if (ch >= '0' && ch <= '9')
	{
		hex = ch - '0';
	}

	else if (ch >= 'a' && ch <= 'f')
	{
		hex = ch - 'a' + 10;
	}

	else if (ch >= 'A' && ch <= 'F')
	{
		hex = ch - 'A' + 10;
	}
	return hex;
}

void convertStr2Byte(unsigned char* from, int size, unsigned char* to)
{
	int cnt_i = 0;
	int cnt_j = 0;
	int ch = 0;

	for (cnt_i = 0; cnt_i < size; cnt_i += 2, cnt_j++)
	{

		ch = from[cnt_i];
		to[cnt_j] = getHex(ch);
		to[cnt_j] <<= 4;
		ch = from[cnt_i + 1];
		to[cnt_j] |= getHex(ch);
	}

}

void ShiftRow(unsigned char* state)
{
	int temp, temp2;
	temp = state[13]; //1 bytes Leftshift
	state[13] = state[1];
	state[1] = state[5];
	state[5] = state[9];
	state[9] = temp;

	temp = state[10]; //2 bytes Leftshift
	temp2 = state[14];
	state[10] = state[2];
	state[14] = state[6];
	state[2] = temp;
	state[6] = temp2;

	temp = state[7]; // 3 bytes Leftshift
	state[7] = state[3];
	state[3] = state[15];
	state[15] = state[11];
	state[11] = temp;
}

void InvShiftRow(unsigned char* state)
{
	int temp, temp2;
	temp = state[13]; //1 bytes Rightshift
	state[13] = state[9];
	state[9] = state[5];
	state[5] = state[1];
	state[1] = temp;

	temp = state[10]; //2 bytes Rightshift
	temp2 = state[14];
	state[10] = state[2];
	state[14] = state[6];
	state[2] = temp;
	state[6] = temp2;

	temp = state[3]; //3 bytes Rightshift
	state[3] = state[7];
	state[7] = state[11];
	state[11] = state[15];
	state[15] = temp;
}
void InvSubByte(unsigned char* state)
{
	int cnt_i;
	for (cnt_i = 0; cnt_i < 16; cnt_i++)
	{
		*(state + cnt_i) = rsbox[state[cnt_i]];
	}
}

void AddRoundKey_1Round(unsigned char* state, unsigned char* key)
{
	int cnt_i;
	for (cnt_i = 0; cnt_i < AES_PLANETXT_LEN; cnt_i++)
	{
		state[cnt_i] ^= key[cnt_i];
	}

}

byte Find_HammingDistance(unsigned char before_distance, unsigned char after_distance)
{
	byte temp = before_distance ^ after_distance;
	byte count = 0;
	int cnt_i = 0;

	//printf("temp = %02X\n", temp);
	for (cnt_i = 0; cnt_i < 8; cnt_i++)
	{
		if (((temp >> cnt_i) & 0x01) == 1)
		{
			count++;
		}
	}

	//printf("%x\n", count);
	return count;
}

void Calculates_SumX(double* Sum_xx, double* Sum_Ex, double TraceTemp[TRACE_NUM][TRACE_LENGTH])
{
	/*
	 *double Sum_xx[TRACE_LENGTH] = { 0x00, };
	 *double Sum_Ex[TRACE_LENGTH] = { 0x00, };
	 *float TraceTemp[TRACE_NUM][TRACE_LENGTH] = { 0x00, };
	*/

	int cnt_i = 0, cnt_j = 0, cnt_k = 0;
	double sum_xx = 0x00;
	double sum_ex = 0x00;

	for (cnt_i = 0; cnt_i < TRACE_LENGTH; cnt_i++)
	{
		for (cnt_j = 0; cnt_j < TRACE_NUM; cnt_j++)
		{
			sum_ex += TraceTemp[cnt_j][cnt_i];
			sum_xx += (TraceTemp[cnt_j][cnt_i] * TraceTemp[cnt_j][cnt_i]);
		}
		Sum_Ex[cnt_i] = sum_ex;
		Sum_xx[cnt_i] = sum_xx;
		sum_ex = 0x00;
		sum_xx = 0x00;
	}
}


void Calculates_SumY(float Sum_yy[S_BOX][GUESSKEY], float Sum_Ey[S_BOX][GUESSKEY], byte HammingDistance[S_BOX][TRACE_NUM][GUESSKEY])
{
  /*
   *float Sum_yy[S_BOX][GUESSKEY] = { 0x00, };
   *float Sum_Ey[S_BOX][GUESSKEY] = { 0x00, };
   *byte HammingDistance[AES_PLANETXT_LEN][TRACE_NUM][GUESSKEY] = {0x00,};
   */

	int cnt_i = 0, cnt_j = 0, cnt_k = 0;
	float sum_yy = 0x00;
	float sum_ey = 0x00;

	for (cnt_i = 0; cnt_i < S_BOX; cnt_i++)
	{
		for (cnt_j = 0; cnt_j < GUESSKEY; cnt_j++)
		{
			for (cnt_k = 0; cnt_k < TRACE_NUM; cnt_k++)
			{
				sum_ey += HammingDistance[cnt_i][cnt_k][cnt_j];
				sum_yy += (HammingDistance[cnt_i][cnt_k][cnt_j] * HammingDistance[cnt_i][cnt_k][cnt_j]);
			}
			Sum_Ey[cnt_i][cnt_j] = sum_ey;
			Sum_yy[cnt_i][cnt_j] = sum_yy;
			sum_ey = 0x00;
			sum_yy = 0x00;
		}
		
	}
}

void Calculates_SumXY(double Sum_xy[S_BOX][GUESSKEY][TRACE_LENGTH], double TraceTemp[TRACE_NUM][TRACE_LENGTH], byte HammingDistance[S_BOX][TRACE_NUM][GUESSKEY])
{
	int cnt_i = 0, cnt_j = 0, cnt_k = 0, cnt_s = 0;
	double sum_xy = 0x00;

	for (cnt_i = 0; cnt_i < S_BOX; cnt_i++)
	{
		for (cnt_j = 0; cnt_j < GUESSKEY; cnt_j++)
		{
			for (cnt_k = 0; cnt_k < TRACE_LENGTH; cnt_k++)
			{
				for (cnt_s = 0; cnt_s < TRACE_NUM; cnt_s++)
				{
					sum_xy += (double)(HammingDistance[cnt_i][cnt_s][cnt_j] * TraceTemp[cnt_s][cnt_k]);
				}
				Sum_xy[cnt_i][cnt_j][cnt_k] = sum_xy;
				sum_xy = 0x00;
			}
		}

	}
}