#include "header.h"

FILE* FILE_Trace = NULL;
FILE* FILE_PT = NULL;

byte temp = 0;
byte ciphertext[TRACE_NUM][AES_PLANETXT_LEN] = { 0x00, };
byte buffer[64] = { 0, };
byte HammingDistance[AES_PLANETXT_LEN][TRACE_NUM][GUESSKEY];
byte guess_key[16] = { 0x00, };
char str[256] = { 0, };

float Corr[S_BOX][TRACE_LENGTH][GUESSKEY] = { 0x00, };
double TraceTemp[TRACE_NUM][TRACE_LENGTH] = { 0x00, };
double Sum_xy[S_BOX][GUESSKEY][TRACE_LENGTH] = { 0x00, };
double Sum_xx[TRACE_LENGTH] = { 0x00, };
double Sum_Ex[TRACE_LENGTH] = { 0x00, };
float Sum_yy[S_BOX][GUESSKEY] = { 0x00, };
float Sum_Ey[S_BOX][GUESSKEY] = { 0x00, };


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

		/*for (int cnt_j = 0; cnt_j < AES_PLANETXT_LEN; cnt_j++)
		{
			printf("%02X ", ciphertext[cnt_i][cnt_j]);
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



	//[Caluates HammingDistance]*******************************************************************************************************************************
	for (cnt_i = 0; cnt_i < S_BOX; cnt_i++)
	{
		//printf("\n*******[S_BOX %02X]*******\n", cnt_i);
		for (cnt_j = 0; cnt_j < GUESSKEY; cnt_j++)
		{
			guess_key[cnt_i] = cnt_j;
			//printf("\n\n*******[GUESS KEY %02X]*******\n", cnt_j);
			for (cnt_k = 0; cnt_k < TRACE_NUM; cnt_k++)
			{
				byte before_distance = ciphertext[cnt_k][cnt_i];
				temp = ciphertext[cnt_k][cnt_i];
				temp = rsbox[temp ^ guess_key[cnt_i]];
				byte after_distance = temp;
				HammingDistance[cnt_i][cnt_k][cnt_j] = Find_HammingDistance(before_distance, after_distance);
				//printf("%d ", HammingDistance[cnt_i][cnt_k][cnt_j]);
			}
		}
	}
	
	//[Caluates SumEx, xx, Ey, yy]*******************************************************************************************************************************
	Calculates_SumX(Sum_xx, Sum_Ex, TraceTemp);
	Calculates_SumY(Sum_yy, Sum_Ey, HammingDistance);
	Calculates_SumXY(Sum_xy, TraceTemp, HammingDistance);

	//for (cnt_i = 0; cnt_i < S_BOX; cnt_i++)
	//{
	//	printf("*****[SBOX %d]****\n", cnt_i);
	//	for (cnt_j = 0; cnt_j < GUESSKEY; cnt_j++)
	//	{
	//		printf("%f\n", Sum_yy[cnt_i][cnt_j]);
	//	}
	//}

	//for (cnt_i = 0; cnt_i < S_BOX; cnt_i++)
	//{
	//	printf("*****[SBOX %d]****\n", cnt_i);
	//	for (cnt_j = 0; cnt_j < GUESSKEY; cnt_j++)
	//	{
	//		printf("%f\n", Sum_Ey[cnt_i][cnt_j]);
	//	}
	//}

	/*for (cnt_i = 0; cnt_i < S_BOX; cnt_i++)
	{
		printf("*****[SBOX %d]****\n", cnt_i);
		for (cnt_j = 0; cnt_j < GUESSKEY; cnt_j++)
		{
			printf("*****[GUESSKEY %d]****\n", cnt_j);
			for (cnt_k = 0; cnt_k < TRACE_LENGTH; cnt_k++)
			{
				printf("%lf\n", Sum_xy[cnt_i][cnt_j][cnt_k]);
			}
		}
	}*/


	//[Caluates Correlation coefficient]*******************************************************************************************************************************
	return 0;
}