#include "combinations.h"
#include "core.h"

//nk_table::nk_table(int n, int k)
//{
//	table = new int[n + 1][k + 1];
//}

class CombinationsTable
{
	SIZE* table;
	int max;
public:
	CombinationsTable(int max) : max(max)
	{
		table = new SIZE[(max + 1) * (max + 1)];
		
		// Initilize the border and the diagonal
		for (int i = 0; i <= max; i++)
		{
			this->operator()(i, 0) = 1;
			this->operator()(i, i) = 1;
		}

		// Initilize the rest recursively
		for (int i = 1; i <= max; i++)
			for (int j = i - 1; j > 0; j--)
				this->operator()(i, j) = this->operator()(i - 1, j - 1) + this->operator()(i - 1, j);
	}

	~CombinationsTable()
	{
		delete[] table;
	}

	SIZE& operator()(int n, int k) const
	{
		DCHECK(n <= max);
		DCHECK(n >= k && k >= 0);
		return table[n + k * (max + 1)];
	}
};

SIZE nk(int n, int k)
{
	static CombinationsTable table(100);
	return table(n, k);
}
