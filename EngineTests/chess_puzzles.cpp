#include "pch.h"
#include "..\BoardGamesEngine\Games\chess.h"
#include "..\BoardGamesEngine\Algorithms.h"

void check(std::string pgn_or_fen, int depth, int mate_in) {
	chess::ChessPosition<false> pos(pgn_or_fen);
	
	EXPECT_EQ(pos.turn(), Player::First);
	chess::Move move;
	for (int i = 1; i < mate_in; i++) {
		// White move
		move = MinMax<chess::ChessPosition<false>, chess::Move>::FindBestMove(pos, depth);
		pos += move;
		EXPECT_FALSE(pos.is_check_mate()) << "Checkmate in only " << i << " moves instead of " << mate_in;

		// Black move
		move = MinMax<chess::ChessPosition<false>, chess::Move>::FindBestMove(pos, depth);
		pos += move;
		EXPECT_FALSE(pos.is_check_mate());
	}

	// Final white move
	move = MinMax<chess::ChessPosition<false>, chess::Move>::FindBestMove(pos, depth);
	pos += move;
	EXPECT_TRUE(pos.is_check_mate());
}

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
