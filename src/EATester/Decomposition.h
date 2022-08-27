#ifndef DECOMPOSITION_H
#define DECOMPOSITION_H

#define DECOMPOSITION_ARGUMENT_TYPE "decomposition_type"
#define DECOMPOSITION_ARGUMENT_TYPE_REAL_IRRG "real_irrg"
#define DECOMPOSITION_ARGUMENT_TYPE_REAL_FAST_INTERDEPENDENCE_SEARCHING "real_fast_interdependence_searching"

#include "Error.h"
#include "GenePattern.h"
#include "Generation.h"
#include "Log.h"
#include "Optimizer.h"
#include "Problem.h"
#include "RealCoding.h"
#include "RealEvaluation.h"
#include "RealRandomGeneration.h"

#include <cstdint>
#include <istream>
#include <vector>

using namespace std;

namespace Decomposition
{
	template <class TGenotype, class TFenotype>
	class CDecomposition
	{
	public:
		CDecomposition(CProblem<TGenotype, TFenotype> *pcProblem, CLog *pcLog, uint32_t iRandomSeed);

		virtual ~CDecomposition();

		virtual CError eConfigure(istream *psSettings);

		virtual void vRun() = 0;

		vector<CGenePattern*> *pvGetGroups() { return &v_groups; };

		CString sGetAdditionalSummary() { return s_additional_summary; };

	protected:
		CProblem<TGenotype, TFenotype> *pc_problem;
		CLog *pc_log;
		uint32_t i_random_seed;

		CString s_additional_summary;

		vector<CGenePattern*> v_groups;
	};//class CDecomposition


	class CRealIncrementalRecursiveRankingGroupingDecomposition : public CDecomposition<CRealCoding, CRealCoding>
	{
	public:
		CRealIncrementalRecursiveRankingGroupingDecomposition(CProblem<CRealCoding, CRealCoding> *pcProblem, CLog *pcLog, uint32_t iRandomSeed);

		virtual ~CRealIncrementalRecursiveRankingGroupingDecomposition();

		virtual CError eConfigure(istream *psSettings);

		virtual void vRun();

	private:
		void v_find_groups(CRealCoding *pcWorst, CRealCoding *pcBest, double **ppdTheta, uint32_t iIterationNumber);
		void v_find_groups(CRealCoding *pcWorst, CRealCoding *pcBest, vector<CGenePattern*> *pvPatterns);

		bool b_is_dependent(vector<CGenePattern*> *pvPatterns0, vector<CGenePattern*> *pvPatterns1, CRealCoding *pcWorst, CRealCoding *pcBest);

		void v_interact(vector<CGenePattern*> *pvPatterns0, vector<CGenePattern*> *pvPatterns1, CRealCoding *pcWorst, CRealCoding *pcBest);

		void v_interact
		(
			vector<CGenePattern*> *pvPatterns0, vector<CGenePattern*> *pvPatterns1,
			CRealCoding *pcWorst, CRealCoding *pcBest,
			vector<CRealCoding*> *pvSamples,
			vector<double> *pvFitnesses,
			vector<uint32_t> *pvIndexes
		);

		double d_calculate_fitness
		(
			vector<CGenePattern*> *pvPatterns0, vector<CGenePattern*> *pvPatterns1,
			CRealCoding *pcContext0, CRealCoding *pcContext1, CRealCoding *pcCandidate
		);

		void v_calculate_steps();

		double d_calculate_epsilon(double dValue0, double dValue1);

		int8_t i_compare(double dValue0, double dValue1, double dEpsilon);

		double **ppd_create_theta();
		uint32_t i_update_theta(double **ppdTheta);
		void v_update_theta(double **ppdTheta, uint16_t iSrc, uint16_t iDst, unordered_set<uint16_t> *psVisited);

		uint32_t i_number_of_samples;

		uint8_t i_number_consecutive_of_zero_update_counters;
		uint8_t i_max_number_consecutive_of_zero_update_counters;

		double *pd_steps;

		CGeneration<CRealCoding> *pc_generation;
		COptimizer<CRealCoding, CRealCoding> *pc_global_optimizer;
		COptimizer<CRealCoding, CRealCoding> *pc_local_optimizer;

		size_t i_previous_one_element_patterns_size;
		bool b_previous_one_element_patterns_are_dependent;
	};//class CRealIncrementalRecursiveRankingGroupingDecomposition : public CDecomposition<CRealCoding, CRealCoding>


	class CRealFastInterdependenceSearchingDecomposition : public CDecomposition<CRealCoding, CRealCoding>
	{
	public:
		CRealFastInterdependenceSearchingDecomposition(CProblem<CRealCoding, CRealCoding> *pcProblem, CLog *pcLog, uint32_t iRandomSeed);

		virtual CError eConfigure(istream *psSettings);

		virtual void vRun();

	private:
		CGenePattern *pc_find_group(uint16_t iVariable, vector<uint16_t> *pvResidualVariables, CRealRandomGeneration *pcRandomGeneration);

		double d_evaluate(CRealCoding *pcContext, CRealCoding *pcSolution0, CRealCoding *pcSolution1, uint16_t iVariable, vector<uint16_t> *pvResidualVariables);

		static void v_remove_from_variables(vector<uint16_t> *pvVariables, CGenePattern *pcOrderedGroup);

		void v_log_theta();

		void v_update_theta(double **ppdTheta);

		uint16_t i_N;
	};//class CRealFastInterdependenceSearchingDecomposition : public CDecomposition<CRealCoding, CRealCoding>


	template <class TFenotype, class TGenotype>
	CDecomposition<TGenotype, TFenotype> *pcGetDecompositon(CProblem<TGenotype, TFenotype> *pcProblem, CLog *pcLog, uint32_t iRandomSeed, istream *psSettings, CError *pcError, bool bIsObligatory = true);

	void vLogTheta(double **ppdTheta, uint16_t iNumberOfElements, CLog *pcLog);
}//namespace Decomposition

#endif//DECOMPOSITION_H