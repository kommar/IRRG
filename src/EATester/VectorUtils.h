#ifndef VECTOR_UTILS_H
#define VECTOR_UTILS_H

#include <algorithm>
#include <vector>

using namespace std;

namespace VectorUtils
{
	template <typename T>
	bool bRemove(vector<T> *pvVector, T tElement)
	{
		bool b_removed = false;

		vector<T>::iterator it_element = find(pvVector->begin(), pvVector->end(), tElement);

		if (it_element != pvVector->end())
		{
			pvVector->erase(it_element);
		}//if (it_element != pvVector->end())

		return b_removed;
	}//bool bRemove(vector<T> *pvVector, T tElement)

	template <typename T>
	void vDeleteElements(vector<T*> *pvVector)
	{
		for (size_t i = 0; i < pvVector->size(); i++)
		{
			delete pvVector->at(i);
		}//for (size_t i = 0; i < pvVector->size(); i++)
	}//void vDeleteElements(vector<T*> *pvVector)

	template <typename T>
	void vDeleteElementsAndClear(vector<T*> *pvVector)
	{
		vDeleteElements(pvVector);
		pvVector->clear();
	}//void vDeleteElementsAndClear(vector<T*> *pvVector)
}//namespace VectorUtils

#endif//VECTOR_UTILS_H