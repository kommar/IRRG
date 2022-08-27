#include "MTSLS1.h"

#include "GenerationUtils.h"

#include <algorithm>
#include <numeric>

CMTSLS1::CMTSLS1(CProblem<CRealCoding, CRealCoding> *pcProblem, CLog *pcLog, uint32_t iRandomSeed)
	: COptimizer<CRealCoding, CRealCoding>(pcProblem, pcLog, iRandomSeed)
{

}//CMTSLS1::CMTSLS1(CProblem<CRealCoding, CRealCoding> *pcProblem, CLog *pcLog, uint32_t iRandomSeed)

CMTSLS1::CMTSLS1(CMTSLS1 *pcOther)
	: COptimizer<CRealCoding, CRealCoding>(pcOther)
{
	d_sr_initial_range = pcOther->d_sr_initial_range;
}//CMTSLS1::CMTSLS1(CMTSLS1 *pcOther)

CError CMTSLS1::eConfigure(istream *psSettings)
{
	CError c_error = COptimizer<CRealCoding, CRealCoding>::eConfigure(psSettings);

	d_sr_initial_range = 0.2;
	pc_generation = GenerationUtils::pcGetGeneration(pc_problem, psSettings, &c_error);

	return c_error;
}//CError CMTSLS1::eConfigure(istream *psSettings)

void CMTSLS1::vInitialize(time_t tStartTime)
{
	uint16_t i_number_of_elements = pc_problem->pcGetEvaluation()->iGetNumberOfElements();
	CRealCoding *pc_genotype = pc_generation->pcGenerate();

	v_srs.resize(i_number_of_elements);
	v_improvements.resize(i_number_of_elements);

	for (uint16_t i = 0; i < i_number_of_elements; i++)
	{
		v_srs[i] = (pc_genotype->pdGetMaxValues()[i] - pc_genotype->pdGetMinValues()[i]) * d_sr_initial_range;
		v_improvements[i] = 0;
	}//for (uint16_t i = 0; i < i_number_of_elements; i++)

	CIndividual<CRealCoding, CRealCoding> *pc_individual = new CIndividual<CRealCoding, CRealCoding>(pc_genotype, pc_problem);
	pc_individual->vEvaluate();

	b_update_best_individual(0, tStartTime, pc_individual);
	delete pc_individual;

	v_dimensions.resize(i_number_of_elements);

	iota(v_dimensions.begin(), v_dimensions.end(), 0);
	next_permutation(v_dimensions.begin(), v_dimensions.end());

	uint16_t i_dimension;
	double d_improvement;

	for (uint16_t i = 0; i < (uint16_t)v_dimensions.size(); i++)
	{
		i_dimension = v_dimensions[i];

		pc_individual = pc_run_one_dimension(i_dimension);
		d_improvement = pc_problem->dCalculateImprovement(pc_individual, pc_best_individual);
		v_improvements[i_dimension] = d_improvement;

		if (d_improvement > 0)
		{
			b_update_best_individual(0, tStartTime, pc_individual);
		}//if (d_improvement > 0)
		else
		{
			v_srs[i_dimension] /= 2.0;
		}//else if (d_improvement > 0)

		delete pc_individual;
	}//for (uint16_t i = 0; i < (uint16_t)v_dimensions.size(); i++)

	v_sort_dimensions();

	i_dimensions_index = 0;
}//void CMTSLS1::vInitialize(time_t tStartTime)

bool CMTSLS1::bRunIteration(uint32_t iIterationNumber, time_t tStartTime)
{
	bool b_updated = false;

	uint16_t i_dimension = v_dimensions[i_dimensions_index];
	uint16_t i_next_dimensions_index = (i_dimensions_index + 1) % pc_problem->pcGetEvaluation()->iGetNumberOfElements();
	uint16_t i_next_dimension = v_dimensions[i_next_dimensions_index];

	CIndividual<CRealCoding, CRealCoding> *pc_individual = pc_run_one_dimension(i_dimension);
	double d_improvement = pc_problem->dCalculateImprovement(pc_individual, pc_best_individual);

	v_improvements[i_dimension] = d_improvement;

	if (d_improvement > 0)
	{
		b_updated = b_update_best_individual(iIterationNumber, tStartTime, pc_individual);

		if (v_improvements[i_dimension] < v_improvements[i_next_dimensions_index])
		{
			v_sort_dimensions();
		}//if (v_improvements[i_dimension] < v_improvements[i_next_dimensions_index])
	}//if (d_improvement > 0)
	else
	{
		v_srs[i_dimension] /= 2.0;
		i_dimensions_index = i_next_dimensions_index;

		if (v_srs[i_dimension] < 1e-15)
		{
			CRealCoding *pc_genotype = pc_individual->pcGetGenotype();

			v_srs[i_dimension] = (pc_genotype->pdGetMaxValues()[i_dimension] - pc_genotype->pdGetMinValues()[i_dimension]) * d_sr_initial_range;
		}//if (v_srs[i_dimension] < 1e-15)
	}//else if (d_improvement > 0)

	delete pc_individual;

	CString s_log_message;

	s_log_message.Format
	(
		"iteration: %u; time: %u; ffe: %u; best: %f",
		iIterationNumber,
		(uint32_t)(time(nullptr) - tStartTime),
		pc_problem->pcGetEvaluation()->iGetFFE(),
		pc_best_individual->dGetFitnessValue()
	);//s_log_message.Format

	pc_log->vPrintLine(s_log_message, true);

	return b_updated;
}//bool CMTSLS1::bRunIteration(uint32_t iIterationNumber, time_t tStartTime)

CIndividual<CRealCoding, CRealCoding> * CMTSLS1::pc_run_one_dimension(uint16_t iDimension)
{
	CRealCoding *pc_genotype = new CRealCoding(pc_best_individual->pcGetGenotype());
	CIndividual<CRealCoding, CRealCoding> *pc_individual = new CIndividual<CRealCoding, CRealCoding>(pc_genotype, pc_problem);

	pc_genotype->pdGetValues()[iDimension] -= v_srs[iDimension];
	pc_genotype->vRepair();

	pc_individual->vEvaluate();

	if (!pc_problem->bIsBetterIndividual(pc_individual, pc_best_individual))
	{
		pc_genotype->pdGetValues()[iDimension] += 1.5 * v_srs[iDimension];
		pc_genotype->vRepair();

		pc_individual->vIsEvaluated(false);
		pc_individual->vEvaluate();
	}//if (!pc_problem->bIsBetterIndividual(pc_individual, pc_best_individual))

	return pc_individual;
}//CIndividual<CRealCoding, CRealCoding> * CMTSLS1::pc_run_one_dimension(uint16_t iDimension)

void CMTSLS1::v_sort_dimensions()
{
	sort(v_dimensions.begin(), v_dimensions.end(), [&](uint16_t iDimension0, uint16_t iDimension1)
	{
		return v_improvements[iDimension0] > v_improvements[iDimension1];
	});//sort(v_dimensions.begin(), v_dimensions.end(), [&](uint16_t iDimension0, uint16_t iDimension1)
}//void CMTSLS1::v_sort_dimensions()