#ifndef PAIR_UTILS_H
#define PAIR_UTILS_H

#include <utility>

using namespace std;

namespace PairUtils
{
	struct SHashPair
	{
		template <class T1, class T2>
		size_t operator()(const pair<T1, T2> &pPair) const
		{
			hash<T1> c_hash_1;
			hash<T2> c_hash_2;

			size_t i_hash_1 = c_hash_1(pPair.first);
			size_t i_hash_2 = c_hash_2(pPair.second);

			return i_hash_1 != i_hash_2 ? i_hash_1 ^ i_hash_2 : i_hash_1;
		};//size_t operator()(const pair<T1, T2> &sPair) const
	};//struct SHashPair
}//namespace PairUtils

#endif//PAIR_UTILS_H