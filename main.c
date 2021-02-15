#include "header.h"

FILE* FILE_Trace = NULL;
FILE* FILE_PT = NULL;

byte ciphertext[TRACE_NUM][AES_PLANETXT_LEN] = { 0x00, };
byte buffer[64] = { 0, };
byte HammingDistance[AES_PLANETXT_LEN][TRACE_NUM][GUESSKEY];
byte guess_key[16] = { 0x00, };
char str[256] = { 0, };
double Corr[AES_PLANETXT_LEN][TRACE_NUM][GUESSKEY] = { 0x00, };
double TraceTemp[TRACE_NUM][TRACE_LENGTH] = { 0x00, };
double sum_xy[GUESSKEY][TRACE_LENGTH] = { 0x00 };
double Sum_xx[TRACE_LENGTH] = { 0x00, };
double Sum_Ex[TRACE_LENGTH] = { 0x00, };
float Sum_yy[S_BOX][TRACE_LENGTH] = { 0x00, };
float Sum_Ey[S_BOX][TRACE_LENGTH] = { 0x00, };



int main()
{
	int cnt_i = 0, cnt_j = 0, cnt_k = 0;
	FILE_PT = fopen("C:\\Users\\YoungBeom Kim\\source\\repos\\AES_HW_trace\\ciphertext.txt", "r");
	assert(FILE_PT != NULL);

	//[Read CipherText]**************************************************************************************************************************
	for (cnt_i = 0; cnt_i < TRACE_NUM; cnt_i++)
	{
		fgets(buffer, sizeof(buffer), FILE_PT); //! 한줄씩만 읽는함수
		convertStr2Byte(buffer, 32, ciphertext[cnt_i]);

		/*for (int j = 0; j < AES_PLANETXT_LEN; j++)
		{
			printf("%02X ", ciphertext[i][j]);
		}
		printf("\n");*/

	}
	fclose(FILE_PT);


	//[Read Trace]*******************************************************************************************************************************
	for (cnt_i = 0; cnt_i < TRACE_NUM; cnt_i++)
	{
		sprintf(str, "C:\\Users\\YoungBeom Kim\\source\\repos\\AES_HW_trace\\trace_%0.5d.txt", cnt_i);
		FILE_Trace = fopen(str, "rb");
		assert(FILE_Trace != NULL);
		for (cnt_j = 0; cnt_j < TRACE_LENGTH; cnt_j++)
		{
			fscanf(FILE_Trace, "%lf", &TraceTemp[cnt_i][cnt_j]);
			//printf("%lf\n", TraceTemp[cnt_i][cnt_j]);
		}
		//printf("\n");
		//return 0;
		fclose(FILE_Trace);
	}

	for (cnt_i = 0; cnt_i < S_BOX; cnt_i++)
	{
		for (cnt_j = 0; cnt_j < GUESSKEY; cnt_j++)
		{
			guess_key[cnt_i] = cnt_j;

			for (cnt_k = 0; cnt_k < TRACE_NUM; cnt_k++)
			{
				byte before_distance = ciphertext[cnt_k][cnt_j];
				AddRoundKey_1Round(ciphertext[cnt_k], guess_key);
				InvSubByte(ciphertext[cnt_k]);
				byte after_distance = ciphertext[cnt_k][cnt_j];
				HammingDistance[cnt_i][cnt_k][cnt_j] = Find_HammingDistance(before_distance, after_distance);
				
			}
		}
	}


	return 0;
}