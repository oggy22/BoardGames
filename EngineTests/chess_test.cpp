#include "pch.h"
#include "..\BoardGamesEngine\Games\chess.h"

TEST(chess, all_squares) {
	int count = 0;
	for (chess::Square square : chess::Square::all_squares())
	{
		count++;
	}
	EXPECT_EQ(count, 64);
}

TEST(chess, init_position) {
	chess::ChessPosition<true> pos;

	for (chess::Square square : chess::Square::all_squares())
	{
		chess::Square square_flip = square.flip_vertifaclly();

		EXPECT_EQ(pos[square], -pos[square_flip])
			<< "comparing " << square.chess_notation() << " and " << square_flip.chess_notation() << std::endl;
	}

	int count = 0;
	for (chess::Move move : pos.all_legal_moves())
	{
		count++;
	}
	EXPECT_EQ(count, 20);	// fails with 13
}

TEST(chess, invert) {
	chess::ChessPosition<true> pos;
	chess::ChessPosition<true> pos_copy = pos;
	
	// Invert and expect not equal
	pos_copy.invert();
	EXPECT_EQ(pos, pos_copy);

	// Invert again and expect not equal
	pos_copy.invert();
	//EXPECT_EQ(pos, pos_copy);
}
