#include "RealEvaluation.h"

#include "CommandParam.h"
#include "EnumCommandParam.h"
#include "EvaluationUtils.h"
#include "FloatVectorCommandParam.h"
#include "MathUtils.h"
#include "StringCommandParam.h"
#include "StringUtils.h"
#include "RandUtils.h"
#include "UIntCommandParam.h"
#include "UIntVectorCommandParam.h"
#include "VectorUtils.h"

#include <atlstr.h>
#include <cfloat>
#include <cmath>
#include <list>
#include <numeric>
#include <random>


CRealEvaluation::CRealEvaluation()
{
	v_init(DBL_MIN);
}//CRealEvaluation::CRealEvaluation()

CRealEvaluation::CRealEvaluation(uint16_t iNumberOfElements, double dMaxValue)
	: CEvaluation<CRealCoding>(iNumberOfElements, dMaxValue)
{
	v_init(DBL_MIN);
}//CRealEvaluation::CRealEvaluation(uint16_t iNumberOfElements, double dMaxValue)

CRealEvaluation::CRealEvaluation(uint16_t iNumberOfElements, double dMaxValue, double dEqualEpsilon)
	: CEvaluation<CRealCoding>(iNumberOfElements, dMaxValue)
{
	v_init(dEqualEpsilon);
}//CRealEvaluation::CRealEvaluation(uint16_t iNumberOfElements, double dMaxValue, double dEqualEpsilon)

CRealEvaluation::~CRealEvaluation()
{
	v_clear_params();
}//CRealEvaluation::~CRealEvaluation()

CError CRealEvaluation::eConfigure(istream *psSettings)
{
	v_clear_params();

	CError c_error = CEvaluation<CRealCoding>::eConfigure(psSettings);

	if (!c_error)
	{
		CDoubleCommandParam p_epsilon(EVALUATION_REAL_ARGUMENT_EPSILON, 0, DBL_MAX, EVALUATION_REAL_ARGUMENT_EPSILON_DEFAULT_VALUE);
		d_epsilon = p_epsilon.dGetValue(psSettings, &c_error);
	}//if (!c_error)

	return c_error;
}//CError CRealEvaluation::eConfigure(istream *psSettings)

CRealCoding * CRealEvaluation::pcCreateSampleFenotype()
{
	return new CRealCoding(i_number_of_elements, nullptr, pd_min_values, pd_max_values, d_equal_epsilon);
}//CRealCoding * CRealEvaluation::pcCreateSampleFenotype()

void CRealEvaluation::v_init(double dEqualEpsilon)
{
	d_equal_epsilon = dEqualEpsilon;

	pd_min_values = nullptr;
	pd_max_values = nullptr;
}//void CRealEvaluation::v_init(double dEqualEpsilon)

void CRealEvaluation::v_clear_params()
{
	delete pd_min_values;
	delete pd_max_values;

	v_init(d_equal_epsilon);
}//void CRealEvaluation::v_clear_params()


CRealCEC2013LSGOEvaluation::CRealCEC2013LSGOEvaluation()
{
	pc_benchmark = nullptr;
}//CRealCEC2013LSGOEvaluation::CRealCEC2013LSGOEvaluation()

CRealCEC2013LSGOEvaluation::~CRealCEC2013LSGOEvaluation()
{
	delete pc_benchmark;
}//CRealCEC2013LSGOEvaluation::~CRealCEC2013LSGOEvaluation()

CError CRealCEC2013LSGOEvaluation::eConfigure(istream *psSettings)
{
	delete pc_benchmark;
	pc_benchmark = nullptr;

	CError c_error = CRealEvaluation::eConfigure(psSettings);

	if (!c_error)
	{
		CUIntCommandParam p_function_number(EVALUATION_REAL_CEC2013_LSGO_ARGUMENT_FUNCTION_NUMBER,
			EVALUATION_REAL_CEC2013_LSGO_ARGUMENT_FUNCTION_NUMBER_MIN, EVALUATION_REAL_CEC2013_LSGO_ARGUMENT_FUNCTION_NUMBER_MAX);

		uint8_t i_function_number = (uint8_t)p_function_number.iGetValue(psSettings, &c_error);

		if (!c_error)
		{
			pc_benchmark = generateFuncObj((int)i_function_number);
		}//if (!c_error)
	}//if (!c_error)

	if (!c_error)
	{
		i_number_of_elements = (uint16_t)pc_benchmark->getDimension();
		d_max_value = 0;

		pd_min_values = new double[i_number_of_elements];
		pd_max_values = new double[i_number_of_elements];

		for (uint16_t i = 0; i < i_number_of_elements; i++)
		{
			*(pd_min_values + i) = (double)pc_benchmark->getMinX();
			*(pd_max_values + i) = (double)pc_benchmark->getMaxX();
		}//for (uint16_t i = 0; i < i_number_of_elements; i++)
	}//if (!c_error)

	return c_error;
}//CError CRealCEC2013LSGOEvaluation::eConfigure(istream *psSettings)

