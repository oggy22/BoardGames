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

TEST(Connect4_test, FourByThree)
{
#define S(str) SquareBase<4, 3>(str)

	std::vector<Move<4, 3>> moves = {
		Move<4, 3>(S("C1"), Field::X),
		Move<4, 3>(S("B1"), Field::O),
		Move<4, 3>(S("B2"), Field::X),
		Move<4, 3>(S("B3"), Field::O),
		Move<4, 3>(S("C2"), Field::X),
		Move<4, 3>(S("C3"), Field::O),
		Move<4, 3>(S("A1"), Field::X),
		Move<4, 3>(S("A2"), Field::O),
		Move<4, 3>(S("A3"), Field::X),
	};
	MNKGravity<4, 3, 3> FourByThree;
	for (int i = 0; i < moves.size(); i++)
	{
		auto move = MinMax<MNKGravity<4, 3, 3>, Move<4, 3>, KillerOptions::Multiple, false>::FindBestMove(FourByThree, 12);
		GTEST_LOG_(INFO) << move.chess_notation();
		if (i < moves.size() - 1)
			EXPECT_FALSE(FourByThree.easycheck_winning_move(move));
		else
			EXPECT_TRUE(FourByThree.easycheck_winning_move(move));
		FourByThree += move;
		EXPECT_EQ(moves[i], move) << "Expected move: " << moves[i].chess_notation() << " but got: " << move.chess_notation();
	}
}