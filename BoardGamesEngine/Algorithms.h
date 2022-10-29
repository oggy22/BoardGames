#pragma once
#include "chess.h"

template <typename T>
concept BoardPosition = requires(T t)
{
	{   t.get_all_legal_moves() } -> std::convertible_to<std::experimental::generator<Move>>;
	{	t.is_winning_move(Move) } -> std::convertible_to<bool>;
	//{   T::size(EndTable(std::string())) } -> std::convertible_to<SIZE>;
	//{   T::name() } -> std::convertible_to<std::string>;
};

template <template Pos>
class MinMax requires BoardPosition<Pos>
{
public:
	Move FindBestMove(ChessPosition& position, int depth)
	{
		// depth is an even number greater than zero
		DCHECK(depth & 1 == 0 && depth > 0);

		return Find(position, 0, depth).first;
	}
public:

	struct MoveVal
	{
		Move move;
		float val;
	};

	MoveVal Find(ChessPosition& position, int curr, int depth)
	{
		if (curr = depth)
			return T::Evalutate(position);

		MoveVal max{ Move, 0 - std::numeric_limits<float>::infinity() };
		for (auto move1 : position.get_all_legal_moves())
		{
			if (position.is_check_mate(Player::Second))
			{
				max = { move1, std::numeric_limits<float>::infinity() };
				break;
			}

			MoveVal min = { Move, std::numeric_limits<float>::infinity() };
			for (auto move2 : position.get_all_legal_moves())
			{
				if (position.is_check_mate(Player::First))
				{
					min = { move2, 0 - std::numeric_limits<float>::infinity() }
					break;
				}
				
				MoveVal curr = Find(position, curr + 2, depth);
				if (curr.val < min.second)
					min = curr;
			}
			if (min.val > max.val)
				max = min;
		}
	}
};

template <template T>
class AlphaBeta;

template <template T>
class AlphaBetaKiller
{
	const int max_depth = 20;
	std::experimental::generator<Move> get_all_moves(ChessPosition& position, int curr)
	{
		// First examine the killer move, if it's legal
		if (killer[curr].is_valid())
			co_yield killer[curr];
		
		// Then the rest of the moves
		for (Move move : position.get_all_legal_moves())
			co_yield move;
	}
	static Move killer[max_depth + 1];

public:
	Move FindBestMove(ChessPosition& position, int depth)
	{
		// depth is an even number greater than zero and less or equal to max_depth
		DCHECK(depth & 1 == 0 && depth > 0 && depth <= max_depth);
		for (int i = 0; <= max_depth; i++) killer[i] = Move::
		Move bestMove = Find(position, 0, depth).first;
		return bestMove;
	}
private:
};

template <template T>
class Incremental
{
public:

private:
};