double CRealCEC2013LSGOEvaluation::d_evaluate(CRealCoding *pcFenotype, uint16_t iShift)
{
	return -(pc_benchmark->compute(pcFenotype->pdGetValues() + iShift));
}//double CRealCEC2013LSGOEvaluation::d_evaluate(CRealCoding *pcFenotype, uint16_t iShift)


double CRealCEC2013LSGOSqrtEvaluation::d_evaluate(CRealCoding *pcFenotype, uint16_t iShift)
{
	return -sqrt(-CRealCEC2013LSGOEvaluation::d_evaluate(pcFenotype, iShift));
}//double CRealCEC2013LSGOSqrtEvaluation::d_evaluate(CRealCoding *pcFenotype, uint16_t iShift)


double CRealCEC2013LSGOSquareEvaluation::d_evaluate(CRealCoding *pcFenotype, uint16_t iShift)
{
	double d_sqrt_val = CRealCEC2013LSGOEvaluation::d_evaluate(pcFenotype, iShift);

	return -d_sqrt_val * d_sqrt_val;
}//double CRealCEC2013LSGOSquareEvaluation::d_evaluate(CRealCoding *pcFenotype, uint16_t iShift)


CRealComputerNetworkEvaluation::~CRealComputerNetworkEvaluation()
{
	v_clear();
}//CRealComputerNetworkEvaluation::~CRealComputerNetworkEvaluation()

