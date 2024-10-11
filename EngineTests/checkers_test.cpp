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

TEST(checkers, moves) {
	// There are 7 possible moves both playes can take as their initial moves.
	const int NUMBER_OF_MOVES = 7;
	checkers::CheckersPosition pos;
	int count = 0;
	for (auto move : pos.all_legal_moves())
	{
		EXPECT_TRUE(move.is_valid());
		EXPECT_FALSE(move.cap1().is_valid());
		EXPECT_FALSE(move.cap2().is_valid());

		const auto figure = ((checkers::CheckersPosition const&)(pos))[move.from()];
		EXPECT_EQ(figure, checkers::Piece::Chip);
		count++;

		pos += move;
		int count2 = 0;
		for (auto move2 : pos.all_legal_moves())
		{
			EXPECT_TRUE(move2.is_valid());
			GTEST_LOG_(INFO) << move2.notation();

			count2++;
		}
		EXPECT_EQ(count2, NUMBER_OF_MOVES);
		pos -= move;
	}
	EXPECT_EQ(count, NUMBER_OF_MOVES);
}

TEST(checkers, minmax) {
	checkers::CheckersPosition pos;
	for (int i = 0; i < 10; i++)
	{
		checkers::Move move = MinMax<checkers::CheckersPosition, checkers::Move>::FindBestMove(pos, 4);
		EXPECT_TRUE(move.is_valid());

		pos += move;
	}
}