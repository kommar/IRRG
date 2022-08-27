#include "Decomposition.h"

#include "CommandParam.h"
#include "GenerationUtils.h"
#include "Individual.h"
#include "MathUtils.h"
#include "OptimizerUtils.h"
#include "PointerUtils.h"
#include "RandUtils.h"
#include "UIntCommandParam.h"
#include "VectorUtils.h"

#include "EvaluationUtils.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <numeric>

using namespace Decomposition;


template <class TGenotype, class TFenotype>
CDecomposition<TGenotype, TFenotype>::CDecomposition(CProblem<TGenotype, TFenotype> *pcProblem, CLog *pcLog, uint32_t iRandomSeed)
{
	pc_problem = pcProblem;
	pc_log = pcLog;
	i_random_seed = iRandomSeed;
}//CDecomposition<TGenotype, TFenotype>::CDecomposition(CProblem<TGenotype, TFenotype> *pcProblem, CLog *pcLog, uint32_t iRandomSeed)

template <class TGenotype, class TFenotype>
CDecomposition<TGenotype, TFenotype>::~CDecomposition()
{
	VectorUtils::vDeleteElementsAndClear(&v_groups);
}//CDecomposition<TGenotype, TFenotype>::~CDecomposition()

template <class TGenotype, class TFenotype>
CError CDecomposition<TGenotype, TFenotype>::eConfigure(istream *psSettings)
{
	return CError();
}//CError CDecomposition<TGenotype, TFenotype>::eConfigure(istream *psSettings)


CRealIncrementalRecursiveRankingGroupingDecomposition::CRealIncrementalRecursiveRankingGroupingDecomposition(CProblem<CRealCoding, CRealCoding> *pcProblem, CLog *pcLog, uint32_t iRandomSeed)
	: CDecomposition<CRealCoding, CRealCoding>(pcProblem, pcLog, iRandomSeed)
{
	pd_steps = new double[pcProblem->pcGetEvaluation()->iGetNumberOfElements()];
}//CRealIncrementalRecursiveRankingGroupingDecomposition::CRealIncrementalRecursiveRankingGroupingDecomposition(CProblem<CRealCoding, CRealCoding> *pcProblem, CLog *pcLog, uint32_t iRandomSeed)

CRealIncrementalRecursiveRankingGroupingDecomposition::~CRealIncrementalRecursiveRankingGroupingDecomposition()
{
	delete pd_steps;
}//CRealIncrementalRecursiveRankingGroupingDecomposition::~CRealIncrementalRecursiveRankingGroupingDecomposition()

CError CRealIncrementalRecursiveRankingGroupingDecomposition::eConfigure(istream *psSettings)
{
	CError c_error = CDecomposition<CRealCoding, CRealCoding>::eConfigure(psSettings);

	if (!c_error)
	{
		pc_global_optimizer = OptimizerUtils::pcGetOptimizer(pc_problem, pc_log, i_random_seed, psSettings, &c_error);
	}//if (!c_error)

	if (!c_error)
	{
		pc_local_optimizer = OptimizerUtils::pcGetOptimizer(pc_problem, pc_log, i_random_seed, psSettings, &c_error);
	}//if (!c_error)

	if (!c_error)
	{
		pc_generation = GenerationUtils::pcGetGeneration(pc_problem, psSettings, &c_error);
	}//if (!c_error)

	if (!c_error)
	{
		CUIntCommandParam p_number_of_samples("number_of_samples");
		i_number_of_samples = p_number_of_samples.iGetValue(psSettings, &c_error);
	}//if (!c_error)

	if (!c_error)
	{
		CUIntCommandParam p_run_optimizer_frequency("max_number_consecutive_of_zero_update_counters", (uint32_t)1, (uint32_t)UINT8_MAX);
		i_max_number_consecutive_of_zero_update_counters = (uint8_t)p_run_optimizer_frequency.iGetValue(psSettings, &c_error);
	}//if (!c_error)

	return c_error;
}//CError CRealIncrementalRecursiveRankingGroupingDecomposition::eConfigure(istream *psSettings)

void CRealIncrementalRecursiveRankingGroupingDecomposition::vRun()
{
	CString s_log;

	pc_global_optimizer->vRun();

	pc_local_optimizer->vSetBestIndividual(pc_global_optimizer->pcGetBestIndividual());
	pc_local_optimizer->vRun();

	CRealCoding *pc_best = pc_local_optimizer->pcGetBestIndividual()->pcGetFenotype();

	uint16_t i_number_of_elements = pc_problem->pcGetEvaluation()->iGetNumberOfElements();

	double **ppd_theta = ppd_create_theta();

	uint64_t i_ffe_before = pc_problem->pcGetEvaluation()->iGetFFE();

	i_previous_one_element_patterns_size = 0;
	b_previous_one_element_patterns_are_dependent = false;

	bool b_steady = false;

	i_number_consecutive_of_zero_update_counters = 0;

	uint32_t i_iteration_number = 1;

	while (!b_steady)
	{
		VectorUtils::vDeleteElementsAndClear(&v_groups);

		CRealCoding *pc_worst = pc_generation->pcGenerate();

		uint64_t i_ffe_before = pc_problem->pcGetEvaluation()->iGetFFE();

		v_find_groups(pc_worst, pc_best, ppd_theta, i_iteration_number);

		uint64_t i_ffe_after = pc_problem->pcGetEvaluation()->iGetFFE();
		uint64_t i_ffe = i_ffe_after - i_ffe_before;

		uint32_t i_update_counter = i_update_theta(ppd_theta);
		
		if (i_update_counter == 0)
		{
			i_number_consecutive_of_zero_update_counters++;
		}//if (i_update_counter == 0)
		else
		{
			i_number_consecutive_of_zero_update_counters = 0;
		}//else if (i_update_counter == 0)

		s_log.Format("iteration: %d; update counter: %d; ffe: %u; steady state iterations: %d", i_iteration_number, i_update_counter, i_ffe, i_number_consecutive_of_zero_update_counters);
		pc_log->vPrintLine(s_log, true);

		if (i_iteration_number == 1)
		{
			b_steady = i_update_counter == 0;
		}//if (iIterationNumber == 1)
		else
		{
			b_steady = i_number_consecutive_of_zero_update_counters >= i_max_number_consecutive_of_zero_update_counters;
		}//else if (iIterationNumber == 1)

		delete pc_worst;

		i_iteration_number++;
	}//while (!b_steady)

	vLogTheta(ppd_theta, i_number_of_elements, pc_log);

	VectorUtils::vDeleteElementsAndClear(&v_groups);

	vector<bool> v_visited(i_number_of_elements, false);

	vector<CGenePattern*> v_one_element_groups;
	v_one_element_groups.reserve(v_groups.size());

	CGenePattern *pc_pattern;

	for (uint16_t i = 0; i < i_number_of_elements; i++)
	{
		if (!v_visited[i])
		{
			pc_pattern = new CGenePattern();

			pc_pattern->vAdd(i);

			for (uint16_t j = i + 1; j < i_number_of_elements; j++)
			{
				if (ppd_theta[i][j] == 1)
				{
					pc_pattern->vAdd(j);
					v_visited[j] = true;
				}//if (ppd_theta[i][j] == 1)
			}//for (uint16_t j = i + 1; j < i_number_of_elements; j++)

			if (pc_pattern->iGetSize() == 1)
			{
				v_one_element_groups.push_back(pc_pattern);
			}//if (pc_pattern->iGetSize() == 1)
			else
			{
				v_groups.push_back(pc_pattern);
			}//else if (pc_pattern->iGetSize() == 1)
		}//if (!v_visited[i])
	}//for (uint16_t i = 0; i < i_number_of_elements; i++)

	s_log.Format("number of separable variables: %d", v_one_element_groups.size());
	pc_log->vPrintLine(s_log, true);

	s_log.Format("number of longer groups: %d", v_groups.size());
	pc_log->vPrintLine(s_log, true);

	s_log = CString("longer groups:");

	for (size_t i = 0; i < v_groups.size(); i++)
	{
		s_log.AppendFormat(" %d", v_groups[i]->iGetSize());
	}//for (size_t i = 0; i < v_groups.size(); i++)
	
	pc_log->vPrintLine(s_log, true);

	v_groups.insert(v_groups.begin(), v_one_element_groups.begin(), v_one_element_groups.end());

	s_log.Format("number of all groups: %d", v_groups.size());
	pc_log->vPrintLine(s_log, true);

	s_additional_summary.Format("%d", i_iteration_number - 1);
}//void CRealIncrementalRecursiveRankingGroupingDecomposition::vRun()

