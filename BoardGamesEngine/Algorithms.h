#pragma once
#include <functional>
#include "core.h"

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
private:
	payload_t value;
};

enum class KillerOptions
{
	None = 0,
	Single = 1,
	Multiple = 2
};

template <typename Pos, typename Move, KillerOptions ko = KillerOptions::Single>
requires BoardPosition<Pos, Move>
class MinMax
{
public:
	static_assert(ko != KillerOptions::Multiple, "KillerOptions::Multiple not implemented");
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
		if (position.turn() == Player::First)
			return minmax.Find<Player::First>(0, depth).move;
		else
			return minmax.Find<Player::Second>(0, depth).move;
	}

	std::vector<Move> killer;
	Pos position;
	MinMax(const Pos& position, int depth) : killer(depth+1, Move()), position(position) {}

	struct MoveVal
	{
		Move move;
		EvalValue val;
	};

private:
	std::experimental::generator<Move> all_legal_moves_played_and_killer(int depth)
	{
		if constexpr (ko == KillerOptions::Single)
		{
			Player turn = position.turn();
			if (position.play_if_legal(killer[depth]))
			{
				DCHECK(position.turn() != turn);
				co_yield killer[depth];
			}
		}

		for (auto move : position.all_legal_moves_played())
		{
			if constexpr (ko == KillerOptions::None)
			{
				co_yield move;
			}
			else if constexpr (ko == KillerOptions::Single)
			{
				if (move != killer[depth])
					co_yield move;
				else
					position -= move;
			}
		}
	}

	std::function<EvalValue::payload_t(Pos& position)> eval_func;

	template <Player player1>
	MoveVal Find(int curr_depth, int max_depth, EvalValue cut = EvalValue::Win<player1>())
	{
		constexpr Player player2 = oponent(player1);
		DCHECK(position.turn() == player1);
		if (curr_depth == max_depth)
			return { Move(), eval_func(position) };

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
			if constexpr (ko == KillerOptions::Single)
			{
				// Update killer move for the next depth
				if (!killer[curr_depth + 1].is_valid()) // todo: if not legal as well
					killer[curr_depth + 1] = best2.move;
			}
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

		return best;
	}
};