CError CRealComputerNetworkEvaluation::eConfigure(istream *psSettings)
{
	v_clear();

	CError c_error = CRealEvaluation::eConfigure(psSettings);

	uint32_t i_min_link_capacity = 0;
	uint32_t i_max_link_capacity = 0;

	uint32_t i_min_demand_value = 0;
	uint32_t i_max_demand_value = 0;

	uint32_t i_evaluation_seed = 0;
	uint32_t i_link_capacity_seed = 0;

	ofstream f_problem_definition;

	if (!c_error)
	{
		CUIntCommandParam pc_min_link_capacity("min_link_capacity", 1, UINT32_MAX);
		i_min_link_capacity = pc_min_link_capacity.iGetValue(psSettings, &c_error);
	}//if (!c_error)

	if (!c_error)
	{
		CUIntCommandParam pc_max_link_capacity("max_link_capacity");
		i_max_link_capacity = pc_max_link_capacity.iGetValue(psSettings, &c_error);

		if (!c_error && i_min_link_capacity > i_max_link_capacity)
		{
			c_error.vSetError(CError::iERROR_CODE_SYSTEM_ARGUMENT_WRONG_VALUE, "max_link_capacity must be greater than or equal to min_link_capacity");
		}//if (!c_error && i_min_link_capacity > i_max_link_capacity)
	}//if (!c_error)

	if (!c_error)
	{
		CUIntCommandParam pc_min_demand_value("min_demand_value", 1, UINT32_MAX);
		i_min_demand_value = pc_min_demand_value.iGetValue(psSettings, &c_error);
	}//if (!c_error)

	if (!c_error)
	{
		CUIntCommandParam pc_max_demand_value("max_demand_value");
		i_max_demand_value = pc_max_demand_value.iGetValue(psSettings, &c_error);

		if (!c_error && i_min_demand_value > i_max_demand_value)
		{
			c_error.vSetError(CError::iERROR_CODE_SYSTEM_ARGUMENT_WRONG_VALUE, "max_demand_value must be greater than or equal to min_demand_value");
		}//if (!c_error && i_min_demand_value > i_max_demand_value)
	}//if (!c_error)

	if (!c_error)
	{
		CUIntCommandParam pc_number_of_demands("number_of_demands", 1, UINT16_MAX);
		i_number_of_demands = (uint16_t)pc_number_of_demands.iGetValue(psSettings, &c_error);
	}//if (!c_error)

	if (!c_error)
	{
		CUIntCommandParam pc_number_of_routes_per_demand("number_of_routes_per_demand", 1, UINT16_MAX);
		i_number_of_routes_per_demand = (uint16_t)pc_number_of_routes_per_demand.iGetValue(psSettings, &c_error);
	}//if (!c_error)

	if (!c_error)
	{
		CUIntCommandParam pc_evaluation_seed("evaluation_seed");
		i_evaluation_seed = pc_evaluation_seed.iGetValue(psSettings, &c_error);
	}//if (!c_error)

	if (!c_error)
	{
		CUIntCommandParam pc_link_capacity_seed("link_capacity_seed", (uint32_t)0);
		i_link_capacity_seed = pc_link_capacity_seed.iGetValue(psSettings, &c_error);
	}//if (!c_error)

	if (!c_error)
	{
		CFilePathCommandParam pc_network_topology_file_path("network_topology_file_path");
		CString s_network_topology_file_path = pc_network_topology_file_path.sGetValue(psSettings, &c_error);

		if (!c_error)
		{
			mt19937 c_link_capacity_rand_gen(i_link_capacity_seed);
			uniform_int_distribution<uint32_t> c_link_capacity_distr(i_min_link_capacity, i_max_link_capacity);

			mt19937 c_demand_value_rand_gen(i_evaluation_seed);
			uniform_int_distribution<uint32_t> c_demand_value_distr(i_min_demand_value, i_max_demand_value);

			ifstream f_network_topology(s_network_topology_file_path);

			uint32_t i_number_of_nodes;
			f_network_topology >> i_number_of_nodes;

			vector<vector<uint32_t>> v_adjacency_list(i_number_of_nodes);

			unordered_map<pair<uint32_t, uint32_t>, CComputerNetworkLink*, SHashPair> m_all_links;

			for (uint32_t i_node = 0; i_node < i_number_of_nodes; i_node++)
			{
				uint32_t i_node_index;
				f_network_topology >> i_node_index;

				uint32_t i_number_of_node_links;
				f_network_topology >> i_number_of_node_links;

				for (uint32_t i_node_link = 0; i_node_link < i_number_of_node_links; i_node_link++)
				{
					uint32_t i_node_to_index;
					f_network_topology >> i_node_to_index;

					uint32_t i_node_original_capacity;
					f_network_topology >> i_node_original_capacity;

					v_adjacency_list[i_node_index - 1].push_back(i_node_to_index - 1);

					pair<uint32_t, uint32_t> p_link(min(i_node_index - 1, i_node_to_index - 1), max(i_node_index - 1, i_node_to_index - 1));

					if (m_all_links.count(p_link) == 0)
					{
						m_all_links[p_link] = new CComputerNetworkLink(0);
					}//if (m_all_links.count(p_link) == 0)
				}//for (uint32_t i_node_link = 0; i_node_link < i_number_of_node_links; i_node_link++)
			}//for (uint32_t i_node = 0; i_node < i_number_of_nodes; i_node++)

			f_network_topology.close();

			vector<pair<uint32_t, uint32_t>> v_possible_connections;
			v_possible_connections.reserve(i_number_of_nodes * (i_number_of_nodes - 1) / 2);

			for (uint32_t i = 0; i + 1 < i_number_of_nodes; i++)
			{
				for (uint32_t j = i + 1; j < i_number_of_nodes; j++)
				{
					v_possible_connections.push_back(pair<uint32_t, uint32_t>(i, j));
				}//for (uint32_t j = i + 1; j < i_number_of_nodes; j++)
			}//for (uint32_t i = 0; i + 1 < i_number_of_nodes; i++)

			for (uint16_t i_demand = 0; i_demand < (uint16_t)v_possible_connections.size(); i_demand++)
			{
				uint32_t i_node_from = v_possible_connections[i_demand % v_possible_connections.size()].first;
				uint32_t i_node_to = v_possible_connections[i_demand % v_possible_connections.size()].second;

				v_add_demands_many(1 * i_demand, 1 * (i_demand + 1) - 1, 0, i_node_from, i_node_to, &v_adjacency_list, &m_all_links);
			}//for (uint16_t i_demand = 0; i_demand < (uint16_t)v_possible_connections.size(); i_demand++)

			vector< CComputerNetworkLink*> v_all_links;

			v_all_links.reserve(m_all_links.size());

			for (unordered_map<pair<uint32_t, uint32_t>, CComputerNetworkLink*, SHashPair>::iterator i_it = m_all_links.begin(); i_it != m_all_links.end(); i_it++)
			{
				v_all_links.push_back(i_it->second);
			}//for (unordered_map<CString, CCommandExecution*>::iterator i_it = m_commands.begin(); i_it != m_commands.end(); i_it++)

			sort(v_all_links.begin(), v_all_links.end(), [](CComputerNetworkLink *pcLink0, CComputerNetworkLink *pcLink1)
			{
				return pcLink0->iGetNumberOfDemands() > pcLink1->iGetNumberOfDemands();
			});//sort(v_all_links.begin(), v_all_links.end(), [](CComputerNetworkLink *pcLink0, CComputerNetworkLink *pcLink1)

			vector<double> v_link_capacities;

			v_link_capacities.reserve(v_all_links.size());

			for (uint32_t i_link = 0; i_link < (uint32_t)v_all_links.size(); i_link++)
			{
				v_link_capacities.push_back(c_link_capacity_distr(c_link_capacity_rand_gen));
			}//for (uint32_t i_link = 0; i_link < (uint32_t)v_all_links.size(); i_link++)

			sort(v_link_capacities.begin(), v_link_capacities.end(), greater<double>());

			for (uint32_t i_link = 0; i_link < (uint32_t)v_all_links.size(); i_link++)
			{
				v_all_links[i_link]->vSetCapacity(v_link_capacities[i_link]);
				v_all_links[i_link]->vClearDemands();
			}//for (uint32_t i_link = 0; i_link < (uint32_t)v_all_links.size(); i_link++)

			mt19937 c_conn_rand_gen(i_evaluation_seed);

			shuffle(v_possible_connections.begin(), v_possible_connections.end(), c_conn_rand_gen);

			for (uint16_t i_demand = 0; i_demand < i_number_of_demands; i_demand++)
			{
				uint32_t i_node_from = v_possible_connections[i_demand % v_possible_connections.size()].first;
				uint32_t i_node_to = v_possible_connections[i_demand % v_possible_connections.size()].second;

				v_add_demands_many(i_number_of_routes_per_demand * i_demand, i_number_of_routes_per_demand * (i_demand + 1) - 1, c_demand_value_distr(c_demand_value_rand_gen),
					i_node_from, i_node_to, &v_adjacency_list, &m_all_links);
			}//for (uint16_t i_demand = 0; i_demand < i_number_of_demands; i_demand++)

			s_problem_definition_file_path = s_network_topology_file_path;
			
			s_problem_definition_file_path.AppendFormat("_%d", i_number_of_demands);
			s_problem_definition_file_path.AppendFormat("_%d", i_number_of_routes_per_demand);
			s_problem_definition_file_path.AppendFormat("_%d", i_min_link_capacity);
			s_problem_definition_file_path.AppendFormat("_%d", i_max_link_capacity);
			s_problem_definition_file_path.AppendFormat("_%d", i_min_demand_value);
			s_problem_definition_file_path.AppendFormat("_%d", i_max_demand_value);
			s_problem_definition_file_path.AppendFormat("_%d", i_link_capacity_seed);
			s_problem_definition_file_path.AppendFormat("_%d", i_evaluation_seed);
			s_problem_definition_file_path.Append(".def");

			f_problem_definition.open(s_problem_definition_file_path, ofstream::out | ofstream::ate);

			f_problem_definition << i_number_of_demands << endl;
			f_problem_definition << i_number_of_routes_per_demand << endl;

			for (unordered_map<pair<uint32_t, uint32_t>, CComputerNetworkLink*, SHashPair>::iterator i_it = m_all_links.begin(); i_it != m_all_links.end(); i_it++)
			{
				if (i_it->second->iGetNumberOfDemands() > 0)
				{
					v_considered_links.push_back(i_it->second);
				}//if (i_it->second->iGetNumberOfDemands() > 0)
				else
				{
					delete i_it->second;
				}//else if (i_it->second->iGetNumberOfDemands() > 0)
			}//for (unordered_map<CString, CCommandExecution*>::iterator i_it = m_commands.begin(); i_it != m_commands.end(); i_it++)

			f_problem_definition << v_considered_links.size() << endl;

			for (size_t i_link = 0; i_link < v_considered_links.size(); i_link++)
			{
				f_problem_definition << v_considered_links[i_link]->dGetCapacity();

				for (uint16_t i_demand = 0; i_demand < v_considered_links[i_link]->iGetNumberOfDemands(); i_demand++)
				{
					pair<uint16_t, double> *pp_demand = v_considered_links[i_link]->ppGetDemand(i_demand);

					f_problem_definition << " " << pp_demand->first << " " << pp_demand->second;
				}//for (uint16_t i_demand = 0; i_demand < v_considered_links[i_link]->iGetNumberOfDemands(); i_demand++)

				f_problem_definition << endl;
			}//for (size_t i_link = 0; i_link < v_considered_links.size(); i_link++)

			f_problem_definition.close();
		}//if (!c_error)
	}//if (!c_error)

	if (!c_error)
	{
		i_number_of_elements = i_number_of_demands * (i_number_of_routes_per_demand - 1);

		d_max_value = 0;

		pd_min_values = new double[i_number_of_elements];
		pd_max_values = new double[i_number_of_elements];

		for (uint16_t i = 0; i < i_number_of_elements; i++)
		{
			*(pd_min_values + i) = 0.0;
			*(pd_max_values + i) = 1.0;
		}//for (uint16_t i = 0; i < i_number_of_elements; i++)
	}//if (!c_error)

	return c_error;
}//CError CRealComputerNetworkEvaluation::eConfigure(istream *psSettings)