void CRealIncrementalRecursiveRankingGroupingDecomposition::v_find_groups(CRealCoding *pcWorst, CRealCoding *pcBest, double **ppdTheta, uint32_t iIterationNumber)
{
	v_calculate_steps();

	uint16_t i_number_of_elements = pc_problem->pcGetEvaluation()->iGetNumberOfElements();

	CGenePattern *pc_pattern;

	unordered_set<int16_t> s_added_indexes;
	s_added_indexes.reserve(i_number_of_elements);

	vector<CGenePattern*> v_patterns;
	v_patterns.reserve(i_number_of_elements);

	vector<uint16_t> v_indexes(pc_problem->pcGetEvaluation()->iGetNumberOfElements());

	iota(v_indexes.begin(), v_indexes.end(), 0);
	random_shuffle(v_indexes.begin(), v_indexes.end());

	vector<CGenePattern*> v_one_element_patterns;
	v_one_element_patterns.reserve(i_number_of_elements);

	uint16_t i_max_group_size = 0;

	for (uint16_t i = 0; i < i_number_of_elements; i++)
	{
		if (s_added_indexes.count(v_indexes[i]) == 0)
		{
			pc_pattern = new CGenePattern();
			pc_pattern->vAdd(v_indexes[i]);

			for (uint16_t j = i + 1; j < i_number_of_elements; j++)
			{
				if (ppdTheta[v_indexes[i]][v_indexes[j]] == 1)
				{
					pc_pattern->vAdd(v_indexes[j]);
					s_added_indexes.insert(v_indexes[j]);
				}//if (ppdTheta[i][j] == 1)
			}//for (uint16_t j = i + 1; j < i_number_of_elements; j++)

			if (pc_pattern->iGetSize() == 1)
			{
				v_one_element_patterns.push_back(pc_pattern);
			}//if (pc_pattern->iGetSize() == 1)
			else
			{
				v_patterns.push_back(pc_pattern);
			}//else if (pc_pattern->iGetSize() == 1)

			if (pc_pattern->iGetSize() > i_max_group_size)
			{
				i_max_group_size = pc_pattern->iGetSize();
			}//if (pc_pattern->iGetSize() > i_max_group_size)

			s_added_indexes.insert(v_indexes[i]);
		}//if (s_added_indexes.count(i) == 0)
	}//for (uint16_t i = 0; i < i_number_of_elements; i++)

	random_shuffle(v_patterns.begin(), v_patterns.end());

	uint16_t i_cut_point;
	size_t i_patterns_size = v_patterns.size();

	if (v_one_element_patterns.size() == (size_t)i_number_of_elements)
	{
		v_patterns.insert(v_patterns.end(), v_one_element_patterns.begin(), v_one_element_patterns.end());
	}//if (v_one_element_patterns.size() == (size_t)i_number_of_elements)
	else if (v_one_element_patterns.size() == 1)
	{
		v_patterns.push_back(v_one_element_patterns.front());
	}//else if (v_one_element_patterns.size() == 1)
	else if (v_one_element_patterns.size() > 0)
	{
		bool b_is_dependent_value = true;

		CGenePattern *pc_merged_pattern = new CGenePattern();

		for (size_t i = 0; i < v_one_element_patterns.size(); i++)
		{
			pc_merged_pattern->vAdd(v_one_element_patterns.at(i));
		}//for (size_t i = 0; i < v_one_element_patterns.size(); i++)

		pc_merged_pattern->vShuffle();

		vector<CGenePattern*> v_merged_pattern{ pc_merged_pattern };

		if (i_previous_one_element_patterns_size != v_one_element_patterns.size() || !b_previous_one_element_patterns_are_dependent)
		{
			b_is_dependent_value = false;

			CGenePattern *pc_half_merged_pattern_0 = new CGenePattern();
			CGenePattern *pc_half_merged_pattern_1 = new CGenePattern();

			for (uint16_t i = 0; i < pc_merged_pattern->iGetSize() / 2; i++)
			{
				pc_half_merged_pattern_0->vAdd(pc_merged_pattern->piGetPattern()[i]);
			}//for (uint16_t i = 0; i < pc_merged_pattern->iGetSize() / 2; i++)

			for (uint16_t i = pc_merged_pattern->iGetSize() / 2; i < pc_merged_pattern->iGetSize(); i++)
			{
				pc_half_merged_pattern_1->vAdd(pc_merged_pattern->piGetPattern()[i]);
			}//for (uint16_t i = pc_merged_pattern->iGetSize(); i < pc_merged_pattern->iGetSize(); i++)

			vector<CGenePattern*> v_half_merged_pattern_0{ pc_half_merged_pattern_0 };
			vector<CGenePattern*> v_half_merged_pattern_1{ pc_half_merged_pattern_1 };

			b_is_dependent_value = b_is_dependent(&v_half_merged_pattern_0, &v_half_merged_pattern_1, pcWorst, pcBest) || b_is_dependent(&v_half_merged_pattern_1, &v_half_merged_pattern_0, pcWorst, pcBest);

			delete pc_half_merged_pattern_0;
			delete pc_half_merged_pattern_1;

			random_shuffle(v_patterns.begin(), v_patterns.end());

			size_t i_cut_length = 1;

			vector<CGenePattern*> v_patterns_to_check_dependency;
			v_patterns_to_check_dependency.reserve(i_cut_length);

			for (size_t i = 0; i < v_patterns.size() && !b_is_dependent_value; i++)
			{
				if (v_patterns_to_check_dependency.size() == i_cut_length)
				{
					b_is_dependent_value = b_is_dependent(&v_merged_pattern, &v_patterns_to_check_dependency, pcWorst, pcBest) || b_is_dependent(&v_patterns_to_check_dependency, &v_merged_pattern, pcWorst, pcBest);
					v_patterns_to_check_dependency.clear();
				}//if (v_patterns_to_check_dependency.size() == i_cut_length)

				v_patterns_to_check_dependency.push_back(v_patterns.at(i));
			}//for (size_t i = 0; i < v_patterns.size() && !b_is_dependent_value; i++)

			if (!b_is_dependent_value && !v_patterns_to_check_dependency.empty())
			{
				b_is_dependent_value = b_is_dependent(&v_merged_pattern, &v_patterns_to_check_dependency, pcWorst, pcBest) || b_is_dependent(&v_patterns_to_check_dependency, &v_merged_pattern, pcWorst, pcBest);
			}//if (!b_is_dependent_value && !v_patterns_to_check_dependency.empty())

			if (b_is_dependent_value)
			{
				i_number_consecutive_of_zero_update_counters = 0;
			}//if (b_is_dependent_value)
		}//if (i_previous_one_element_patterns_size != v_one_element_patterns.size() || !b_previous_one_element_patterns_are_dependent)

		i_previous_one_element_patterns_size = v_one_element_patterns.size();
		b_previous_one_element_patterns_are_dependent = b_is_dependent_value;

		if (b_is_dependent_value)
		{
			v_patterns.insert(v_patterns.end(), v_one_element_patterns.begin(), v_one_element_patterns.end());
		}//if (b_is_dependent(&v_merged_pattern, &v_patterns, pcWorst, pcBest) || b_is_dependent(&v_patterns, &v_merged_pattern, pcWorst, pcBest))
		else
		{
			VectorUtils::vDeleteElements(&v_one_element_patterns);
		}//else if (b_is_dependent(&v_merged_pattern, &v_patterns, pcWorst, pcBest) || b_is_dependent(&v_patterns, &v_merged_pattern, pcWorst, pcBest))

		VectorUtils::vDeleteElementsAndClear(&v_merged_pattern);
	}//else if (v_one_element_patterns.size() > 0)

	v_one_element_patterns.clear();

	CString s_log;

	s_log.Format("groups before %d iteration:", iIterationNumber);

	for (size_t i = 0; i < v_patterns.size(); i++)
	{
		s_log.AppendFormat(" %d", v_patterns.at(i)->iGetSize());
	}//for (size_t i = 0; i < v_patterns.size(); i++)

	pc_log->vPrintLine(s_log, true);

	v_find_groups(pcWorst, pcBest, &v_patterns);

	VectorUtils::vDeleteElementsAndClear(&v_patterns);
}//void CRealIncrementalRecursiveRankingGroupingDecomposition::v_find_groups(CRealCoding *pcWorst, CRealCoding *pcBest, double **ppdTheta, uint32_t iIterationNumber)

