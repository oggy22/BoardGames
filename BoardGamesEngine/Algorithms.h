#pragma once
#include "core.h"

template <typename T, typename Q>
concept BoardPosition = requires(T pos, Q move)
{
	{   pos.all_legal_moves() } -> std::convertible_to<std::experimental::generator<Q>>;
	{   pos.all_legal_moves(move) } -> std::convertible_to<std::experimental::generator<Q>>;
	{	pos.is_winning_move(move) } -> std::convertible_to<bool>;
	{	pos.is_legal_move(move) } -> std::convertible_to<bool>;

	//{   T::size(EndTable(std::string())) } -> std::convertible_to<SIZE>;
	//{   T::name() } -> std::convertible_to<std::string>;
};

//enum type {MinMax, AlphaBetaCutting, Killer};

template <typename Pos, typename Move>
//requires BoardPosition<Pos, Move>
class MinMax
{
public:
	static Move FindBestMove(Pos& position, int depth)
	{
		// depth is an even number greater than zero
		DCHECK(depth % 2 == 0 && depth > 0);

		return Find(position, 0, depth).move;
	}

	struct MoveVal
	{
		Move move;
		float val;
	};

	static MoveVal Find(Pos& position, int curr_depth, int max_depth)
	{
		if (curr_depth = max_depth)
			return { Move(), float(position.Evaluate()) };

		MoveVal max{ Move(), 0 - std::numeric_limits<float>::infinity()};
		for (auto move1 : position.all_legal_moves())
		{
			if (position.is_check_mate(Player::Second))
			{
				max = { move1, std::numeric_limits<float>::infinity() };
				break;
			}

			MoveVal min = { Move(), std::numeric_limits<float>::infinity()};
			for (auto move2 : position.all_legal_moves())
			{
				if (position.is_check_mate(Player::First))
				{
					min = { move2, 0 - std::numeric_limits<float>::infinity() };
					break;
				}
				
				MoveVal curr = Find(position, curr_depth + 2, max_depth);
				if (curr.val < min.val)
					min = curr;

				// Cut short
				if (min.val <= max.val)
					break;
			}
			if (min.val > max.val)
				max = min;
		}
		return max;
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
