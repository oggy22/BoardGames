#include "pch.h"
#include "..\BoardGamesEngine\Games\Connect4.h"
#include "..\BoardGamesEngine\Algorithms.h"

TEST(Connect4_test, all_legal_moves)
{
	Connect4 connect4;
	int count = 0;
	bool first = true;
	Move<7, 6> last_move;
	for (auto move : connect4.all_legal_moves())
	{
		if (first)
		{
			EXPECT_EQ(move.square.x(), 3);
			first = false;
		}
		EXPECT_TRUE(move.is_valid());
		EXPECT_EQ(move.square.y(), 0);
		EXPECT_TRUE(connect4.is_legal(move));
		count++;
		last_move = move;
	}
	EXPECT_TRUE(last_move.square.x() == 0 || last_move.square.x() == 6);
	EXPECT_EQ(count, 7);
}

TEST(Connect4_test, play_20_moves)
{
	Connect4 connect4;
	Move<7, 6> move;
	for (int i = 0; i < 20; i++)
	{
		move = MinMax<Connect4, Move<7, 6>>::FindBestMove(connect4, 6);
		GTEST_LOG_(INFO) << move.chess_notation();
		EXPECT_TRUE(connect4.is_legal(move)) << " not legal move";
		bool game_over = connect4.easycheck_winning_move(move);
		EXPECT_FALSE(game_over) << " game over!";
		connect4 += move;
	}
}