void CRealIncrementalRecursiveRankingGroupingDecomposition::v_find_groups(CRealCoding *pcWorst, CRealCoding *pcBest, vector<CGenePattern*> *pvPatterns)
{
	vector<CGenePattern*> v_reversed_patterns(pvPatterns->rbegin(), pvPatterns->rend());

	vector<CGenePattern*> v_patterns_0;

	v_patterns_0.reserve(pvPatterns->size());

	v_patterns_0.push_back(v_reversed_patterns.back());
	v_reversed_patterns.pop_back();

	uint16_t i_cut_counter = 0;

	while (!v_reversed_patterns.empty())
	{
		vector<CGenePattern*> v_copied_patterns_0(v_patterns_0);

		v_interact(&v_copied_patterns_0, &v_reversed_patterns, pcWorst, pcBest);

		if (v_copied_patterns_0.size() == v_patterns_0.size())
		{
			uint16_t i_reversed_patterns_min_size = v_reversed_patterns.front()->iGetSize();

			for (size_t ii = 1; ii < v_reversed_patterns.size(); ii++)
			{
				if (v_reversed_patterns.at(ii)->iGetSize() < i_reversed_patterns_min_size)
				{
					i_reversed_patterns_min_size = v_reversed_patterns.at(ii)->iGetSize();
				}//if (v_reversed_patterns.at(ii)->iGetSize() < i_reversed_patterns_min_size)
			}//for (size_t ii = 1; ii < v_reversed_patterns.size(); ii++)

			if (v_copied_patterns_0.front()->iGetSize() > 1 && v_copied_patterns_0.front()->iGetSize() >= i_reversed_patterns_min_size)
			{
				CGenePattern *pc_pattern = v_copied_patterns_0.front();
				uint16_t i_cut_size = pc_pattern->iGetSize() / 2;

				for (uint16_t i = 0; i < i_cut_size; i++)
				{
					pc_pattern->bRemove(*pc_pattern->piGetPattern());
				}//for (uint16_t i = 0; i < i_cut_size; i++)

				i_cut_counter++;
			}//if (v_copied_patterns_0.size() == 1 && v_copied_patterns_0.front()->iGetSize() > 1)
			else
			{
				CGenePattern *pc_found_group = new CGenePattern();

				for (size_t i = 0; i < v_patterns_0.size(); i++)
				{
					pc_found_group->vAdd(v_patterns_0.at(i));
				}//for (size_t i = 0; i < v_patterns_0.size(); i++)

				pc_found_group->vSort();
				v_groups.push_back(pc_found_group);

				v_patterns_0.clear();
				v_patterns_0.push_back(v_reversed_patterns.back());

				v_reversed_patterns.pop_back();

				i_cut_counter = 0;
			}//else if (v_copied_patterns_0.size() == 1 && v_copied_patterns_0.front()->iGetSize() > 1)
		}//if (v_copied_patterns_0.size() == v_patterns_0.size())
		else
		{
			v_patterns_0.clear();
			v_patterns_0.insert(v_patterns_0.end(), v_copied_patterns_0.begin(), v_copied_patterns_0.end());

			for (size_t i = 0; i < v_patterns_0.size(); i++)
			{
				VectorUtils::bRemove(&v_reversed_patterns, v_patterns_0.at(i));
			}//for (size_t i = 0; i < v_patterns_0.size(); i++)
		}//else if (v_copied_patterns_0.size() == v_patterns_0.size())
	}//while (!v_reversed_patterns.empty())

	CGenePattern *pc_found_group = new CGenePattern();

	for (size_t i = 0; i < v_patterns_0.size(); i++)
	{
		pc_found_group->vAdd(v_patterns_0.at(i));
	}//for (size_t i = 0; i < v_patterns_0.size(); i++)

	pc_found_group->vSort();
	v_groups.push_back(pc_found_group);
}//void CRealIncrementalRecursiveRankingGroupingDecomposition::v_find_groups(CRealCoding *pcWorst, CRealCoding *pcBest, vector<CGenePattern*> *pvPatterns)

