#include "header.h"

FILE* FILE_Trace = NULL;
FILE* FILE_PT = NULL;
FILE* CPA_peak = NULL;

byte bytes_value = 0;
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

double co_co[TRACE_LENGTH];
double MAX_peak[GUESSKEY];
double temp;


int main()
{
	int cnt_i = 0, cnt_j = 0, cnt_k = 0, cnt_s = 0;
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
				bytes_value = ciphertext[cnt_k][cnt_i];
				bytes_value = rsbox[bytes_value ^ guess_key[cnt_i]];
				byte after_distance = bytes_value;
				HammingDistance[cnt_i][cnt_k][cnt_j] = Find_HammingDistance(before_distance, after_distance);
				//printf("%d ", HammingDistance[cnt_i][cnt_k][cnt_j]);
			}
		}
	}
	
	//[Caluates SumEx, xx, Ey, yy]*******************************************************************************************************************************
	Calculates_SumX(Sum_xx, Sum_Ex, TraceTemp);
	Calculates_SumY(Sum_yy, Sum_Ey, HammingDistance);
	Calculates_SumXY(Sum_xy, TraceTemp, HammingDistance);

	//for (cnt_i = 0; cnt_i < TRACE_LENGTH; cnt_i++)
	//{
	//	printf("%f\n", Sum_xx[cnt_i]);
	//
	//}

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

	for (cnt_i = 0; cnt_i < S_BOX; cnt_i++)
	{
		//printf("\n*******[S_BOX %02X]*******\n", cnt_i);
		for (cnt_j = 0; cnt_j < GUESSKEY; cnt_j++)
		{
			//printf("\n*******[GUESSKEY %02X]*******\n", cnt_j);
			for (cnt_k = 0; cnt_k < TRACE_LENGTH; cnt_k++)
			{
				co_co[cnt_k] = ((TRACE_NUM)*Sum_xy[cnt_i][cnt_j][cnt_k] - (Sum_Ex[cnt_k]*Sum_Ey[cnt_i][cnt_j]))/ ((sqrt((TRACE_NUM) * Sum_xx[cnt_k]-(Sum_Ex[cnt_k] * Sum_Ex[cnt_k]))) * (sqrt((double)(TRACE_NUM)*Sum_yy[cnt_i][cnt_j] - ((double)Sum_Ey[cnt_i][cnt_j] * (double)Sum_Ey[cnt_i][cnt_j])))) ;
			}
			temp = co_co[0];
			for (cnt_s = 1; cnt_s < TRACE_LENGTH; cnt_s++)
			{
				if (temp < co_co[cnt_s])
				{
					temp = co_co[cnt_s];
				}
			}
			MAX_peak[cnt_j] = temp;
			//printf("MAXpeak : %lf\n", MAX_peak[cnt_j]);
		}

		temp = MAX_peak[0];
		for (cnt_s = 0; cnt_s < GUESSKEY; cnt_s++)
		{
			if (temp <= MAX_peak[cnt_s])
			{
				temp = MAX_peak[cnt_s];
				guess_key[cnt_i] = cnt_s;
				//printf("%02X\n", guess_key[cnt_j]);
			}
		}

		for (cnt_j = 0; cnt_j < TRACE_LENGTH; cnt_j++)
		{
			co_co[cnt_j] = ((TRACE_NUM)*Sum_xy[cnt_i][guess_key[cnt_i]][cnt_j] - (Sum_Ex[cnt_j] * Sum_Ey[cnt_i][guess_key[cnt_i]])) / ((sqrt((TRACE_NUM)*Sum_xx[cnt_j] - (Sum_Ex[cnt_j] * Sum_Ex[cnt_j]))) * (sqrt((double)(TRACE_NUM)*Sum_yy[cnt_i][guess_key[cnt_i]] - ((double)Sum_Ey[cnt_i][guess_key[cnt_i]] * (double)Sum_Ey[cnt_i][guess_key[cnt_i]]))));
		}

		sprintf(str, "C:\\Users\\YoungBeom Kim\\source\\repos\\AESHW_CPA210215\\CPA_Peak\\!CPA_%d_peak_%x.txt", cnt_i, guess_key[cnt_i]);
		CPA_peak = fopen(str, "w");

		for (cnt_j = 0; cnt_j < TRACE_LENGTH; cnt_j++)
		{
			fprintf(CPA_peak, "%lf\n", co_co[cnt_j]);
		}
		fclose(CPA_peak);
	}

	return 0;
}