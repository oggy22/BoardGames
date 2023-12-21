#include "pch.h"
#include "..\BoardGamesEngine\Games\Gomoku.h"

FieldsOptimized<15, 15> MNK<15, 15, 5>::fo;

TEST(Gomoku_test, 15)
{
	Gomoku15 gomoku15;
	auto arr = gomoku15.fo.arr;
}

TEST(Gomoku_test, 19)
{
	Gomoku19 gomoku19;
}

FieldsOptimized<3, 3> MNK<3, 3, 3>::fo;

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
}
