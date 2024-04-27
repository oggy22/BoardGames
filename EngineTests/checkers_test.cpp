#include "pch.h"
#include "..\BoardGamesEngine\Games\checkers.h"
#include "..\BoardGamesEngine\Algorithms.h"

TEST(checkers, all_squares) {
	int count = 0;	
	for (auto square : SquareBase<8,8>::all_squares())
	{
		count++;
	}
	EXPECT_EQ(count, 64);
}

TEST(checkers, init_position) {
	int count_white = 0;
	int count_black = 0;
	const checkers::CheckersPosition pos;
	for (auto square : SquareBase<8, 8>::all_squares())
	{
		switch (pos[square])
		{
		case checkers::Piece::Chip: count_white++; break;
		case checkers::Piece::OtherChip: count_black++; break;
		default:
			break;
		}
	}
	EXPECT_EQ(count_white, 12);
	EXPECT_EQ(count_black, 12);
}

TEST(checkers, minmax) {
	checkers::CheckersPosition pos;
	for (int i = 0; i < 10; i++)
	{
		checkers::Move move = MinMax<checkers::CheckersPosition, checkers::Move>::FindBestMove(pos, 4);
		pos += move;
	}
}