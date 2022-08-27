#ifndef STOP_CONDITION_H
#define STOP_CONDITION_H

#define STOP_CONDITION_ARGUMENT_TYPE "stop_condition_type"
#define STOP_CONDITION_ARGUMENT_TYPE_NONE "none"
#define STOP_CONDITION_ARGUMENT_TYPE_RUNNING_TIME "running_time"
#define STOP_CONDITION_ARGUMENT_TYPE_ITERATIONS "iterations"
#define STOP_CONDITION_ARGUMENT_TYPE_PROGRESS "progress"
#define STOP_CONDITION_ARGUMENT_TYPE_FFE "ffe"

#define STOP_CONDITION_ARGUMENT_TIME "stop_condition_time"

#define STOP_CONDITION_ARGUMENT_MAX_NUMBER_OF_ITERATIONS "stop_condition_max_number_of_iterations"

#define STOP_CONDITION_ARGUMENT_MAX_NUMBER_OF_ITERATIONS_WITHOUT_PROGRESS "stop_condition_max_number_of_iterations_without_progress"

#define STOP_CONDITION_ARGUMENT_MAX_NUMBER_OF_FFE "stop_condition_max_number_of_ffe"

#define STOP_CONDITION_NOW_UNSUPPORTED -1
#define STOP_CONDITION_MAX_UNSUPPORTED -1

#include "Error.h"
#include "Evaluation.h"
#include "Problem.h"

#include <cstdint>
#include <ctime>
#include <istream>

using namespace std;


enum EStopConditionType
{
	STOP_CONDITION_NONE = 0,
	STOP_CONDITION_RUNNING_TIME = 1,
	STOP_CONDITION_ITERATIONS = 2,
	STOP_CONDITION_PROGRESS = 3,
	STOP_CONDITION_FFE = 4
};//enum EStopConditionType


template <class TGenotype, class TFenotype>
class CStopCondition
{
public:
	CStopCondition(CEvaluation<TFenotype> *pcEvaluation);
	
	virtual ~CStopCondition();

	virtual CError eConfigure(istream *psSettings) { return CError(); };

	virtual bool bStop(time_t tStart, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual);

	virtual double dGetNow(time_t tStart, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual) = 0;
	virtual double dGetMax() = 0;

private:
	CEvaluation<TFenotype> *pc_evaluation;
};//class CStopCondition


template <class TGenotype, class TFenotype>
class CNoneStopCondition : public CStopCondition<TGenotype, TFenotype>
{
public:
	CNoneStopCondition() : CStopCondition(nullptr) { }

	virtual bool bStop(time_t tStart, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual) { return false; }

	virtual double dGetNow(time_t tStart, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual) { return STOP_CONDITION_NOW_UNSUPPORTED; }
	virtual double dGetMax() { return STOP_CONDITION_MAX_UNSUPPORTED; }
};//class CNoneStopCondition : public CStopCondition<TGenotype, TFenotype>


template <class TGenotype, class TFenotype>
class CIterationsStopCondition : public CStopCondition<TGenotype, TFenotype>
{
public:
	CIterationsStopCondition(CEvaluation<TFenotype> *pcEvaluation);

	virtual CError eConfigure(istream *psSettings);

	virtual bool bStop(time_t tStart, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual);

	virtual double dGetNow(time_t tStart, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual) { return (double)iIterationNumber; }
	virtual double dGetMax() { return (double)i_max_number_of_iterations; }

private:
	uint32_t i_max_number_of_iterations;
};//class CIterationsStopCondition : public CStopCondition<TGenotype, TFenotype>


template <class TGenotype, class TFenotype>
class CProgressStopCondition : public CStopCondition<TGenotype, TFenotype>
{
public:
	CProgressStopCondition(CEvaluation<TFenotype> *pcEvaluation);

	virtual CError eConfigure(istream *psSettings);

	virtual bool bStop(time_t tStart, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual);

	virtual double dGetNow(time_t tStart, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual) { return STOP_CONDITION_NOW_UNSUPPORTED; }
	virtual double dGetMax() { return STOP_CONDITION_MAX_UNSUPPORTED; }

private:
	CIndividual<TGenotype, TFenotype> *pc_last_best;
	uint32_t i_last_best_iteration;

	uint32_t i_max_number_of_iterations_without_progress;
};//class CNoneStopCondition : public CStopCondition<TGenotype, TFenotype>


template <class TGenotype, class TFenotype>
class CFFEStopCondition : public CStopCondition<TGenotype, TFenotype>
{
public:
	CFFEStopCondition(CEvaluation<TFenotype> *pcEvaluation);

	virtual CError eConfigure(istream *psSettings);

	virtual bool bStop(time_t tStart, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual);

	virtual double dGetNow(time_t tStart, uint32_t iIterationNumber, uint64_t iFFE, CIndividual<TGenotype, TFenotype> *pcBestIndividual) { return (double)iFFE; }
	virtual double dGetMax() { return (double)i_max_number_of_ffe; }

private:
	uint64_t i_max_number_of_ffe;
};//class CIterationsStopCondition : public CStopCondition<TGenotype, TFenotype>

#endif//STOP_CONDITION_H