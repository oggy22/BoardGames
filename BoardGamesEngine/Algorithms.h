#pragma once
#include "core.h"

template <typename T, typename Q>
concept BoardPosition = requires(T pos, Q move)
{
	{   pos.all_legal_moves() } -> std::convertible_to<std::experimental::generator<Q>>;
	{   pos.all_legal_moves(move) } -> std::convertible_to<std::experimental::generator<Q>>;
	{	pos.easycheck_winning_move(move) } -> std::convertible_to<bool>;
	{	pos.is_legal_move(move) } -> std::convertible_to<bool>;

	//{   T::size(EndTable(std::string())) } -> std::convertible_to<SIZE>;
	//{   T::name() } -> std::convertible_to<std::string>;
};

//enum type {MinMax, AlphaBetaCutting, Killer};

class EvalValue
{
public:
	typedef int32_t payload_t;
	
	static const int max_plys = 100; // Up to 50 moves before mate
	static const payload_t max = std::numeric_limits<payload_t>::max();
	EvalValue(payload_t val) : value(val) {}

	///<summary>
	/// Weaken positions leading to win or lose.
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

template <typename Pos, typename Move>
//requires BoardPosition<Pos, Move>
class MinMax
{
public:
	static Move FindBestMove(Pos& position, int depth)
	{
		// depth is an even number greater than zero
		DCHECK(depth % 2 == 0 && depth > 0);

		if (position.turn() == Player::First)
			return Find<Player::First>(position, 0, depth).move;
		else
			return Find<Player::Second>(position, 0, depth).move;
	}

	struct MoveVal
	{
		Move move;
		EvalValue val;
	};

private:
	template <Player player1>
	static MoveVal Find(Pos& position, int curr_depth, int max_depth, EvalValue cut = EvalValue::Win<player1>())
	{
		constexpr Player player2 = oponent(player1);
		DCHECK(position.turn() == player1);
		if (curr_depth == max_depth)
			return { Move(), (EvalValue::payload_t)(position.Evaluate()) };

		MoveVal best { Move(), EvalValue::Lose<player1>() };
		for (auto move1 : position.all_legal_moves_played())
		{
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
			MoveVal best2 = Find<player2>(position, curr_depth + 1, max_depth, best.val);
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

		return best;
	}
};

//template <template T>
//class AlphaBeta;

//template <template T>
//class AlphaBetaKiller
//{
//	const int max_depth = 20;
//	std::experimental::generator<Move> get_all_moves(ChessPosition& position, int curr)
//	{
//		// First examine the killer move, if it's legal
//		if (killer[curr].is_valid())
//			co_yield killer[curr];
//		
//		// Then the rest of the moves
//		for (Move move : position.all_legal_moves())
//			co_yield move;
//	}
//	static Move killer[max_depth + 1];
//
//public:
//	Move FindBestMove(ChessPosition& position, int depth)
//	{
//		// depth is an even number greater than zero and less or equal to max_depth
//		DCHECK(depth & 1 == 0 && depth > 0 && depth <= max_depth);
//		for (int i = 0; <= max_depth; i++) killer[i] = Move::
//		Move bestMove = Find(position, 0, depth).first;
//		return bestMove;
//	}
//private:
//};

//template <template T>
//class Incremental
//{
//public:
//
//private:
//};
