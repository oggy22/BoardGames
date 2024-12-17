#include "pch.h"
#include "..\BoardGamesEngine\Algorithms.h"
#include "..\BoardGamesEngine\Games\MNKGeneralized.h"

TEST(TicTacToeTest, blank_draw)
{
	TicTacToe ttt;

	for (int i = 0; i < 9; i++)
	{
		Move<3, 3> move = MinMax<TicTacToe>::FindBestMove(ttt, 10);
		ttt += move;
		std::cerr << ttt << std::endl;
		EXPECT_FALSE(ttt.easycheck_winning_move(move)) << move.chess_notation();
	}
}

TEST(TicTacToeTest, X_wins)
{
	TicTacToe ttt(std::string("3/1XO/1XO"));
	for (int i = 0; i < 4; i++)
	{
		// Check there is only one winning move
		int count = 0;
		for (auto move : ttt.all_legal_moves())
		{
			if (ttt.easycheck_winning_move(move))
				count++;
		}
		EXPECT_EQ(1, count);

		// FindBestMove finds that winning move
		TicTacToe to_play = ttt;
		Move<3, 3> move = MinMax<TicTacToe>::FindBestMove(to_play, 2);
		EXPECT_TRUE(to_play.is_legal(move));
		to_play += move;
		EXPECT_TRUE(to_play.easycheck_winning_move(move));
		ttt.roate_90();
	}
}

TEST(TicTacToeTest, X_forks_and_wins)
{
	TicTacToe ttt(std::string("3/XXO/1O1"));
	Move<3, 3> moveX = MinMax<TicTacToe>::FindBestMove(ttt, 4);
	EXPECT_EQ(moveX.field, Field::X);
	EXPECT_TRUE(ttt.is_legal(moveX));
	ttt += moveX;
	EXPECT_FALSE(ttt.easycheck_winning_move(moveX));

	Move<3, 3> moveO = MinMax<TicTacToe>::FindBestMove(ttt, 4);
	EXPECT_EQ(moveO.field, Field::O);
	EXPECT_TRUE(ttt.is_legal(moveO));
	ttt += moveO;
	EXPECT_FALSE(ttt.easycheck_winning_move(moveO));

	moveX = MinMax<TicTacToe>::FindBestMove(ttt, 4);
	EXPECT_EQ(moveX.field, Field::X);
	EXPECT_TRUE(ttt.is_legal(moveX));
	ttt += moveX;
	EXPECT_TRUE(ttt.easycheck_winning_move(moveX));
}