bool CRealIncrementalRecursiveRankingGroupingDecomposition::b_is_dependent(vector<CGenePattern*> *pvPatterns0, vector<CGenePattern*> *pvPatterns1, CRealCoding *pcWorst, CRealCoding *pcBest)
{
	vector<CRealCoding*> v_samples;
	vector<uint32_t> v_indexes;
	vector<double> v_fitnesses;

	CRealCoding *pc_sample;

	uint16_t i_number_of_elements = pc_problem->pcGetEvaluation()->iGetNumberOfElements();

	uint16_t **ppi_order = new uint16_t*[i_number_of_elements];

	for (uint16_t i = 0; i < i_number_of_elements; i++)
	{
		*(ppi_order + i) = new uint16_t[i_number_of_samples];
		iota(*(ppi_order + i) + 0, *(ppi_order + i) + i_number_of_samples, 0);
		random_shuffle(*(ppi_order + i) + 0, *(ppi_order + i) + i_number_of_samples);
	}//for (uint16_t i = 0; i < i_number_of_elements; i++)

	for (uint32_t i = 0; i < i_number_of_samples; i++)
	{
		pc_sample = pc_generation->pcGenerateEmpty();

		for (uint16_t j = 0; j < pc_sample->iGetNumberOfDimensions(); j++)
		{
			*(pc_sample->pdGetValues() + j) = *(pc_sample->pdGetMinValues() + j) + *(*(ppi_order + j) + i) * *(pd_steps + j);
		}//for (uint16_t j = 0; j < pc_sample->iGetNumberOfDimensions(); j++)

		v_samples.push_back(pc_sample);
	}//for (uint32_t i = 0; i < i_number_of_samples; i++)

	PointerUtils::vDelete(ppi_order, i_number_of_elements);

	CRealCoding *pc_candidate = new CRealCoding(pcBest);

	double d_fitness;

	for (uint32_t i = 0; i < i_number_of_samples; i++)
	{
		d_fitness = d_calculate_fitness(pvPatterns0, nullptr, v_samples.at(i), nullptr, pc_candidate);

		v_fitnesses.push_back(d_fitness);
		v_indexes.push_back(i);
	}//for (uint32_t i = 0; i < i_number_of_samples; i++)

	sort(v_indexes.begin(), v_indexes.end(), [&](uint16_t iIndex0, uint16_t iIndex1)
	{
		return v_fitnesses.at(iIndex0) > v_fitnesses.at(iIndex1);
	});//sort(v_indexes.begin(), v_indexes.end(), [&](uint16_t iIndex0, uint16_t iIndex1)

	bool b_is_dependent = false;

	double d_previous_fitness = d_calculate_fitness(pvPatterns0, pvPatterns1, v_samples.at(v_indexes.front()), pcWorst, pc_candidate);

	double d_epsilon_buf_1, d_epsilon_buf_2;
	double d_sum_abs_1, d_sum_abs_2;

	for (uint32_t i = 1; i < i_number_of_samples && !b_is_dependent; i++)
	{
		d_fitness = d_calculate_fitness(pvPatterns0, pvPatterns1, v_samples.at(v_indexes.at(i)), pcWorst, pc_candidate);

		d_sum_abs_1 = abs(d_previous_fitness) + abs(d_fitness);
		d_sum_abs_2 = abs(v_fitnesses.at(v_indexes.at(i - 1))) + abs(v_fitnesses.at(v_indexes.at(i)));

		d_epsilon_buf_1 = d_calculate_epsilon(d_previous_fitness, d_fitness);
		d_epsilon_buf_2 = d_calculate_epsilon(v_fitnesses.at(v_indexes.at(i - 1)), v_fitnesses.at(v_indexes.at(i)));

		b_is_dependent = i_compare(d_previous_fitness, d_fitness, d_epsilon_buf_1) != i_compare(v_fitnesses.at(v_indexes.at(i - 1)), v_fitnesses.at(v_indexes.at(i)), d_epsilon_buf_2);

		if (b_is_dependent)
		{
			if (i_compare(d_previous_fitness, d_fitness, d_epsilon_buf_1) == 0)
			{
				b_is_dependent = false;
			}//if (i_compare(d_previous_fitness, d_fitness, d_epsilon_buf_1) == 0)

			if (i_compare(v_fitnesses.at(v_indexes.at(i - 1)), v_fitnesses.at(v_indexes.at(i)), d_epsilon_buf_2) == 0)
			{
				b_is_dependent = false;
			}//if (i_compare(v_fitnesses.at(v_indexes.at(i - 1)), v_fitnesses.at(v_indexes.at(i)), d_epsilon_buf_2) == 0)
		}//if (b_is_dependent)

		d_previous_fitness = d_fitness;
	}//for (uint32_t i = 1; i < i_number_of_samples && !b_is_dependent; i++)

	delete pc_candidate;

	return b_is_dependent;
}//bool CRealIncrementalRecursiveRankingGroupingDecomposition::b_is_dependent(vector<CGenePattern*> *pvPatterns0, vector<CGenePattern*> *pvPatterns1, CRealCoding *pcWorst, CRealCoding *pcBest)

void CRealIncrementalRecursiveRankingGroupingDecomposition::v_interact(vector<CGenePattern*> *pvPatterns0, vector<CGenePattern*> *pvPatterns1, CRealCoding *pcWorst, CRealCoding *pcBest)
{
	vector<CRealCoding*> v_samples;
	vector<uint32_t> v_indexes;
	vector<double> v_fitnesses;

	CRealCoding *pc_sample;

	uint16_t i_number_of_elements = pc_problem->pcGetEvaluation()->iGetNumberOfElements();

	uint16_t **ppi_order = new uint16_t*[i_number_of_elements];

	for (uint16_t i = 0; i < i_number_of_elements; i++)
	{
		*(ppi_order + i) = new uint16_t[i_number_of_samples];
		iota(*(ppi_order + i) + 0, *(ppi_order + i) + i_number_of_samples, 0);
		random_shuffle(*(ppi_order + i) + 0, *(ppi_order + i) + i_number_of_samples);
	}//for (uint16_t i = 0; i < i_number_of_elements; i++)

	for (uint32_t i = 0; i < i_number_of_samples; i++)
	{
		pc_sample = pc_generation->pcGenerateEmpty();

		for (uint16_t j = 0; j < pc_sample->iGetNumberOfDimensions(); j++)
		{
			*(pc_sample->pdGetValues() + j) = *(pc_sample->pdGetMinValues() + j) + *(*(ppi_order + j) + i) * *(pd_steps + j);
		}//for (uint16_t j = 0; j < pc_sample->iGetNumberOfDimensions(); j++)

		v_samples.push_back(pc_sample);
	}//for (uint32_t i = 0; i < i_number_of_samples; i++)

	PointerUtils::vDelete(ppi_order, i_number_of_elements);

	CRealCoding *pc_candidate = new CRealCoding(pcBest);

	double d_fitness;

	for (uint32_t i = 0; i < i_number_of_samples; i++)
	{
		d_fitness = d_calculate_fitness(pvPatterns0, nullptr, v_samples.at(i), nullptr, pc_candidate);

		v_fitnesses.push_back(d_fitness);
		v_indexes.push_back(i);
	}//for (uint32_t i = 0; i < i_number_of_samples; i++)

	sort(v_indexes.begin(), v_indexes.end(), [&](uint16_t iIndex0, uint16_t iIndex1)
	{
		return v_fitnesses.at(iIndex0) > v_fitnesses.at(iIndex1);
	});//sort(v_indexes.begin(), v_indexes.end(), [&](uint16_t iIndex0, uint16_t iIndex1)

	bool b_is_dependent = false;

	double d_previous_fitness = d_calculate_fitness(pvPatterns0, pvPatterns1, v_samples.at(v_indexes.front()), pcWorst, pc_candidate);

	double d_epsilon_buf_1, d_epsilon_buf_2;
	double d_sum_abs_1, d_sum_abs_2;

	for (uint32_t i = 1; i < i_number_of_samples && !b_is_dependent; i++)
	{
		d_fitness = d_calculate_fitness(pvPatterns0, pvPatterns1, v_samples.at(v_indexes.at(i)), pcWorst, pc_candidate);

		d_sum_abs_1 = abs(d_previous_fitness) + abs(d_fitness);
		d_sum_abs_2 = abs(v_fitnesses.at(v_indexes.at(i - 1))) + abs(v_fitnesses.at(v_indexes.at(i)));

		d_epsilon_buf_1 = d_calculate_epsilon(d_previous_fitness, d_fitness);
		d_epsilon_buf_2 = d_calculate_epsilon(v_fitnesses.at(v_indexes.at(i - 1)), v_fitnesses.at(v_indexes.at(i)));

		b_is_dependent = i_compare(d_previous_fitness, d_fitness, d_epsilon_buf_1) != i_compare(v_fitnesses.at(v_indexes.at(i - 1)), v_fitnesses.at(v_indexes.at(i)), d_epsilon_buf_2);

		if (b_is_dependent)
		{
			if (i_compare(d_previous_fitness, d_fitness, d_epsilon_buf_1) == 0)
			{
				b_is_dependent = false;
			}//if (i_compare(d_previous_fitness, d_fitness, d_epsilon_buf_1) == 0)

			if (i_compare(v_fitnesses.at(v_indexes.at(i - 1)), v_fitnesses.at(v_indexes.at(i)), d_epsilon_buf_2) == 0)
			{
				b_is_dependent = false;
			}//if (i_compare(v_fitnesses.at(v_indexes.at(i - 1)), v_fitnesses.at(v_indexes.at(i)), d_epsilon_buf_2) == 0)
		}//if (b_is_dependent)

		d_previous_fitness = d_fitness;
	}//for (uint32_t i = 1; i < i_number_of_samples && !b_is_dependent; i++)

	delete pc_candidate;

	if (b_is_dependent)
	{
		if (pvPatterns1->size() == 1)
		{
			pvPatterns0->insert(pvPatterns0->end(), pvPatterns1->begin(), pvPatterns1->end());
		}//if (pvPatterns1->size() == 1)
		else
		{
			size_t i_split_index = pvPatterns1->size() / 2;

			vector<CGenePattern*> v_new_patterns_1_0;
			vector<CGenePattern*> v_new_patterns_1_1;

			v_new_patterns_1_0.reserve(pvPatterns1->size());
			v_new_patterns_1_1.reserve(pvPatterns1->size());

			for (size_t i = 0; i < i_split_index; i++)
			{
				v_new_patterns_1_0.push_back(pvPatterns1->at(i));
			}//for (size_t i = 0; i < i_split_index; i++)

			for (size_t i = i_split_index; i < pvPatterns1->size(); i++)
			{
				v_new_patterns_1_1.push_back(pvPatterns1->at(i));
			}//for (size_t i = i_split_index; i < pvPatterns1->size(); i++)

			vector<CGenePattern*> v_new_patterns_0_0(*pvPatterns0);
			vector<CGenePattern*> v_new_patterns_0_1(*pvPatterns0);

			v_interact(&v_new_patterns_0_0, &v_new_patterns_1_0, pcWorst, pcBest, &v_samples, &v_fitnesses, &v_indexes);
			v_interact(&v_new_patterns_0_1, &v_new_patterns_1_1, pcWorst, pcBest, &v_samples, &v_fitnesses, &v_indexes);

			size_t i_patterns_0_size = pvPatterns0->size();

			pvPatterns0->insert(pvPatterns0->end(), v_new_patterns_0_0.begin() + i_patterns_0_size, v_new_patterns_0_0.end());
			pvPatterns0->insert(pvPatterns0->end(), v_new_patterns_0_1.begin() + i_patterns_0_size, v_new_patterns_0_1.end());
		}//else if (pvPatterns1->size() == 1)
	}//if (b_is_dependent)
}//void CRealIncrementalRecursiveRankingGroupingDecomposition::v_interact(vector<CGenePattern*> *pvPatterns0, vector<CGenePattern*> *pvPatterns1, CRealCoding *pcWorst, CRealCoding *pcBest)

