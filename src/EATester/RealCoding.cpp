#include "RealCoding.h"

#include <unordered_set>

using namespace std;

CRealCoding::CRealCoding(uint16_t iNumberOfDimensions, double *pdValues, double *pdMinValues, double *pdMaxValues, double dEqualEpsilon)
{
	b_own_values = true;
	b_own_min_max_values = false;

	i_number_of_dimensions = iNumberOfDimensions;
	
	pd_values = pdValues;

	pd_min_values = pdMinValues;
	pd_max_values = pdMaxValues;

	d_equal_epsilon = dEqualEpsilon;
}//CRealCoding::CRealCoding(uint16_t iNumberOfDimensions, double *pdValues, double *pdMinValues, double *pdMaxValues, double dEqualEpsilon)

CRealCoding::CRealCoding(CRealCoding *pcOther)
{
	b_own_values = true;
	b_own_min_max_values = false;

	i_number_of_dimensions = pcOther->i_number_of_dimensions;

	if (pcOther->pd_values)
	{
		pd_values = new double[pcOther->i_number_of_dimensions];

		for (uint32_t i = 0; i < i_number_of_dimensions; i++)
		{
			*(pd_values + i) = *(pcOther->pd_values + i);
		}//for (uint32_t i = 0; i < i_number_of_dimensions; i++)
	}//if (pcOther->pd_values)
	else
	{
		pd_values = nullptr;
	}//else if (pcOther->pd_values)

	pd_min_values = pcOther->pd_min_values;
	pd_max_values = pcOther->pd_max_values;

	d_equal_epsilon = pcOther->d_equal_epsilon;
}//CRealCoding::CRealCoding(CRealCoding *pcOther)

CRealCoding::~CRealCoding()
{
	if (b_own_values)
	{
		delete pd_values;
	}//if (b_own_values)

	if (b_own_min_max_values)
	{
		delete pd_min_values;
		delete pd_max_values;
	}//if (b_own_min_max_values)
}//CRealCoding::~CRealCoding()

#include <iostream>
using namespace std;

void CRealCoding::vMerge(CRealCoding *pcOther)
{
	if (pd_values && pcOther->pd_values)
	{
		double *pd_merged_values = new double[i_number_of_dimensions + pcOther->i_number_of_dimensions];

		for (uint16_t i = 0; i < i_number_of_dimensions; i++)
		{
			*(pd_merged_values + i) = *(pd_values + i);
		}//for (uint16_t i = 0; i < i_number_of_dimensions; i++)

		for (uint16_t i = 0; i < pcOther->i_number_of_dimensions; i++)
		{
			*(pd_merged_values + i + i_number_of_dimensions) = *(pcOther->pd_values + i);
		}//for (uint16_t i = 0; i < pcOther->i_number_of_dimensions; i++)

		delete pd_values;
		pd_values = pd_merged_values;
	}//if (pd_values && pcOther->pd_values)

	//TODO: delete
	double *pd_merged_min_values = new double[i_number_of_dimensions + pcOther->i_number_of_dimensions];
	double *pd_merged_max_values = new double[i_number_of_dimensions + pcOther->i_number_of_dimensions];

	for (uint16_t i = 0; i < i_number_of_dimensions; i++)
	{
		*(pd_merged_min_values + i) = *(pd_min_values + i);
		*(pd_merged_max_values + i) = *(pd_max_values + i);
	}//for (uint16_t i = 0; i < i_number_of_dimensions; i++)

	for (uint16_t i = 0; i < pcOther->i_number_of_dimensions; i++)
	{
		*(pd_merged_min_values + i + i_number_of_dimensions) = *(pcOther->pd_min_values + i);
		*(pd_merged_max_values + i + i_number_of_dimensions) = *(pcOther->pd_max_values + i);
	}//for (uint16_t i = 0; i < pcOther->i_number_of_dimensions; i++)
	
	if (b_own_min_max_values)
	{
		delete pd_min_values;
		delete pd_max_values;
	}//if (b_own_min_max_values)

	b_own_min_max_values = true;

	pd_min_values = pd_merged_min_values;
	pd_max_values = pd_merged_max_values;

	i_number_of_dimensions += pcOther->i_number_of_dimensions;
	d_equal_epsilon = min(d_equal_epsilon, pcOther->d_equal_epsilon);
}//void CRealCoding::vMerge(CRealCoding *pcOther)

#include <iostream>

#include "RandUtils.h"

