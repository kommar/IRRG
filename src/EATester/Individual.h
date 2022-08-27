#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include "Evaluation.h"
#include "Generation.h"
#include "Transformation.h"

#include <cstdint>

using namespace std;

template <class TGenotype, class TFenotype> class CProblem;

template <class TGenotype, class TFenotype>
class CIndividual
{
public:
	CIndividual(TGenotype *pcGenotype, CEvaluation<TFenotype> *pcEvaluation, CTransformation<TGenotype, TFenotype> *pcTransformation);
	CIndividual(TGenotype *pcGenotype, CProblem<TGenotype, TFenotype> *pcProblem);
	CIndividual(CIndividual<TGenotype, TFenotype> *pcOther);
	
	virtual ~CIndividual();

	virtual void vEvaluate();

	TGenotype *pcGetGenotype() { return pc_genotype; };
	TFenotype *pcGetFenotype() { return pc_fenotype; };

	double dGetFitnessValue() { return d_fitness_value; };
	void vSetFitnessValue(double dFitnessValue, bool bIsEvaluated = true);

	bool bIsEvaluated() { return b_is_evaluated; };
	void vIsEvaluated(bool bIsEvaluated) { b_is_evaluated = bIsEvaluated; };

protected:
	double d_fitness_value;

private:
	void v_transform_genotype();

	bool b_is_evaluated;

	TGenotype *pc_genotype;
	TFenotype *pc_fenotype;

	CEvaluation<TFenotype> *pc_evaluation;

	CTransformation<TGenotype, TFenotype> *pc_transformation;
};//class CIndividual

#endif//INDIVIDUAL_H