void CRealIncrementalRecursiveRankingGroupingDecomposition::v_interact(vector<CGenePattern*> *pvPatterns0, vector<CGenePattern*> *pvPatterns1, CRealCoding *pcWorst, CRealCoding *pcBest, vector<CRealCoding*> *pvSamples, vector<double> *pvFitnesses, vector<uint32_t> *pvIndexes)
{
	CRealCoding *pc_candidate = new CRealCoding(pcBest);

	double d_fitness;

	bool b_is_dependent = false;

	double d_previous_fitness = d_calculate_fitness(pvPatterns0, pvPatterns1, pvSamples->at(pvIndexes->front()), pcWorst, pc_candidate);

	double d_epsilon_buf_1, d_epsilon_buf_2;
	double d_sum_abs_1, d_sum_abs_2;

	for (uint32_t i = 1; i < i_number_of_samples && !b_is_dependent; i++)
	{
		d_fitness = d_calculate_fitness(pvPatterns0, pvPatterns1, pvSamples->at(pvIndexes->at(i)), pcWorst, pc_candidate);

		d_sum_abs_1 = abs(d_previous_fitness) + abs(d_fitness);
		d_sum_abs_2 = abs(pvFitnesses->at(pvIndexes->at(i - 1))) + abs(pvFitnesses->at(pvIndexes->at(i)));

		d_epsilon_buf_1 = d_calculate_epsilon(d_previous_fitness, d_fitness);
		d_epsilon_buf_2 = d_calculate_epsilon(pvFitnesses->at(pvIndexes->at(i - 1)), pvFitnesses->at(pvIndexes->at(i)));

		b_is_dependent = i_compare(d_previous_fitness, d_fitness, d_epsilon_buf_1) != i_compare(pvFitnesses->at(pvIndexes->at(i - 1)), pvFitnesses->at(pvIndexes->at(i)), d_epsilon_buf_2);

		if (b_is_dependent)
		{
			if (i_compare(d_previous_fitness, d_fitness, d_epsilon_buf_1) == 0)
			{
				b_is_dependent = false;
			}//if (i_compare(d_previous_fitness, d_fitness, d_epsilon_buf_1) == 0)

			if (i_compare(pvFitnesses->at(pvIndexes->at(i - 1)), pvFitnesses->at(pvIndexes->at(i)), d_epsilon_buf_2) == 0)
			{
				b_is_dependent = false;
			}//if (i_compare(pvFitnesses->at(pvIndexes->at(i - 1)), pvFitnesses->at(pvIndexes->at(i)), d_epsilon_buf_2)) == 0)
		}//if (b_is_dependent)

		d_previous_fitness = d_fitness;
	}//for (uint32_t i = 1; i < i_number_of_samples && !b_is_dependent; i++)

	delete pc_candidate;

	if (b_is_dependent)
	{
		if (pvPatterns1->size() == 1)
		{
			pvPatterns0->insert(pvPatterns0->end(), pvPatterns1->begin(), pvPatterns1->end());
		}//if (pvPatterns1->size() == 1)
		else
		{
			size_t i_split_index = pvPatterns1->size() / 2;

			vector<CGenePattern*> v_new_patterns_1_0;
			vector<CGenePattern*> v_new_patterns_1_1;

			v_new_patterns_1_0.reserve(pvPatterns1->size());
			v_new_patterns_1_1.reserve(pvPatterns1->size());

			for (size_t i = 0; i < i_split_index; i++)
			{
				v_new_patterns_1_0.push_back(pvPatterns1->at(i));
			}//for (size_t i = 0; i < i_split_index; i++)

			for (size_t i = i_split_index; i < pvPatterns1->size(); i++)
			{
				v_new_patterns_1_1.push_back(pvPatterns1->at(i));
			}//for (size_t i = i_split_index; i < pvPatterns1->size(); i++)

			vector<CGenePattern*> v_new_patterns_0_0(*pvPatterns0);
			vector<CGenePattern*> v_new_patterns_0_1(*pvPatterns0);

			v_interact(&v_new_patterns_0_0, &v_new_patterns_1_0, pcWorst, pcBest, pvSamples, pvFitnesses, pvIndexes);
			v_interact(&v_new_patterns_0_1, &v_new_patterns_1_1, pcWorst, pcBest, pvSamples, pvFitnesses, pvIndexes);

			size_t i_patterns_0_size = pvPatterns0->size();

			pvPatterns0->insert(pvPatterns0->end(), v_new_patterns_0_0.begin() + i_patterns_0_size, v_new_patterns_0_0.end());
			pvPatterns0->insert(pvPatterns0->end(), v_new_patterns_0_1.begin() + i_patterns_0_size, v_new_patterns_0_1.end());
		}//else if (pvPatterns1->size() == 1)
	}//if (b_is_dependent)
}//void CRealIncrementalRecursiveRankingGroupingDecomposition::v_interact(vector<CGenePattern*> *pvPatterns0, vector<CGenePattern*> *pvPatterns1, CRealCoding *pcWorst, CRealCoding *pcBest, vector<CRealCoding*> *pvSamples, vector<double> *pvFitnesses, vector<uint32_t> *pvIndexes)

