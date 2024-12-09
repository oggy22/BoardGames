#include "pch.h"
#include "..\BoardGamesEngine\Games\chess.h"
#include "..\BoardGamesEngine\Algorithms.h"

TEST(Algorithm_suite, chess)
{
	chess::ChessPosition<false> pos(false);
	MinMax<chess::ChessPosition<false>, chess::Move, KillerOptions::SingleUpdating, true>::FindBestMove(
		pos,
		DebugRelease(8 /* ~1s Debug */, 10 /* ~2s Release */));
}

TEST(Algorithm_suite, chess_material)
{
	int count = 0;
	chess::ChessPosition<false> pos(false);
	pos.turn_on_material_tracking();
	MinMax<chess::ChessPosition<false>, chess::Move, KillerOptions::SingleUpdating, true>::FindBestMove(
		pos,
		DebugRelease(6 /* ~1s Debug */, 8 /* ~2s Release */),
		[&](chess::ChessPosition<false>& pos) -> EvalValue::payload_t
		{
			count++;
			return pos.evaluate<1>();
		});
	std::cout << "Count: " << count << std::endl;
}