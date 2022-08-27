#include "Individual.h"

#include "Problem.h"
#include "RealCoding.h"

template <class TGenotype, class TFenotype>
CIndividual<TGenotype, TFenotype>::CIndividual(TGenotype *pcGenotype, CEvaluation<TFenotype> *pcEvaluation, CTransformation<TGenotype, TFenotype> *pcTransformation)
{
	b_is_evaluated = false;

	pc_genotype = pcGenotype;
	pc_fenotype = nullptr;

	pc_evaluation = pcEvaluation;

	pc_transformation = pcTransformation;
}//CIndividual<TGenotype, TFenotype>::CIndividual(TGenotype *pcGenotype, CEvaluation<TFenotype> *pcEvaluation, CTransformation<TGenotype, TFenotype> *pcTransformation)

template <class TGenotype, class TFenotype>
CIndividual<TGenotype, TFenotype>::CIndividual(TGenotype *pcGenotype, CProblem<TGenotype, TFenotype> *pcProblem)
	: CIndividual<TGenotype, TFenotype>(pcGenotype, pcProblem->pcGetEvaluation(), pcProblem->pcGetTransformation())
{

}//template <class TGenotype, class TFenotype>

template <class TGenotype, class TFenotype>
CIndividual<TGenotype, TFenotype>::CIndividual(CIndividual<TGenotype, TFenotype> *pcOther)
{
	b_is_evaluated = pcOther->b_is_evaluated;
	d_fitness_value = pcOther->d_fitness_value;

	pc_genotype = new TGenotype(pcOther->pc_genotype);
	pc_fenotype = pcOther->pc_transformation->pcTransform(pc_genotype);

	pc_evaluation = pcOther->pc_evaluation;

	pc_transformation = pcOther->pc_transformation;
}//CIndividual<TGenotype, TFenotype>::CIndividual(CIndividual<TGenotype, TFenotype> *pcOther)

template <class TGenotype, class TFenotype>
CIndividual<TGenotype, TFenotype>::~CIndividual()
{
	if (pc_genotype != pc_fenotype)
	{
		delete pc_fenotype;
	}//if (pc_genotype != pc_fenotype)

	delete pc_genotype;
}//CIndividual<TGenotype, TFenotype>::~CIndividual()

template <class TGenotype, class TFenotype>
void CIndividual<TGenotype, TFenotype>::vEvaluate()
{
	if (!b_is_evaluated)
	{
		v_transform_genotype();

		d_fitness_value = pc_evaluation->dEvaluate(pc_fenotype);
		b_is_evaluated = true;
	}//if (!b_is_evaluated)
}//void CIndividual<TGenotype, TFenotype>::vEvaluate()

template <class TGenotype, class TFenotype>
void CIndividual<TGenotype, TFenotype>::vSetFitnessValue(double dFitnessValue, bool bIsEvaluated)
{
	d_fitness_value = dFitnessValue;
	b_is_evaluated = bIsEvaluated;

	if (bIsEvaluated)
	{
		v_transform_genotype();
	}//if (bIsEvaluated)
}//void CIndividual<TGenotype, TFenotype>::vSetFitnessValue(double dFitnessValue, bool bIsEvaluated)

template <class TGenotype, class TFenotype>
void CIndividual<TGenotype, TFenotype>::v_transform_genotype()
{
	if (!pc_fenotype)
	{
		pc_fenotype = pc_transformation->pcTransform(pc_genotype);
	}//if (!pc_fenotype)
	else
	{
		pc_transformation->vTransform(pc_genotype, pc_fenotype);
	}//else if (!pc_fenotype)
}//void CIndividual<TGenotype, TFenotype>::v_transform_genotype()

template class CIndividual<CRealCoding, CRealCoding>;