double CRealIncrementalRecursiveRankingGroupingDecomposition::d_calculate_fitness(vector<CGenePattern*> *pvPatterns0, vector<CGenePattern*> *pvPatterns1, CRealCoding *pcContext0, CRealCoding *pcContext1, CRealCoding *pcCandidate)
{
	CGenePattern *pc_pattern;
	uint16_t i_pattern;

	if (pvPatterns0 && pcContext0)
	{
		for (size_t i = 0; i < pvPatterns0->size(); i++)
		{
			pc_pattern = pvPatterns0->at(i);

			for (uint16_t j = 0; j < pc_pattern->iGetSize(); j++)
			{
				i_pattern = *(pc_pattern->piGetPattern() + j);
				*(pcCandidate->pdGetValues() + i_pattern) = *(pcContext0->pdGetValues() + i_pattern);
			}//for (uint16_t j = 0; j < pc_pattern->iGetSize(); j++)
		}//for (size_t i = 0; i < pvPatterns0->size(); i++)
	}//if (pvPatterns0 && pcContext0)

	if (pvPatterns1 && pcContext1)
	{
		for (size_t i = 0; i < pvPatterns1->size(); i++)
		{
			pc_pattern = pvPatterns1->at(i);

			for (uint16_t j = 0; j < pc_pattern->iGetSize(); j++)
			{
				i_pattern = *(pc_pattern->piGetPattern() + j);
				*(pcCandidate->pdGetValues() + i_pattern) = *(pcContext1->pdGetValues() + i_pattern);
			}//for (uint16_t j = 0; j < pc_pattern->iGetSize(); j++)
		}//for (size_t i = 0; i < pvPatterns1->size(); i++)
	}//if (pvPatterns1 && pcContext1)

	return pc_problem->pcGetEvaluation()->dEvaluate(pcCandidate);
}//double CRealIncrementalRecursiveRankingGroupingDecomposition::d_calculate_fitness(vector<CGenePattern*> *pvPatterns0, vector<CGenePattern*> *pvPatterns1, CRealCoding *pcContext0, CRealCoding *pcContext1, CRealCoding *pcCandidate)

void CRealIncrementalRecursiveRankingGroupingDecomposition::v_calculate_steps()
{
	double d_min_value, d_max_value;

	CRealCoding *pc_sample_fenotype = pc_problem->pcGetEvaluation()->pcCreateSampleFenotype();

	for (uint16_t i = 0; i < pc_problem->pcGetEvaluation()->iGetNumberOfElements(); i++)
	{
		d_min_value = *(pc_sample_fenotype->pdGetMinValues() + i);
		d_max_value = *(pc_sample_fenotype->pdGetMaxValues() + i);

		*(pd_steps + i) = (d_max_value - d_min_value) / (double)(i_number_of_samples - 1);
	}//for (uint16_t i = 0; i < pc_problem->pcGetEvaluation()->iGetNumberOfElements(); i++)

	delete pc_sample_fenotype;
}//void CRealIncrementalRecursiveRankingGroupingDecomposition::v_calculate_steps()

double CRealIncrementalRecursiveRankingGroupingDecomposition::d_calculate_epsilon(double dValue0, double dValue1)
{
	double d_mu_m = DBL_EPSILON / 2.0;
	double d_sqrt_elements = pow((double)pc_problem->pcGetEvaluation()->iGetNumberOfElements(), 0.5);
	double d_sum_abs = abs(dValue0) + abs(dValue1);

	return MathUtils::dComputeGamma(d_mu_m, d_sqrt_elements + 1.0) * d_sum_abs;
}//double CRealIncrementalRecursiveRankingGroupingDecomposition::d_calculate_epsilon(double dValue0, double dValue1)

int8_t CRealIncrementalRecursiveRankingGroupingDecomposition::i_compare(double dValue0, double dValue1, double dEpsilon)
{
	int8_t i_comparison_result;

	if (abs(dValue0 - dValue1) <= dEpsilon)
	{
		i_comparison_result = 0;
	}//if (abs(dValue0 - dValue1) <= dEpsilon)
	else if (dValue0 > dValue1)
	{
		i_comparison_result = 1;
	}//else if (dValue0 == dValue1)
	else
	{
		i_comparison_result = -1;
	}//else

	return i_comparison_result;
}//int8_t CRealIncrementalRecursiveRankingGroupingDecomposition::i_compare(double dValue0, double dValue1, double dEpsilon)

double ** CRealIncrementalRecursiveRankingGroupingDecomposition::ppd_create_theta()
{
	uint16_t i_number_of_elements = pc_problem->pcGetEvaluation()->iGetNumberOfElements();

	double **ppd_theta = new double*[i_number_of_elements];

	for (uint16_t i = 0; i < i_number_of_elements; i++)
	{
		*(ppd_theta + i) = new double[i_number_of_elements];

		for (uint16_t j = 0; j < i_number_of_elements; j++)
		{
			*(*(ppd_theta + i) + j) = 0;
		}//for (uint16_t j = 0; j < i_number_of_elements; j++)

		*(*(ppd_theta + i) + i) = 1;
	}//for (uint16_t i = 0; i < i_number_of_elements; i++)

	return ppd_theta;
}//double ** CRealIncrementalRecursiveRankingGroupingDecomposition::ppd_create_theta()

uint32_t CRealIncrementalRecursiveRankingGroupingDecomposition::i_update_theta(double **ppdTheta)
{
	uint32_t i_counter = 0;

	CGenePattern *pc_pattern;

	uint16_t i_index_0, i_index_1;

	unordered_set<uint16_t> s_visited;

	for (uint16_t i = 0; i < v_groups.size(); i++)
	{
		pc_pattern = v_groups.at(i);

		for (uint16_t j = 0; j + 1 < pc_pattern->iGetSize(); j++)
		{
			i_index_0 = pc_pattern->piGetPattern()[j];

			for (uint16_t k = j + 1; k < pc_pattern->iGetSize(); k++)
			{
				i_index_1 = pc_pattern->piGetPattern()[k];

				if (ppdTheta[i_index_0][i_index_1] == 0)
				{
					ppdTheta[i_index_0][i_index_1] = 1;
					ppdTheta[i_index_1][i_index_0] = 1;

					s_visited.clear();
					v_update_theta(ppdTheta, i_index_0, i_index_1, &s_visited);

					s_visited.clear();
					v_update_theta(ppdTheta, i_index_1, i_index_0, &s_visited);

					i_counter++;
				}//if (ppdTheta[i_index_0][i_index_1] == 0)
			}//for (uint16_t k = j + 1; k < pc_pattern->iGetSize(); k++)
		}//for (uint16_t j = 0; j + 1 < pc_pattern->iGetSize(); j++)
	}//for (uint16_t i = 0; i < v_groups.size(); i++)

	return i_counter;
}//uint32_t CRealIncrementalRecursiveRankingGroupingDecomposition::i_update_theta(double **ppdTheta)

void CRealIncrementalRecursiveRankingGroupingDecomposition::v_update_theta(double **ppdTheta, uint16_t iSrc, uint16_t iDst, unordered_set<uint16_t> *psVisited)
{
	if (psVisited->count(iDst) == 0)
	{
		psVisited->insert(iDst);

		uint16_t i_number_of_elements = pc_problem->pcGetEvaluation()->iGetNumberOfElements();

		double *pd_theta_src = ppdTheta[iSrc];
		double *pd_theta_dst = ppdTheta[iDst];

		for (uint16_t i = 0; i < i_number_of_elements; i++)
		{
			if (i != iSrc && i != iDst)
			{
				if (pd_theta_src[i] == 1 && pd_theta_dst[i] == 0)
				{
					pd_theta_dst[i] = 1;
					v_update_theta(ppdTheta, iSrc, i, psVisited);
				}//if (pd_theta_src[i] == 1)
			}//if (i != iSrc && i != iDst)
		}//for (uint16_t i = 0; i < i_number_of_elements; i++)
	}//if (psVisited->count(iDst) == 0)
}//void CRealIncrementalRecursiveRankingGroupingDecomposition::v_update_theta(double **ppdTheta, uint16_t iSrc, uint16_t iDst, unordered_set<uint16_t> *psVisited)


