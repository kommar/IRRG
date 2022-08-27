#ifndef PROBLEM_H
#define PROBLEM_H

#define PROBLEM_ARGUMENT_TYPE "problem_type"
#define PROBLEM_ARGUMENT_TYPE_REAL_REAL "real_real"

#include "Error.h"
#include "Evaluation.h"
#include "Individual.h"
#include "Transformation.h"

#include <iostream>

using namespace std;


enum EProblemType
{
	PROBLEM_REAL_REAL = 1
};//enum EProblemType


template <class TGenotype, class TFenotype>
class CProblem
{
public:
	CProblem(CEvaluation<TFenotype> *pcEvaluation, CTransformation<TGenotype, TFenotype> *pcTransformation);
	
	virtual ~CProblem();

	bool bIsBetterFitnessValue(double dFitnessValue0, double dFitnessValue1) { return dFitnessValue0 > dFitnessValue1; };
	bool bIsBetterIndividual(CIndividual<TGenotype, TFenotype> *pcIndividual0, CIndividual<TGenotype, TFenotype> *pcIndividual1);

	CIndividual<TGenotype, TFenotype> *pcGetBetterIndividual(CIndividual<TGenotype, TFenotype> *pcIndividual0, 
		CIndividual<TGenotype, TFenotype> *pcIndividual1)
	{
		return bIsBetterIndividual(pcIndividual0, pcIndividual1) ? pcIndividual0 : pcIndividual1;
	}

	bool bIsWorseFitnessValue(double dFitnessValue0, double dFitnessValue1) { return dFitnessValue0 < dFitnessValue1; };

	double dCalculateImprovement(CIndividual<TGenotype, TFenotype> *pcIndividual0, CIndividual<TGenotype, TFenotype> *pcIndividual1);

	CEvaluation<TFenotype> *pcGetEvaluation() { return pc_evaluation; };
	CTransformation<TGenotype, TFenotype> *pcGetTransformation() { return pc_transformation; };

	virtual CString  sAdditionalSummaryInfo() { return(""); }

private:
	CEvaluation<TFenotype> *pc_evaluation;
	CTransformation<TGenotype, TFenotype> *pc_transformation;
};//class CProblem

#endif//PROBLEM_H