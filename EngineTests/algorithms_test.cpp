#include "pch.h"
#include "..\BoardGamesEngine\Games\chess.h"
#include "..\BoardGamesEngine\Algorithms.h"
#include "..\BoardGamesEngine\Games\Connect4.h"
#include <algorithm>
#include <chrono>

TEST(Algorithm_suite, chess)
{
	chess::ChessPosition pos(false);
	MinMax<chess::ChessPosition, KillerOptions::SingleUpdating, true>::FindBestMove(
		pos,
		DebugRelease(8 /* ~1s Debug */, 10 /* ~2s Release */));
}

TEST(Algorithm_suite, chess_material)
{
	int count = 0;
	chess::ChessPosition pos(false);
	pos.turn_on_material_tracking();
	MinMax<chess::ChessPosition, KillerOptions::SingleUpdating, true>::FindBestMove(
		pos,
		DebugRelease(6 /* ~1s Debug */, 8 /* ~2s Release */),
		[&](chess::ChessPosition& pos) -> EvalValue::payload_t
		{
			count++;
			return pos.evaluate<1>();
		});
	std::cout << "Count: " << count << std::endl;
}

namespace
{
// A deterministic tree whose move generator checks restoration even when
// destroyed while suspended at an immediate win or cutoff.
struct SearchTestPosition
{
	struct Move
	{
		int from = 0;
		int to = 0;
		bool is_valid() const { return to != 0; }
		bool operator==(const Move&) const = default;
	};
	struct Observations
	{
		int generated = 0;
		int evaluations = 0;
		int illegal_killers = 0;
		int hash_calls = 0;
	};
	Observations* observations;
	int node = 1;
	int ply = 0;
	int terminal_ply = 4;
	bool immediate_win = false;
	bool checked = false;

	Player turn() const { return ply % 2 ? Player::Second : Player::First; }
	void operator+=(Move move) { EXPECT_EQ(node, move.from); node = move.to; ++ply; }
	void operator-=(Move move) { EXPECT_EQ(node, move.to); node = move.from; --ply; }
	bool play_if_legal(Move move)
	{
		if (!move.is_valid() || move.from != node || ply >= terminal_ply)
		{
			++observations->illegal_killers;
			return false;
		}
		*this += move;
		return true;
	}
	std::experimental::generator<Move> all_legal_moves() const
	{
		if (ply < terminal_ply)
			for (int i = 0; i < 2; ++i)
				co_yield Move{ node, node * 2 + i };
	}
	std::experimental::generator<Move> all_legal_moves_played()
	{
		struct RestorationCheck
		{
			SearchTestPosition& position;
			int node;
			int ply;
			~RestorationCheck()
			{
				EXPECT_EQ(position.node, node);
				EXPECT_EQ(position.ply, ply);
			}
		} guard{ *this, node, ply };
		for (auto move : all_legal_moves())
		{
			++observations->generated;
			*this += move;
			co_yield move;
			EXPECT_EQ(node, guard.node);
			EXPECT_EQ(ply, guard.ply);
		}
	}
	bool easycheck_winning_move(Move) const { return immediate_win && ply == 1; }
	bool is_checked(Player) const { return checked; }
	int Evaluate() const { return 0; }
	void turn_off_all_trackings() {}
	static consteval bool implements_hash() { return true; }
	uint64_t get_hash() const
	{
		EXPECT_GE(ply, 3);
		++observations->hash_calls;
		return node;
	}
};

template <KillerOptions option, bool incremental>
void check_tied_search()
{
	SearchTestPosition::Observations observations;
	SearchTestPosition position{ &observations };
	auto move = MinMax<SearchTestPosition, option, incremental>::FindBestMove(
		position, 4, [&](SearchTestPosition& leaf)
		{
			EXPECT_TRUE(leaf.ply == 4 || (incremental && leaf.ply == 2));
			++observations.evaluations;
			return 0;
		});
	EXPECT_EQ(move, (SearchTestPosition::Move{ 1, 2 }));
	EXPECT_EQ(position.node, 1);
	EXPECT_EQ(position.ply, 0);
	EXPECT_GT(observations.hash_calls, 0);
	EXPECT_GT(observations.evaluations, 0);
	EXPECT_GT(observations.illegal_killers, 0);
	if constexpr (option == KillerOptions::SingleUpdating && incremental)
	{
		// Baseline counts include a reused legal root killer and its duplicate
		// from normal enumeration, which must be undone without searching twice.
		EXPECT_EQ(observations.generated, 23);
		EXPECT_EQ(observations.evaluations, 11);
		EXPECT_EQ(observations.hash_calls, 5);
	}
}
}