void CRealCoding::vRepair()
{
	for (uint16_t i = 0; i < i_number_of_dimensions; i++)
	{
		if (*(pd_values + i) > *(pd_max_values + i))
		{
			//cout << "repair max " << *(pd_values + i) << endl;
			*(pd_values + i) = *(pd_max_values + i);
			//*(pd_values + i) = RandUtils::dRandNumber(*(pd_min_values + i), *(pd_max_values + i));
		}//if (*(pcGenotype->pdGetValues() + i) > *(pcGenotype->pdGetMaxValues() + i))
		else if (*(pd_values + i) < *(pd_min_values + i))
		{
			//cout << "repair min " << *(pd_values + i) << endl;
			*(pd_values + i) = *(pd_min_values + i);
			//*(pd_values + i) = RandUtils::dRandNumber(*(pd_min_values + i), *(pd_max_values + i));
		}//else if (*(pcGenotype->pdGetValues() + i) < *(pcGenotype->pdGetMinValues() + i))
	}//for (uint16_t i = 0; i < i_number_of_dimensions; i++)
}//void CRealCoding::vRepair()

double CRealCoding::dComputeDistance(CRealCoding *pcOther)
{
	double d_distance = 0;

	double d_one_variable_distance;

	for (uint16_t i = 0; i < i_number_of_dimensions; i++)
	{
		d_one_variable_distance = *(pd_values + i) - *(pcOther->pd_values + i);
		d_distance += d_one_variable_distance * d_one_variable_distance;
	}//for (uint16_t i = 0; i < i_number_of_dimensions; i++)

	return d_distance;
}//double CRealCoding::dComputeDistance(CRealCoding *pcOther)

double CRealCoding::dComputeDistance(CRealCoding *pcOther, CGenePattern *pcGenePattern)
{
	double d_distance = 0;

	double d_one_variable_distance;

	uint16_t i_gene_index;

	for (uint16_t i = 0; i < pcGenePattern->iGetSize(); i++)
	{
		i_gene_index = *(pcGenePattern->piGetPattern() + i);

		d_one_variable_distance = *(pd_values + i_gene_index) - *(pcOther->pd_values + i_gene_index);
		d_distance += d_one_variable_distance * d_one_variable_distance;
	}//for (uint16_t i = 0; i < pcGenePattern->iGetSize(); i++)

	return d_distance;
}//double CRealCoding::dComputeDistance(CRealCoding *pcOther, CGenePattern *pcGenePattern)

bool CRealCoding::bExceededMaxDistance(CRealCoding *pcOther, double dMaxDistance, double *pdDistance)
{
	double d_distance = 0;

	double d_one_variable_distance;

	for (uint16_t i = 0; i < i_number_of_dimensions && d_distance <= dMaxDistance; i++)
	{
		d_one_variable_distance = *(pd_values + i) - *(pcOther->pd_values + i);
		d_distance += d_one_variable_distance * d_one_variable_distance;
	}//for (uint16_t i = 0; i < i_number_of_dimensions && d_distance <= dMaxDistance; i++)

	bool b_exceeded = d_distance > dMaxDistance;

	if (!b_exceeded && pdDistance)
	{
		*pdDistance = d_distance;
	}//if (!b_exceeded && pdDistance)

	return b_exceeded;
}//bool CRealCoding::bExceededMaxDistance(CRealCoding *pcOther, double dMaxDistance, double *pdDistance)

bool CRealCoding::bExceededMaxDistance(CRealCoding *pcOther, CGenePattern *pcGenePattern, double dMaxDistance, double *pdDistance, bool bOnlyPattern)
{
	double d_distance = 0;

	double d_one_variable_distance;

	uint16_t i_gene_index;

	if (bOnlyPattern)
	{
		for (uint16_t i = 0; i < pcGenePattern->iGetSize() && d_distance <= dMaxDistance; i++)
		{
			i_gene_index = *(pcGenePattern->piGetPattern() + i);

			d_one_variable_distance = *(pd_values + i_gene_index) - *(pcOther->pd_values + i_gene_index);
			d_distance += d_one_variable_distance * d_one_variable_distance;
		}//for (uint16_t i = 0; i < pcGenePattern->iGetSize() && d_distance <= dMaxDistance; i++)
	}//if (bOnlyPattern)
	else
	{
		unordered_set<uint16_t> s_without_gene_indexes(pcGenePattern->iGetSize());

		for (uint16_t i = 0; i < pcGenePattern->iGetSize(); i++)
		{
			i_gene_index = *(pcGenePattern->piGetPattern() + i);
			s_without_gene_indexes.insert(i_gene_index);
		}//for (uint16_t i = 0; i < pcGenePattern->iGetSize(); i++)

		for (uint16_t i = 0; i < i_number_of_dimensions && d_distance < dMaxDistance; i++)
		{
			if (s_without_gene_indexes.count(i) == 0 || (uint16_t)s_without_gene_indexes.size() == i_number_of_dimensions)
			{
				d_one_variable_distance = *(pd_values + i) - *(pcOther->pd_values + i);
				d_distance += d_one_variable_distance * d_one_variable_distance;
			}//if (s_without_gene_indexes.count(i) == 0 || (uint16_t)s_without_gene_indexes.size() == i_number_of_dimensions)
		}//for (uint16_t i = 0; i < i_number_of_dimensions && d_distance < dMaxDistance; i++)

		s_without_gene_indexes.clear();
	}//else if (bOnlyPattern)

	bool b_exceeded = d_distance > dMaxDistance;

	if (!b_exceeded)
	{
		*pdDistance = d_distance;
	}//if (!b_exceeded)

	return b_exceeded;
}//bool CRealCoding::bExceededMaxDistance(CRealCoding *pcOther, CGenePattern *pcGenePattern, double dMaxDistance, double *pdDistance, bool bOnlyPattern)

