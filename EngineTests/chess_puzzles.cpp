#include "pch.h"
#include "..\BoardGamesEngine\Games\chess.h"
#include "..\BoardGamesEngine\Algorithms.h"

TEST(chess_puzzles, KRK_MateIn1) {
	chess::ChessPosition<false> pos(std::string("4k3/8/4K3/8/8/8/8/7R"));
	pos.track_pgn();
	auto move = MinMax<chess::ChessPosition<false>, chess::Move>::FindBestMove(pos, 6);
	pos += move;
	GTEST_LOG_(INFO) << move.chess_notation();
	EXPECT_TRUE(pos.is_checked(pos.turn()));
	EXPECT_TRUE(pos.is_check_mate());
}

TEST(chess_puzzles, KRK_MateIn2) {
	chess::ChessPosition<false> pos(std::string("6k1/8/5K2/8/8/8/8/7R"));
	pos.track_pgn();
	
	auto move = MinMax<chess::ChessPosition<false>, chess::Move>::FindBestMove(pos, 6);
	pos += move;
	GTEST_LOG_(INFO) << move.chess_notation();
	EXPECT_FALSE(pos.is_check_mate());

	move = MinMax<chess::ChessPosition<false>, chess::Move>::FindBestMove(pos, 4);
	pos += move;
	GTEST_LOG_(INFO) << move.chess_notation();

	move = MinMax<chess::ChessPosition<false>, chess::Move>::FindBestMove(pos, 4);
	pos += move;
	GTEST_LOG_(INFO) << move.chess_notation();
	EXPECT_TRUE(pos.is_check_mate());
}
