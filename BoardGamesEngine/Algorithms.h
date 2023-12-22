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
	float value;
public:
	EvalValue(float val) : value(val) {}

	template <Player player>
	static constexpr EvalValue Win()
	{
		if constexpr (player == Player::First)
			return EvalValue(std::numeric_limits<float>::infinity());

		if constexpr (player == Player::Second)
			return EvalValue(-std::numeric_limits<float>::infinity());
	}

	template <Player player>
	static constexpr EvalValue Lose()
	{
		if constexpr (player == Player::First)
			return EvalValue(-std::numeric_limits<float>::infinity());

		if constexpr (player == Player::Second)
			return EvalValue(std::numeric_limits<float>::infinity());
	}

	template <Player player>
	bool is_better(EvalValue other)
	{
		if constexpr (player == Player::First)
			return value > other.value;

		if constexpr (player == Player::Second)
			return value < other.value;
	}
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
	static MoveVal Find(Pos& position, int curr_depth, int max_depth)
	{
		constexpr Player player2 = oponent(player1);
		DCHECK(position.turn() == player1);
		if (curr_depth = max_depth)
			return { Move(), float(position.Evaluate()) };

		bool any_moves1 = false;
		MoveVal best1 { Move(), EvalValue::Lose<player1>() };
		for (auto move1 : position.all_legal_moves_played())
		{
			if (position.easycheck_winning_move(best1.move))
			{
				best1 = { move1, EvalValue::Win<player1>() };
				position -= move1;
				break;
			}

			any_moves1 = true;

			bool any_moves2 = false;
			MoveVal best2 { Move(), EvalValue::Lose<player2>() };

			for (auto move2 : position.all_legal_moves_played())
			{
				if (position.easycheck_winning_move(best2.move))
				{
					best2 = { move2, EvalValue::Win<player2>() };
					position -= move2;
					break;
				}

				any_moves2 = true;
				
				MoveVal curr = Find<player1>(position, curr_depth + 2, max_depth);
				position -= move2;

				if (curr.val.is_better<player2>(best2.val))
					best2 = curr;

				// Cut short
				if (!best2.val.is_better<player2>(best1.val))
					break;

				EvalValue val1(0);
				val1.is_better<player1>(val1);
			}

			if (!any_moves2)
			{
				best2 = MoveVal{
					move1,
					position.easycheck_winning_move(best1.move) ? EvalValue::Win<player1>() : 0
				};
			}
			position -= move1;

			if (best2.val.is_better<player2>(best1.val))
				best1 = best2;
		}
		return best1;
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
