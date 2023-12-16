#include "pch.h"
#include "..\BoardGamesEngine\Games\chess.h"
#include "..\BoardGamesEngine\Algorithms.h"

TEST(Algorithm_suite, chess)
{
	chess::ChessPosition<false> pos(false);

	MinMax<chess::ChessPosition<false>, chess::Move>::FindBestMove(pos, 4);
}