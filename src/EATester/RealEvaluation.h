#ifndef REAL_EVALUATION_H
#define REAL_EVALUATION_H

#define EVALUATION_REAL_ARGUMENT_EPSILON "evaluation_epsilon"
#define EVALUATION_REAL_ARGUMENT_EPSILON_DEFAULT_VALUE 1e-8

#define EVALUATION_REAL_CEC2013_ARGUMENT_FUNCTION_NUMBER "evaluation_function_number"
#define EVALUATION_REAL_CEC2013_ARGUMENT_FUNCTION_NUMBER_MIN 1
#define EVALUATION_REAL_CEC2013_ARGUMENT_FUNCTION_NUMBER_MAX 20

#define EVALUATION_REAL_CEC2013_LSGO_ARGUMENT_FUNCTION_NUMBER "evaluation_function_number"
#define EVALUATION_REAL_CEC2013_LSGO_ARGUMENT_FUNCTION_NUMBER_MIN 1
#define EVALUATION_REAL_CEC2013_LSGO_ARGUMENT_FUNCTION_NUMBER_MAX 15


#include "Error.h"
#include "Evaluation.h"
#include "PairUtils.h"
#include "RealCoding.h"

#include "../CEC2013LSGO/Header.h"

#include <cstdint>
#include <istream>
#include <unordered_map>
#include <unordered_set>

using namespace PairUtils;

using namespace std;


class CRealEvaluation : public CEvaluation<CRealCoding>
{
public:
	CRealEvaluation();
	CRealEvaluation(uint16_t iNumberOfElements, double dMaxValue);
	CRealEvaluation(uint16_t iNumberOfElements, double dMaxValue, double dEqualEpsilon);

	virtual ~CRealEvaluation();

	virtual CError eConfigure(istream *psSettings);

	virtual CRealCoding *pcCreateSampleFenotype();

	virtual bool bIsMaxValue(double dValue) { return dValue >= d_max_value - d_epsilon; };

	double *pdGetMinValues() { return pd_min_values; };
	double *pdGetMaxValues() { return pd_max_values; };

	double dGetEqualEpsilon() { return d_equal_epsilon; };

protected:
	void v_clear_params();

	double d_equal_epsilon;

	double *pd_min_values;
	double *pd_max_values;

	double d_epsilon;

private:
	void v_init(double dEqualEpsilon);
};//class CRealEvaluation : public CEvaluation<CRealCoding>


class CRealCEC2013LSGOEvaluation : public CRealEvaluation
{
public:
	CRealCEC2013LSGOEvaluation();

	virtual ~CRealCEC2013LSGOEvaluation();

	virtual CError eConfigure(istream *psSettings);

	uint8_t iGetFunctionNumber() { return pc_benchmark->getID(); }

protected:
	virtual double d_evaluate(CRealCoding *pcFenotype, uint16_t iShift);

private:
	Benchmarks *pc_benchmark;
};//class CRealCEC2013LSGOEvaluation : public CRealEvaluation


class CRealCEC2013LSGOSqrtEvaluation : public CRealCEC2013LSGOEvaluation
{
protected:
	virtual double d_evaluate(CRealCoding *pcFenotype, uint16_t iShift);
};//class CRealCEC2013LSGOSqrtEvaluation : public CRealCEC2013LSGOEvaluation


class CRealCEC2013LSGOSquareEvaluation : public CRealCEC2013LSGOEvaluation
{
protected:
	virtual double d_evaluate(CRealCoding *pcFenotype, uint16_t iShift);
};//class CRealCEC2013LSGOSquareEvaluation : public CRealCEC2013LSGOEvaluation


class CComputerNetworkLink;

class CRealComputerNetworkEvaluation : public CRealEvaluation
{
public:
	virtual ~CRealComputerNetworkEvaluation();

	virtual CError eConfigure(istream *psSettings);

	CString sGetProblemDefinitionFilePath() { return s_problem_definition_file_path; };

protected:
	virtual double d_evaluate(CRealCoding *pcFenotype, uint16_t iShift);

private:
	void v_clear();

	static void v_add_demands_many(uint16_t iDemandIndexFrom, uint16_t iDemandIndexTo, double dDemandValue, uint32_t iNodeFrom, uint32_t iNodeTo, vector<vector<uint32_t>> *pvAdjacencyList,
		unordered_map<pair<uint32_t, uint32_t>, CComputerNetworkLink*, SHashPair> *pmAllLinks);

	uint16_t i_number_of_demands;
	uint16_t i_number_of_routes_per_demand;

	CString s_problem_definition_file_path;

	vector<CComputerNetworkLink*> v_considered_links;
};//class CRealComputerNetworkEvaluation : public CRealEvaluation

class CComputerNetworkLink
{
public:
	CComputerNetworkLink(double dCapacity) { d_capacity = dCapacity; }

	void vAddDemand(uint16_t iDemandIndex, double dDemandValue) { v_demand_values.push_back(pair<uint16_t, double>(iDemandIndex, dDemandValue)); };
	void vClearDemands() { v_demand_values.clear(); };

	double dCalculateFitness(vector<double> *pvFenotype);

	double dGetCapacity() { return d_capacity; };
	uint16_t iGetNumberOfDemands() { return (uint16_t)v_demand_values.size(); };
	pair<uint16_t, double> *ppGetDemand(uint16_t iIndex) { return &v_demand_values[iIndex]; };

	void vSetCapacity(double dCapacity) { d_capacity = dCapacity; };

private:
	double d_capacity;
	vector<pair<uint16_t, double>> v_demand_values;
};//class CComputerNetworkLink

class CBFSNode
{
public:
	CBFSNode(uint32_t iIndex, CBFSNode *pcFrom);

	bool bAlreadyVisited(uint32_t iIndex) { return s_already_visited_nodes.count(iIndex) > 0; };

	uint32_t iGetIndex() { return i_index; };
	CBFSNode *pcGetFrom() { return pc_from; };

private:
	uint32_t i_index;
	CBFSNode *pc_from;
	unordered_set<uint32_t> s_already_visited_nodes;
};//class CBFSNode

#endif//REAL_EVALUATION_H