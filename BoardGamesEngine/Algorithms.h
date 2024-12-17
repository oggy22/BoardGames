#pragma once
#include <functional>
#include "core.h"
#include "KillerMoves.h"

// Uncomment to enable statistics
//#define STATS

class EvalValue
{
public:
	typedef int32_t payload_t;
	
	static const int max_plys = 100; // Up to 50 moves before mate
	static const payload_t max = std::numeric_limits<payload_t>::max();
	EvalValue(payload_t val) : value(val) {}

	///<summary>
	/// Weaken the eval of positions leading to win or lose.
	/// This helps with prioritizing quicker mates when winning,
	/// and longer mates when losing.
	///</summary>
	void weaken_ending_position()
	{
		if (value > max - max_plys)
			value -= 1;
		if (value < -max + max_plys)
			value += 1;
	}

	template <Player player>
	static constexpr EvalValue Win()
	{
		if constexpr (player == Player::First)
			return EvalValue(max);

		if constexpr (player == Player::Second)
			return EvalValue(-max);
	}

	template <Player player>
	static constexpr EvalValue Lose()
	{
		if constexpr (player == Player::First)
			return EvalValue(-max);

		if constexpr (player == Player::Second)
			return EvalValue(max);
	}

	template <Player player>
	bool is_better(EvalValue other)
	{
		if constexpr (player == Player::First)
			return value > other.value;

		if constexpr (player == Player::Second)
			return value < other.value;
	}

	//EvalValue& operator=(const EvalValue&) = default;

	EvalValue() {}

private:
	payload_t value;
};

template <typename Pos, typename Move, KillerOptions ko = KillerOptions::Multiple>
requires BoardPosition<Pos, Move>
class MinMax
{
#ifdef STATS
	class Stats
	{
	public:
		Stats()
		{

		}
		int killer_moves = 0;
		int killer_hits = 0;
		int killer_misses = 0;

		int transposition_hits = 0;
		int transposition_misses = 0;
	};
	inline static std::vector<Stats> stats;
#endif

public:
	static Move FindBestMove(
		const Pos& position,
		int depth,
		std::function<EvalValue::payload_t(Pos& position)> eval_func = [](Pos& position) -> EvalValue::payload_t
		{
			return 0;
		})
	{
		// depth is an even number greater than zero
		DCHECK(depth % 2 == 0 && depth > 0);

		MinMax minmax(position, depth);
		minmax.eval_func = eval_func;
		minmax.transposition_table.resize(depth + 1);

#ifdef STATS
		stats.resize(depth + 1);
#endif

		Move move;
		if (position.turn() == Player::First)
			move = minmax.Find<Player::First>(0, depth).move;
		else
			move = minmax.Find<Player::Second>(0, depth).move;
#ifdef STATS
		for (int i = 0; i <= depth; i++)
		{
			auto& s = stats[i];
			int killer_size = minmax.killer_manager.size(i);
			GTEST_LOG_(INFO) << "Depth " << i << ":\n"
				<< "Killer moves: " << killer_size << "\n"
				<< "Killer hits: " << s.killer_hits << "\n"
				<< "Killer misses: " << minmax.killer_manager.size(i) << "\n"
				<< "Transposition hits: " << s.transposition_hits << "\n"
				<< "Transposition misses: " << s.transposition_misses << "\n"
				<< "Transposition size: " << minmax.transposition_table[i].size() << "\n";
		}
		stats.resize(depth + 1);
#endif
		return move;
	}

	Pos position;
	std::vector<KillerMoveManager<ko, Move>> killer_manager;
	
	MinMax(const Pos& position, int depth) :
		position(position),
		killer_manager(depth + 1)
	{}

	struct MoveVal
	{
		Move move;
		EvalValue val;
	};

private:
	std::experimental::generator<Move> all_legal_moves_played_and_killer(int depth)
	{
		if constexpr (ko != KillerOptions::None)
		{
			Player turn = position.turn();
			for (Move move : killer_manager[depth].all_killers())
			{
				if (position.play_if_legal(move))
				{
					DCHECK(position.turn() != turn);
					co_yield move;
#ifdef STATS
					stats[depth].killer_hits++;

				}
				else
				{
					stats[depth].killer_misses++;
#endif
				}
			}
		}

		for (auto move : position.all_legal_moves_played())
		{
			if constexpr (ko == KillerOptions::None)
			{
				co_yield move;
			}
			else
			{
				if (!killer_manager[depth].is_killer(move))
					co_yield move;
				else
					position -= move;
			}
		}
	}

	std::function<EvalValue::payload_t(Pos& position)> eval_func;
	
	std::vector<std::unordered_map<uint64_t, MoveVal>> transposition_table;

	template <Player player1>
	MoveVal Find(int curr_depth, int max_depth, EvalValue cut = EvalValue::Win<player1>())
	{
		constexpr Player player2 = oponent(player1);
		DCHECK(position.turn() == player1);
		if (curr_depth == max_depth)
			return { Move(), eval_func(position) };
		
		uint64_t hash;
		if constexpr (Pos::implements_hash())
		{
			if (curr_depth >= 3)
			{
				hash = position.get_hash();
				auto it = transposition_table[curr_depth].find(hash);
				if (it != transposition_table[curr_depth].end())
				{
#ifdef STATS
					stats[curr_depth].transposition_hits++;
#endif // STATS
					return it->second;
				}
				else
				{
#ifdef STATS
					stats[curr_depth].transposition_misses++;
#endif // STATS
				}
			}
		}

		MoveVal best { Move(), EvalValue::Lose<player1>() };
		for (auto move1 : all_legal_moves_played_and_killer(curr_depth))
		{
			DCHECK(position.turn() == player2);

			// Get the first move as best.
			// Without this assignment, losing positions would keep invalid move Move().
			if (!best.move.is_valid())
				best.move = move1;

			// Not for chess, for MNK-like only:
			// If this is a winning move, return immediately
			if (position.easycheck_winning_move(move1))
			{
				position -= move1;
				return { move1, EvalValue::Win<player1>() };
			}

			// Perform recursive call and reverse the move
			MoveVal best2 = Find<player2>(curr_depth + 1, max_depth, best.val);
			// Help prefer quicker mates
  			best2.val.weaken_ending_position();
			position -= move1;

			// Update the best if the search returned better value for player1
			if (best2.val.is_better<player1>(best.val))
				best = { move1, best2.val };

			// Cut the search if better than the cut value
			if (best.val.is_better<player1>(cut))
				return best;
		}

		// If no moves, value=0. For chess verify if checked, then lose.
		if (!best.move.is_valid())
		{
			best.val = position.is_checked(player1)
				? EvalValue::Lose<player1>()
				: 0;
		}

		killer_manager[curr_depth].update(best.move);

		if constexpr (Pos::implements_hash())
		{
			transposition_table[curr_depth][hash] = best;
		}
		return best;
	}
};