bool CRealCoding::bHasSameParams(CRealCoding *pcOther)
{
	bool b_has_same_params = i_number_of_dimensions == pcOther->i_number_of_dimensions;

	if (b_has_same_params)
	{
		b_has_same_params = d_equal_epsilon == pcOther->d_equal_epsilon;
	}//if (b_has_same_params)

	for (uint16_t i = 0; i < i_number_of_dimensions && b_has_same_params; i++)
	{
		b_has_same_params = *(pd_min_values + i) == *(pcOther->pd_min_values + i);

		if (b_has_same_params)
		{
			b_has_same_params = *(pd_max_values + i) == *(pcOther->pd_max_values + i);
		}//if (b_has_same_params)
	}//for (uint16_t i = 0; i < i_number_of_dimensions && b_has_same_params; i++)

	return b_has_same_params;
}//bool CRealCoding::bHasSameParams(CRealCoding *pcOther)

void CRealCoding::vSetValues(double *pdValues, bool bOwnValues)
{
	if (b_own_values)
	{
		delete pd_values;
	}//if (b_own_values)

	pd_values = pdValues;
	b_own_values = bOwnValues;
}//void CRealCoding::vSetValues(double *pdValues, bool bOwnValues)

void CRealCoding::vSetMinMaxValues(double *pdMinValues, double *pdMaxValues, bool bOwnMinMaxValues)
{
	if (b_own_min_max_values)
	{
		delete pd_min_values;
		delete pd_max_values;
	}//if (b_own_min_max_values)

	pd_min_values = pdMinValues;
	pd_max_values = pdMaxValues;

	b_own_min_max_values = bOwnMinMaxValues;
}//void CRealCoding::vSetMinMaxValues(double *pdMinValues, double *pdMaxValues, bool bOwnMinMaxValues)

CString CRealCoding::sToString()
{
	CString s_result;

	s_result.Append("[");

	for (uint16_t i = 0; i < i_number_of_dimensions; i++)
	{
		s_result.AppendFormat("%f", *(pd_values + i));

		if (i < i_number_of_dimensions - 1)
		{
			s_result.Append(",");
		}//if (i < i_number_of_dimensions - 1)
	}//for (uint16_t i = 0; i < i_number_of_dimensions; i++)

	s_result.Append("]");

	return s_result;
}//CString CRealCoding::sToString()

bool CRealCoding::operator==(CRealCoding &cOther)
{
	double d_square_epsilon = d_equal_epsilon * d_equal_epsilon;
	double d_square_distance = 0;

	for (uint16_t i = 0; i < i_number_of_dimensions && d_square_distance <= d_square_epsilon; i++)
	{
		d_square_distance += (*(pd_values + i) - *(cOther.pd_values + i)) * (*(pd_values + i) - *(cOther.pd_values + i));
	}//for (uint16_t i = 0; i < i_number_of_dimensions && d_square_distance <= d_square_epsilon; i++)

	return d_square_distance <= d_square_epsilon;
}//bool CRealCoding::operator==(CRealCoding &cOther)

bool CRealCoding::operator!=(CRealCoding &cOther)
{
	return !(*this == cOther);
}//bool CRealCoding::operator!=(CRealCoding &cOther)

ostream& operator<<(ostream &sOutput, CRealCoding *pcRealCoding)
{
	sOutput << pcRealCoding->sToString();

	return sOutput;
}//ostream& operator<<(ostream &sOutput, CRealCoding *pcRealCoding)