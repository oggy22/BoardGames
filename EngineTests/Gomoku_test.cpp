#include "pch.h"
#include <functional>
#include "..\BoardGamesEngine\Games\Gomoku.h"

using namespace std::placeholders;

TEST(Gomoku_test, 15)
{
	Gomoku15 gomoku15;
}

TEST(Gomoku_test, 19)
{
	Gomoku19 gomoku19;
}

template <typename T>
int count_generator(std::function<std::experimental::generator<T>(void)> fun)
{
	int count = 0;
	for (auto x : fun())
	{
		count++;
	}
	return count;
}


template <typename T>
int count_generator(std::experimental::generator<T>(*gen)(void))
{
	int count = 0;
	for (auto x : gen())
	{
		count++;
	}
	return count;
}

TEST(Gomoku_test, TicTacToe)
{
	TicTacToe ttt;

	// 9 total squares
	int count=0;
	for (auto gen : SquareBase<3,3>::all_squares())
	{
		count++;
	}
	EXPECT_EQ(count, 9);

	// 9 empty squares
	count = 0;
	for (auto gen : ttt.empty_squares())
	{
		count++;
	}
	EXPECT_EQ(count, 9);

	// 9 empty squares
	count = 0;
	for (auto gen : ttt.all_legal_moves())
	{
		count++;
	}
	EXPECT_EQ(count, 9);
}