CRealFastInterdependenceSearchingDecomposition::CRealFastInterdependenceSearchingDecomposition(CProblem<CRealCoding, CRealCoding> *pcProblem, CLog *pcLog, uint32_t iRandomSeed)
	: CDecomposition<CRealCoding, CRealCoding>(pcProblem, pcLog, iRandomSeed)
{

}//CRealFastInterdependenceSearchingDecomposition::CRealFastInterdependenceSearchingDecomposition(CProblem<CRealCoding, CRealCoding> *pcProblem, CLog *pcLog, uint32_t iRandomSeed)

CError CRealFastInterdependenceSearchingDecomposition::eConfigure(istream *psSettings)
{
	CError c_error = CDecomposition<CRealCoding, CRealCoding>::eConfigure(psSettings);

	if (!c_error)
	{
		CUIntCommandParam p_N("N", (uint32_t)1, (uint32_t)UINT16_MAX, (uint32_t)10);
		i_N = p_N.iGetValue(psSettings, &c_error);
	}//if (!c_error)

	return c_error;
}//CError CRealFastInterdependenceSearchingDecomposition::eConfigure(istream *psSettings)

void CRealFastInterdependenceSearchingDecomposition::vRun()
{
	VectorUtils::vDeleteElementsAndClear(&v_groups);

	CEvaluation<CRealCoding> *pc_evaluation = pc_problem->pcGetEvaluation();

	CRealCoding *pc_sample_genotype = pc_evaluation->pcCreateSampleFenotype();
	
	CRealRandomGeneration c_random_generation(pc_sample_genotype);

	delete pc_sample_genotype;

	vector<uint16_t> v_variables(pc_evaluation->iGetNumberOfElements());

	iota(v_variables.begin(), v_variables.end(), 0);

	while (!v_variables.empty())
	{
		uint16_t i_random_variable_index = RandUtils::iRandIndex((uint16_t)v_variables.size());
		uint16_t i_random_variable = v_variables[i_random_variable_index];

		v_variables.erase(v_variables.begin() + i_random_variable_index);

		CGenePattern *pc_group = pc_find_group(i_random_variable, &v_variables, &c_random_generation);

		pc_group->vAdd(i_random_variable);
		pc_group->vSort();

		v_remove_from_variables(&v_variables, pc_group);

		CString s_group_log;

		s_group_log.Format("group of %d variable(s)", pc_group->iGetSize());

		pc_log->vPrintLine(s_group_log, true);

		v_groups.push_back(pc_group);
	}//while (!v_variables.empty())

	sort(v_groups.begin(), v_groups.end(), [](CGenePattern *pcGroup0, CGenePattern *pcGroup1)
	{
		return pcGroup0->piGetPattern()[0] < pcGroup1->piGetPattern()[0];
	});//sort(v_groups.begin(), v_groups.end(), [](CGenePattern *pcGroup0, CGenePattern *pcGroup1)

	v_log_theta();

	CString s_groups;

	s_groups.AppendFormat("number_of_group: %d", v_groups.size());

	pc_log->vPrintLine(s_groups, true);
}//void CRealFastInterdependenceSearchingDecomposition::vRun()

CGenePattern * CRealFastInterdependenceSearchingDecomposition::pc_find_group(uint16_t iVariable, vector<uint16_t> *pvResidualVariables, CRealRandomGeneration *pcRandomGeneration)
{
	CGenePattern *pc_group = new CGenePattern();

	bool b_is_dependent = false;

	for (uint16_t i = 0; i < i_N && !b_is_dependent; i++)
	{
		CRealCoding *pc_context = pcRandomGeneration->pcGenerate();
		CRealCoding *pc_solution = pcRandomGeneration->pcGenerate();
		CRealCoding *pc_solution_prime = pcRandomGeneration->pcGenerate();

		double d_alpha = d_evaluate(pc_context, pc_solution, pc_solution, iVariable, pvResidualVariables);
		double d_alpha_prime = d_evaluate(pc_context, pc_solution, pc_solution_prime, iVariable, pvResidualVariables);

		double d_beta = d_evaluate(pc_context, pc_solution_prime, pc_solution, iVariable, pvResidualVariables);
		double d_beta_prime = d_evaluate(pc_context, pc_solution_prime, pc_solution_prime, iVariable, pvResidualVariables);

		delete pc_context;
		delete pc_solution;
		delete pc_solution_prime;

		b_is_dependent = (d_alpha - d_beta) * (d_alpha_prime - d_beta_prime) < 0;
	}//for (uint16_t i = 0; i < i_N && !b_is_dependent; i++)

	if (b_is_dependent)
	{
		if (pvResidualVariables->size() > 1)
		{
			vector<uint16_t> v_residual_variables_0;
			v_residual_variables_0.reserve(pvResidualVariables->size() / 2);

			for (uint16_t i = 0; i < (uint16_t)pvResidualVariables->size() / 2; i++)
			{
				v_residual_variables_0.push_back(pvResidualVariables->at(i));
			}//for (uint16_t i = 0; i < (uint16_t)pvResidualVariables->size() / 2; i++)

			vector<uint16_t> v_residual_variables_1;
			v_residual_variables_1.reserve(pvResidualVariables->size() - v_residual_variables_0.size());

			for (uint16_t i = 0; i < (uint16_t)(pvResidualVariables->size() - v_residual_variables_0.size()); i++)
			{
				v_residual_variables_1.push_back(pvResidualVariables->at(i + v_residual_variables_0.size()));
			}//for (uint16_t i = 0; i < (uint16_t)(pvResidualVariables->size() - v_residual_variables_0.size()); i++)

			CGenePattern *pc_group_0 = pc_find_group(iVariable, &v_residual_variables_0, pcRandomGeneration);
			CGenePattern *pc_group_1 = pc_find_group(iVariable, &v_residual_variables_1, pcRandomGeneration);

			pc_group->vAdd(pc_group_0);
			pc_group->vAdd(pc_group_1);

			delete pc_group_0;
			delete pc_group_1;
		}//if (pvResidualVariables->size() > 1)
		else
		{
			pc_group->vAdd(pvResidualVariables->front());
		}//else if (pvResidualVariables->size() > 1)
	}//if (b_is_dependent)

	return pc_group;
}//CGenePattern * CRealFastInterdependenceSearchingDecomposition::pc_find_group(uint16_t iVariable, vector<uint16_t> *pvResidualVariables, CRealRandomGeneration *pcRandomGeneration)

double CRealFastInterdependenceSearchingDecomposition::d_evaluate(CRealCoding *pcContext, CRealCoding *pcSolution0, CRealCoding *pcSolution1, uint16_t iVariable, vector<uint16_t> *pvResidualVariables)
{
	CRealCoding c_solution_to_evaluate(pcContext);

	c_solution_to_evaluate.pdGetValues()[iVariable] = pcSolution0->pdGetValues()[iVariable];

	for (uint16_t i = 0; i < (uint16_t)pvResidualVariables->size(); i++)
	{
		c_solution_to_evaluate.pdGetValues()[pvResidualVariables->at(i)] = pcSolution1->pdGetValues()[pvResidualVariables->at(i)];
	}//for (uint16_t i = 0; i < (uint16_t)pvResidualVariables->size(); i++)

	return pc_problem->pcGetEvaluation()->dEvaluate(&c_solution_to_evaluate);
}//double CRealFastInterdependenceSearchingDecomposition::d_evaluate(CRealCoding *pcContext, CRealCoding *pcSolution0, CRealCoding *pcSolution1, uint16_t iVariable, vector<uint16_t> *pvResidualVariables)