double CRealComputerNetworkEvaluation::d_evaluate(CRealCoding *pcFenotype, uint16_t iShift)
{
	vector<double> v_normalized_fenotype;

	v_normalized_fenotype.resize(i_number_of_demands * i_number_of_routes_per_demand);

	vector<uint16_t> v_indices(i_number_of_routes_per_demand - 1);
	iota(v_indices.begin(), v_indices.end(), 0);

	vector<double> v_percentages;
	v_percentages.reserve(i_number_of_routes_per_demand - 1);

	for (uint16_t i_demand = 0; i_demand < i_number_of_demands; i_demand++)
	{
		v_percentages.clear();

		for (uint16_t i_route = 0; i_route + 1 < i_number_of_routes_per_demand; i_route++)
		{
			v_percentages.push_back(pcFenotype->pdGetValues()[i_demand * (i_number_of_routes_per_demand - 1) + i_route + iShift]);
		}//for (uint16_t i_route = 0; i_route < i_number_of_routes_per_demand; i_route++)

		sort(v_indices.begin(), v_indices.end(), [&](uint16_t iIndex0, uint16_t iIndex1)
		{
			if (v_percentages[iIndex0] == v_percentages[iIndex1])
			{
				return iIndex0 < iIndex1;
			}//if (v_percentages[iIndex0] == v_percentages[iIndex1])

			return v_percentages[iIndex0] < v_percentages[iIndex1];
		});//sort(v_indices.begin(), v_indices.end(), [&](uint16_t iIndex0, uint16_t iIndex1)

		double d_remaining_percentage = 1.0;

		for (size_t i = 0; i < v_indices.size(); i++)
		{
			double d_used_percentage = d_remaining_percentage * v_percentages[v_indices[i]];

			v_normalized_fenotype[i_demand * i_number_of_routes_per_demand + v_indices[i]] = d_used_percentage;

			d_remaining_percentage -= d_used_percentage;
		}//for (size_t i = 0; i < v_indices.size(); i++)

		v_normalized_fenotype[(i_demand + 1) * i_number_of_routes_per_demand - 1] = d_remaining_percentage;
	}//for (uint16_t i_demand = 0; i_demand < i_number_of_demands; i_demand++)

	double d_fitness_value = 0;

	for (uint16_t i = 0; i < (uint16_t)v_considered_links.size() && d_fitness_value > -DBL_MAX; i++)
	{
		CComputerNetworkLink *pc_link = v_considered_links[i];

		double d_link_fitness_value = pc_link->dCalculateFitness(&v_normalized_fenotype);

		if (d_link_fitness_value == -DBL_MAX)
		{
			d_fitness_value = d_link_fitness_value;
		}//if (d_link_fitness_value == -DBL_MAX)
		else
		{
			d_fitness_value += d_link_fitness_value;
		}//else if (d_link_fitness_value == -DBL_MAX)
	}//for (uint16_t i = 0; i < (uint16_t)v_considered_links.size() && d_fitness_value > -DBL_MAX; i++)

	return d_fitness_value;
}//double CRealComputerNetworkEvaluation::d_evaluate(CRealCoding *pcFenotype, uint16_t iShift)

