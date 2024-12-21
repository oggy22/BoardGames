#include "pch.h"
#include "..\BoardGamesEngine\Games\chess.h"
#include "..\BoardGamesEngine\Algorithms.h"

void check(std::string pgn_or_fen, int depth, int mate_in) {
	chess::ChessPosition<false> pos(pgn_or_fen);
	
	EXPECT_EQ(pos.turn(), Player::First);
	chess::Move move;
	for (int i = 1; i < mate_in; i++) {
		// White move
		move = MinMax<chess::ChessPosition<false>>::FindBestMove(pos, depth);
		pos += move;
		EXPECT_FALSE(pos.is_check_mate()) << "Checkmate in only " << i << " moves instead of " << mate_in;

		// Black move
		move = MinMax<chess::ChessPosition<false>>::FindBestMove(pos, depth);
		pos += move;
		EXPECT_FALSE(pos.is_check_mate());
	}

	// Final white move
	move = MinMax<chess::ChessPosition<false>>::FindBestMove(pos, depth);
	pos += move;
	EXPECT_TRUE(pos.is_check_mate());
}

TEST(chess_puzzles, KRK_MateIn1) {
	// https://lichess.org/editor/4k3/8/4K3/8/8/8/8/7R_w_-_-_0_1?color=white
	check("4k3/8/4K3/8/8/8/8/7R", 6, 1);
}

TEST(chess_puzzles, KRK_MateIn2) {
	// https://lichess.org/editor/6k1/8/5K2/8/8/8/8/7R_w_-_-_0_1?color=white
	check("6k1/8/5K2/8/8/8/8/7R", 4, 2);
}

TEST(chess_puzzles, Tricky_MateIn2) {
	// https://lichess.org/study/hqmSoiYO/0LsmqxWW
	check("8/p7/k1K5/8/1p3P2/5R2/8/4B3", 4, 2);
}

TEST(chess_puzzles, SchusterMate) {
	// https://lichess.org/Ii96fdur#6
	check("1. e4 e5 2. Bc4 d6 3. Qf3 Nc6", 2, 1);
}
