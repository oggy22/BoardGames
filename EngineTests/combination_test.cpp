#include "pch.h"
#include "..\BoardGamesEngine\Games\TicTacToe.h"
#include "..\BoardGamesEngine\combinations.h"

#define CombinationTest(N,K)									\
TEST(CombinationTest, CombinationTest_ ## N ## _ ## K)			\
{																\
	SIZE size = nk(N, K);										\
	for (SIZE index = 0; index < size; index++)					\
	{															\
		combination comb(N, K);									\
		for (bool b : combination::get_combination(N,K,index))	\
		{														\
																\
		};														\
	}															\
}

CombinationTest(1, 1)

CombinationTest(6, 4)

//CombinationTest(15, 5)

TEST(CombinationTest, CombinationTest)
{															
	SIZE size = nk(1, 1);
	for (SIZE index = 0; index < 1; index++)
	{					
		combination comb(1, 1);									
		for (bool b : combination::get_combination(1, 1, index))
		{
		};
	}
}