void CRealComputerNetworkEvaluation::v_clear()
{
	VectorUtils::vDeleteElementsAndClear(&v_considered_links);
}//void CRealComputerNetworkEvaluation::v_clear()

void CRealComputerNetworkEvaluation::v_add_demands_many(uint16_t iDemandIndexFrom, uint16_t iDemandIndexTo, double dDemandValue, uint32_t iNodeFrom, uint32_t iNodeTo, vector<vector<uint32_t>> *pvAdjacencyList, unordered_map<pair<uint32_t, uint32_t>, CComputerNetworkLink*, SHashPair> *pmAllLinks)
{
	list<CBFSNode*> l_queue;
	vector<CBFSNode*> v_all_nodes;

	CBFSNode c_first_node(iNodeFrom, nullptr);

	l_queue.push_back(&c_first_node);

	uint16_t i_demand_index = iDemandIndexFrom;

	while (i_demand_index <= iDemandIndexTo)
	{
		CBFSNode *pc_node = l_queue.front();
		vector<uint32_t> *pv_adjacency = &pvAdjacencyList->at(pc_node->iGetIndex());

		for (size_t i = 0; i < pv_adjacency->size() && i_demand_index <= iDemandIndexTo; i++)
		{
			if (!pc_node->bAlreadyVisited(pv_adjacency->at(i)))
			{
				pair<uint32_t, uint32_t> p_link(min(pc_node->iGetIndex(), pv_adjacency->at(i)), max(pc_node->iGetIndex(), pv_adjacency->at(i)));

				if (pv_adjacency->at(i) == iNodeTo)
				{
					CBFSNode c_node(pv_adjacency->at(i), pc_node);
					CBFSNode *pc_adjacent_node = &c_node;

					while (pc_adjacent_node->pcGetFrom())
					{
						pair<uint32_t, uint32_t> p_link(min(pc_adjacent_node->iGetIndex(), pc_adjacent_node->pcGetFrom()->iGetIndex()), max(pc_adjacent_node->iGetIndex(), pc_adjacent_node->pcGetFrom()->iGetIndex()));

						pmAllLinks->at(p_link)->vAddDemand(i_demand_index, dDemandValue);

						pc_adjacent_node = pc_adjacent_node->pcGetFrom();
					}//while (pc_node->pcGetFrom())

					i_demand_index++;
				}//if (pv_adjacency->at(i) == iNodeTo)
				else
				{
					CBFSNode *pc_adjacent_node = new CBFSNode(pv_adjacency->at(i), pc_node);

					l_queue.push_back(pc_adjacent_node);
					v_all_nodes.push_back(pc_adjacent_node);
				}//else if (pv_adjacency->at(i) == iNodeTo)
			}//if (!pc_node->bAlreadyVisited(pv_adjacency->at(i)))
		}//for (size_t i = 0; i < pv_adjacency->size() && i_demand_index <= iDemandIndexTo; i++)

		l_queue.pop_front();
	}//while (i_demand_index <= iDemandIndexTo)

	VectorUtils::vDeleteElements(&v_all_nodes);
}//void CRealComputerNetworkEvaluation::v_add_demands_many(uint16_t iDemandIndexFrom, uint16_t iDemandIndexTo, double dDemandValue, uint32_t iNodeFrom, uint32_t iNodeTo, vector<vector<uint32_t>> *pvAdjacencyList, unordered_map<pair<uint32_t, uint32_t>, CComputerNetworkLink*, SHashPair> *pmAllLinks)