void CRealFastInterdependenceSearchingDecomposition::v_remove_from_variables(vector<uint16_t> *pvVariables, CGenePattern *pcOrderedGroup)
{
	vector<uint16_t> v_indexes_to_erase;
	v_indexes_to_erase.reserve((size_t)pcOrderedGroup->iGetSize());

	uint16_t i_index = 0;
	uint16_t i_other_index = 0;

	while (i_index < (uint16_t)pvVariables->size() && i_other_index < pcOrderedGroup->iGetSize())
	{
		if (pvVariables->at(i_index) == pcOrderedGroup->piGetPattern()[i_other_index])
		{
			v_indexes_to_erase.push_back(i_index);

			i_index++;
			i_other_index++;
		}//if (pvVariables->at(i_index) == pcOrderedGroup->piGetPattern()[i_other_index])
		else if (pvVariables->at(i_index) < pcOrderedGroup->piGetPattern()[i_other_index])
		{
			i_index++;
		}//else if (pvVariables->at(i_index) < pcOrderedGroup->piGetPattern()[i_other_index])
		else
		{
			i_other_index++;
		}//else
	}//while (i_index < (uint16_t)pvVariables->size() && i_other_index < pcOrderedGroup->iGetSize())

	for (uint16_t i = 0; i < (uint16_t)v_indexes_to_erase.size(); i++)
	{
		pvVariables->erase(pvVariables->begin() + v_indexes_to_erase.at(i) - i);
	}//for (uint16_t i = 0; i < (uint16_t)v_indexes_to_erase.size(); i++)
}//void CRealFastInterdependenceSearchingDecomposition::v_remove_from_variables(vector<uint16_t> *pvVariables, CGenePattern *pcOrderedGroup)

void CRealFastInterdependenceSearchingDecomposition::v_log_theta()
{
	uint16_t i_number_of_elements = pc_problem->pcGetEvaluation()->iGetNumberOfElements();

	double **ppd_theta = new double*[i_number_of_elements];

	for (uint16_t i = 0; i < i_number_of_elements; i++)
	{
		*(ppd_theta + i) = new double[i_number_of_elements];

		for (uint16_t j = 0; j < i_number_of_elements; j++)
		{
			*(*(ppd_theta + i) + j) = 0;
		}//for (uint16_t j = 0; j < i_number_of_elements; j++)

		*(*(ppd_theta + i) + i) = 1;
	}//for (uint16_t i = 0; i < i_number_of_elements; i++)

	v_update_theta(ppd_theta);

	vLogTheta(ppd_theta, i_number_of_elements, pc_log);

	PointerUtils::vDelete(ppd_theta, i_number_of_elements);
}//void CRealFastInterdependenceSearchingDecomposition::v_log_theta()

void CRealFastInterdependenceSearchingDecomposition::v_update_theta(double **ppdTheta)
{
	CGenePattern *pc_pattern;

	uint16_t i_index_0, i_index_1;

	for (uint16_t i = 0; i < v_groups.size(); i++)
	{
		pc_pattern = v_groups.at(i);

		for (uint16_t j = 0; j + 1 < pc_pattern->iGetSize(); j++)
		{
			i_index_0 = pc_pattern->piGetPattern()[j];

			for (uint16_t k = j + 1; k < pc_pattern->iGetSize(); k++)
			{
				i_index_1 = pc_pattern->piGetPattern()[k];

				if (ppdTheta[i_index_0][i_index_1] == 0)
				{
					ppdTheta[i_index_0][i_index_1] = 1;
					ppdTheta[i_index_1][i_index_0] = 1;
				}//if (ppdTheta[i_index_0][i_index_1] == 0)
			}//for (uint16_t k = j + 1; k < pc_pattern->iGetSize(); k++)
		}//for (uint16_t j = 0; j + 1 < pc_pattern->iGetSize(); j++)
	}//for (uint16_t i = 0; i < v_groups.size(); i++)
}//void CRealFastInterdependenceSearchingDecomposition::v_update_theta(double **ppdTheta)


template <class TFenotype, class TGenotype>
CDecomposition<TGenotype, TFenotype> * Decomposition::pcGetDecompositon(CProblem<TGenotype, TFenotype> *pcProblem, CLog *pcLog, uint32_t iRandomSeed, istream *psSettings, CError *pcError, bool bIsObligatory)
{
	CConstructorCommandParam<CDecomposition<TGenotype, TFenotype>> p_decomposition(DECOMPOSITION_ARGUMENT_TYPE, bIsObligatory);

	size_t i_genotype_type_hash_code = typeid(TGenotype).hash_code();
	size_t i_fenotype_type_hash_code = typeid(TFenotype).hash_code();

	if (i_genotype_type_hash_code == typeid(CRealCoding).hash_code() && i_fenotype_type_hash_code == typeid(CRealCoding).hash_code())
	{
		p_decomposition.vAddConstructor(DECOMPOSITION_ARGUMENT_TYPE_REAL_IRRG, [&]()
		{
			return (CDecomposition<TGenotype, TFenotype>*)new CRealIncrementalRecursiveRankingGroupingDecomposition(pcProblem, pcLog, iRandomSeed);
		});//p_decomposition.vAddConstructor(DECOMPOSITION_ARGUMENT_TYPE_REAL_IRRG, [&]()

		p_decomposition.vAddConstructor(DECOMPOSITION_ARGUMENT_TYPE_REAL_FAST_INTERDEPENDENCE_SEARCHING, [&]()
		{
			return (CDecomposition<TGenotype, TFenotype>*)new CRealFastInterdependenceSearchingDecomposition(pcProblem, pcLog, iRandomSeed);
		});//p_decomposition.vAddConstructor(DECOMPOSITION_ARGUMENT_TYPE_REAL_FAST_INTERDEPENDENCE_SEARCHING, [&]()
	}//if (i_genotype_type_hash_code == typeid(CRealCoding).hash_code() &&& i_fenotype_type_hash_code == typeid(CRealCoding).hash_code())

	CDecomposition<TGenotype, TFenotype> *pc_decomposition = p_decomposition.pcGetValue(psSettings, pcError);

	if (!*pcError && pc_decomposition)
	{
		*pcError = pc_decomposition->eConfigure(psSettings);
	}//if (!*pcError && pc_decomposition)

	return pc_decomposition;
}//CDecomposition<TGenotype, TFenotype> * Decomposition::pcGetDecompositon(CProblem<TGenotype, TFenotype> *pcProblem, CLog *pcLog, uint32_t iRandomSeed, istream *psSettings, CError *pcError, bool bIsObligatory)

void Decomposition::vLogTheta(double **ppdTheta, uint16_t iNumberOfElements, CLog *pcLog)
{
	CString s_buf;

	for (uint16_t i = 0; i < iNumberOfElements; i++)
	{
		s_buf.Empty();

		for (uint16_t j = 0; j < iNumberOfElements; j++)
		{
			s_buf.AppendFormat("%d", (uint8_t)*(*(ppdTheta + i) + j));

			if (j + 1 < iNumberOfElements)
			{
				s_buf.Append(",");
			}//if (j + 1 < iNumberOfElements)
		}//for (uint16_t j = i + 1; j < iNumberOfElements; j++)

		pcLog->vPrintLine(s_buf, false, LOG_SYSTEM_DECOMPOSITION_THETA);
	}//for (uint16_t i = 0; i + 1 < iNumberOfElements; i++)
}//void Decomposition::vLogTheta(double **ppdTheta, uint16_t iNumberOfElements, CLog *pcLog)


template class CDecomposition<CRealCoding, CRealCoding>;


template CDecomposition<CRealCoding, CRealCoding> * Decomposition::pcGetDecompositon(CProblem<CRealCoding, CRealCoding>*, CLog*, uint32_t, istream*, CError*, bool);