TEST(Algorithm_suite, tied_order_cutoffs_and_restoration)
{
	check_tied_search<KillerOptions::SingleUpdating, false>();
	check_tied_search<KillerOptions::SingleUpdating, true>();
	check_tied_search<KillerOptions::Fixed2, false>();
	check_tied_search<KillerOptions::Fixed2, true>();
}

TEST(Algorithm_suite, immediate_win_restores_before_generator_destruction)
{
	SearchTestPosition::Observations observations;
	SearchTestPosition position{ &observations };
	position.immediate_win = true;
	auto move = MinMax<SearchTestPosition>::FindBestMove(position, 4);
	EXPECT_EQ(move, (SearchTestPosition::Move{ 1, 2 }));
	EXPECT_EQ(observations.generated, 1);
}

TEST(Algorithm_suite, no_legal_moves)
{
	for (bool checked : { false, true })
	{
		SearchTestPosition::Observations observations;
		SearchTestPosition position{ &observations };
		position.terminal_ply = 0;
		position.checked = checked;
		EXPECT_FALSE(MinMax<SearchTestPosition>::FindBestMove(position, 2).is_valid());
		EXPECT_EQ(observations.generated, 0);
		EXPECT_EQ(observations.hash_calls, 0);
	}
}

// Explicitly opt in with --gtest_also_run_disabled_tests and this test's filter.
// Compare Release x64 builds with the same compiler, inputs and depths.
TEST(Algorithm_suite, DISABLED_search_benchmark)
{
	auto benchmark = [](const char* name, auto search)
	{
		auto expected_evaluations = search(); // Warmup, outside the timed repetitions.
		std::vector<double> milliseconds;
		for (int repetition = 0; repetition < 7; ++repetition)
		{
			auto start = std::chrono::steady_clock::now();
			auto evaluations = search();
			auto end = std::chrono::steady_clock::now();
			EXPECT_EQ(evaluations, expected_evaluations);
			milliseconds.push_back(std::chrono::duration<double, std::milli>(end - start).count());
			std::cout << name << " evaluations=" << evaluations << std::endl;
		}
		std::sort(milliseconds.begin(), milliseconds.end());
		std::cout << name << " median_ms=" << milliseconds[3] << std::endl;
	};
	benchmark("chess_material_depth8", []()
	{
		chess::ChessPosition position(false);
		position.turn_on_material_tracking();
		size_t evaluations = 0;
		auto move = MinMax<chess::ChessPosition, KillerOptions::SingleUpdating, true>::FindBestMove(
			position, 8, [&](chess::ChessPosition& leaf)
			{
				++evaluations;
				return leaf.evaluate<1>();
			});
		EXPECT_TRUE(move.is_valid());
		return evaluations;
	});
	benchmark("connect4_depth8", []()
	{
		Connect4 position;
		size_t evaluations = 0;
		auto move = MinMax<Connect4, KillerOptions::SingleUpdating, true>::FindBestMove(
			position, 8, [&](Connect4&)
			{
				++evaluations;
				return 0;
			});
		EXPECT_TRUE(position.is_legal(move));
		return evaluations;
	});
}