double CComputerNetworkLink::dCalculateFitness(vector<double> *pvFenotype)
{
	double d_sum_demand_value = 0;

	for (uint16_t i = 0; i < (uint16_t)v_demand_values.size(); i++)
	{
		d_sum_demand_value += pvFenotype->at(v_demand_values[i].first) * v_demand_values[i].second;
	}//for (uint16_t i = 0; i < v_demand_values.size(); i++)

	double d_fitness;

	if (d_sum_demand_value >= d_capacity)
	{
		d_fitness = DBL_MAX;
	}//if (d_sum_demand_value >= d_capacity)
	else
	{
		d_fitness = d_sum_demand_value / (d_capacity - d_sum_demand_value);
	}//else if (d_sum_demand_value >= d_capacity)

	return -d_fitness;
}//double CComputerNetworkLink::dCalculateFitness(vector<double> *pvFenotype)

CBFSNode::CBFSNode(uint32_t iIndex, CBFSNode *pcFrom)
{
	i_index = iIndex;
	pc_from = pcFrom;

	if (pcFrom)
	{
		s_already_visited_nodes = pcFrom->s_already_visited_nodes;
	}//if (pcFrom)

	s_already_visited_nodes.insert(iIndex);
}//CBFSNode::CBFSNode(uint32_t iIndex, CBFSNode *pcFrom)