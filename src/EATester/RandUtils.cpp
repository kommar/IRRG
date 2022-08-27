#include "RandUtils.h"

#include <cmath>
#include <cstdlib>
#include <ctime>

uint32_t RandUtils::iInit()
{
    uint32_t i_seed = (uint32_t)time(nullptr);

    vInit(i_seed);

    return i_seed;
}//uin32_t RandUtils::iInit()

void RandUtils::vInit(uint32_t iSeed)
{
    srand(iSeed);
}//void RandUtils::vInit(uint32_t iSeed)

uint32_t RandUtils::iRandIndex(uint32_t iSize)
{
	return iRandNumber((uint32_t)0, iSize - 1);
}//uint32_t RandUtils::iRandIndex(uint32_t iSize)

uint32_t RandUtils::iRandUniqueIndex(uint32_t iSize, unordered_set<uint32_t>* psSelected)
{
	return iRandUniqueNumber(0, iSize - 1, psSelected);
}//uint32_t RandUtils::iRandUniqueIndex(uint32_t iSize, unordered_set<uint32_t>* psSelected)

uint32_t RandUtils::iRandNumber(uint32_t iMinValue, uint32_t iMaxValue)
{
    return iMinValue + ((uint32_t)rand() % (iMaxValue - iMinValue + 1));
}//uint32_t RandUtils::iRandNumber(uint32_t iMinValue, uint32_t iMaxValue)

uint32_t RandUtils::iRandUniqueNumber(uint32_t iMinValue, uint32_t iMaxValue, unordered_set<uint32_t>* psSelected)
{
	uint32_t i_rand_number;

	do//while (psSelected->count(i_rand_number) > 0)
	{
		i_rand_number = iRandNumber(iMinValue, iMaxValue);
	} while (psSelected->count(i_rand_number) > 0);

	return i_rand_number;
}//uint32_t RandUtils::iRandUniqueNumber(uint32_t iMinValue, uint32_t iMaxValue, unordered_set<uint32_t>* psSelected)

int32_t RandUtils::iRandNumber(int32_t iMinValue, int32_t iMaxValue)
{
    return iMinValue + ((int32_t)rand() % (iMaxValue - iMinValue + 1));
}//int32_t RandUtils::iRandNumber(int32_t iMinValue, int32_t iMaxValue)

double RandUtils::dRandNumber(double dMaxValue)
{
	return dRandNumber(0, dMaxValue);
}//double RandUtils::dRandNumber(double dMaxValue)

double RandUtils::dRandNumber(double dMinValue, double dMaxValue)
{
	double d_ratio = (double)rand() / RAND_MAX;

    return dMinValue + d_ratio * (dMaxValue - dMinValue);
}//double RandUtils::dRandNumber(double dMinValue, double dMaxValue)

double RandUtils::dRandProbability()
{
	return dRandNumber(0, 1);
}//double RandUtils::dRandProbability()

double RandUtils::dRandGaussianNumber()
{
	return dRandGaussianNumber(0, 1);
}//double RandUtils::dRandGaussianNumber()

#include <iostream>
using namespace std;

double RandUtils::dRandGaussianNumber(double dMu, double dSigma)
{
	double d_u1 = dRandProbability();
	double d_u2 = dRandProbability();

	if (d_u1 == 0)
	{
		d_u1 = DBL_MIN;
	}//if (d_u1 == 0)

	if (d_u2 == 0)
	{
		d_u2 = DBL_MIN;
	}//if (d_u2 == 0)

	if (d_u1 == 0.0)
	{
		cout << "lipa " << endl;
	}

	double d_z = sqrt(-2.0 * log(d_u1)) * cos(DEF_TWO_PI * d_u2);

	if (isnan(d_z) || isinf(d_z) || isnan(dSigma) || isinf(dSigma) || isnan(dMu) || isinf(dMu))
	{
		cout << "komarenko" << endl;
		cout << d_z * dSigma + dMu << endl;
	}

	return d_z * dSigma + dMu;
}//double RandUtils::dRandGaussianNumber(double dMu, double dSigma)

float RandUtils::fRandNumber(float fMinValue, float fMaxValue)
{
    float f_ratio = (float)rand() / RAND_MAX;

    return fMinValue + f_ratio * (fMaxValue - fMinValue);
}//float RandUtils::fRandNumber(float fMinValue, float fMaxValue)

float RandUtils::fRandProbability()
{
    return fRandNumber(0, 1);
}//float RandUtils::fRandProbability()

bool RandUtils::bSatisfyProbability(float fProbability)
{
	return fRandProbability() <= fProbability;
}//bool RandUtils::bSatisfyProbability(float fProbability)