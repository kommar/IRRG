#include "Problem.h"

#include "RealCoding.h"

template <class TGenotype, class TFenotype>
CProblem<TGenotype, TFenotype>::CProblem(CEvaluation<TFenotype> *pcEvaluation, CTransformation<TGenotype, TFenotype> *pcTransformation)
{
	pc_evaluation = pcEvaluation;
	pc_transformation = pcTransformation;
}//CProblem<TGenotype, TFenotype>::CProblem(CEvaluation<TFenotype> *pcEvaluation, CTransformation<TGenotype, TFenotype> *pcTransformation)

template <class TGenotype, class TFenotype>
CProblem<TGenotype, TFenotype>::~CProblem()
{
	delete pc_evaluation;
	delete pc_transformation;
}//CProblem<TGenotype, TFenotype>::~CProblem()

template <class TGenotype, class TFenotype>
bool CProblem<TGenotype, TFenotype>::bIsBetterIndividual(CIndividual<TGenotype, TFenotype> *pcIndividual0, CIndividual<TGenotype, TFenotype> *pcIndividual1) 
{
	pcIndividual0->vEvaluate();
	pcIndividual1->vEvaluate();

	return bIsBetterFitnessValue(pcIndividual0->dGetFitnessValue(), pcIndividual1->dGetFitnessValue());
}//bool CProblem<TGenotype, TFenotype>::bIsBetterIndividual(CIndividual<TGenotype, TFenotype> *pcIndividual0, CIndividual<TGenotype, TFenotype> *pcIndividual1) 

template <class TGenotype, class TFenotype>
double CProblem<TGenotype, TFenotype>::dCalculateImprovement(CIndividual<TGenotype, TFenotype> *pcIndividual0, CIndividual<TGenotype, TFenotype> *pcIndividual1)
{
	pcIndividual0->vEvaluate();
	pcIndividual1->vEvaluate();

	return max(pcIndividual0->dGetFitnessValue() - pcIndividual1->dGetFitnessValue(), 0);
}//double CProblem<TGenotype, TFenotype>::dCalculateImprovement(CIndividual<TGenotype, TFenotype> *pcIndividual0, CIndividual<TGenotype, TFenotype> *pcIndividual1)

template class CProblem<CRealCoding, CRealCoding>;