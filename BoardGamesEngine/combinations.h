#pragma once
#include "core.h"
#include <experimental/generator>

#define SIZE long long

//class nk_table
//{
//public:
//	nk_table(int n, int k);
//private:
//	int table[][];
//	int n, k;
//};
//
SIZE nk(int n, int k);

class combination_ind
{
	int *ind;
	void init(int n, int k, SIZE enumeration, int* ind, int curr)
	{
		// (n,k) = (n-1,k-1) + (n-1,k)
		if (enumeration < nk(n - 1, k - 1))
		{
			ind[0] = curr;
			init(n - 1, k - 1, enumeration - nk(n - 1, k - 1), ind+1, curr + 1);
		}
		else
		{
			init(n - 1, k, enumeration - nk(n - 1, k - 1), ind, curr + 1);
		}
	}
public:
	combination_ind(int n, int k, SIZE enumeration)
	{
		ind = new int[k];
		init(n, k, enumeration, ind, 0);
	}
};

class combination_bin
{
	void init(int n, int k, SIZE enumeration, bool* bits)
	{
		// (n,k) = (n-1,k-1) + (n-1,k)
		if (enumeration < nk(n - 1, k - 1))
		{
			bits[0] = true;
			init(n - 1, k - 1, enumeration - nk(n - 1, k - 1), bits + 1);
		}
		else
		{
			bits[0] = false;
			init(n - 1, k, enumeration - nk(n - 1, k - 1), bits + 1);
		}
	}

	int n, k;
public:
	bool* bits;
	combination_bin(int n, int k, SIZE enumeration) : n(n), k(k)
	{
		bits = new bool[n];
		init(n, k, enumeration, bits);
	}

	~combination_bin()
	{
		delete[] bits;
	}

	std::experimental::generator<int> indices()
	{
		for (int i = 0; i < n; i++)
			if (bits[i])
				co_yield i;
	}
};

template <int N, int K>
SIZE NK()
{
	static_assert(N >= 0 && K >= 0);

	if constexpr (N == K || K == 0)
		return 1;
	else
		return NK<N - 1, K - 1>() + NK<N - 1, K>();
}

template <int N, int K>
class Combination
{
	static void _index_to_bools(SIZE index, bool* b)
	{
		if constexpr (N == 0)
			return;

		SIZE limit = NK<N - 1, K>();
		if (index < limit)
		{
			b[0] = true;
			_index_to_bools<N - 1, K>(index, b + 1);
		}
		else
		{
			b[0] = false;
			_index_to_bools<N - 1, K>(index- limit, b + 1);
		}
	}

public:
	static SIZE size() { return NK<N, K>(); }
	static bool index_to_bools(SIZE index)
	{
		bool b[N];
		_index_to_bools(index, b);
		return true;
	}
};

/// <summary>
/// Converts combination from and to index
/// </summary>
class combination
{
	SIZE index = 0;
	int n, k;
	int curr_n, curr_k;
public:
	combination(int n, int k) : n(n), k(k), curr_n(0), curr_k(0), index(0)
	{
	}

	void add_zero()
	{
		DCHECK(curr_k < k);
		curr_n++;
	}

	void add_one()
	{
		DCHECK(curr_n < n);
		curr_k++;
		curr_n++;
	}

	SIZE get_index()
	{
		DCHECK(curr_n == n);
		DCHECK(curr_k == k);
	}

	/// <summary>
	/// Converts index to combination
	/// </summary>
	static std::experimental::generator<bool> get_combination(int n, int k, SIZE index)
	{
		DCHECK(index >= 0);
		DCHECK(index < nk(n, k));
		// (n/k) = (n-1/k-1) + (n-1/k)
		// (4/2) = 6
		// 1100, 1010, 1001, 0110, 0101, 0011
		int n_ = n, k_ = k;
		for (; n_ > 0; n_--)
		{
 			SIZE treshold = nk(n_ - 1, k_ - 1);
			co_yield index < treshold;
			if (index >= treshold)
				index -= treshold;
			else
				k_--;
		}
		DCHECK(k_ == 0);
	}
};

