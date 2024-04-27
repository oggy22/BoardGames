#pragma once
#include <experimental/generator>

#include "..\core.h"

namespace checkers
{
typedef SquareBase<8, 8> Square;

enum class Piece : int8_t
{
	None = 0, Chip = 1, King = 2,
	OtherChip = -1, OtherKing = -2
};

bool belongs_to(Piece piece, Player player)
{
	return player == Player::First ? (int8_t(piece) > 0) : (int8_t(piece) < 0);
}

class Move
{
	Square _from, _to, _cap1, _cap2;
public:
	Move() : _from(Square()) {}
	Move(Square from, Square to) : _from(from), _to(to), _cap1(0-1), _cap2(0 - 1) { }
	Move(Square from, Square to, Square cap1) : _from(from), _to(to), _cap1(cap1), _cap2(0 - 1) { }
	Move(Square from, Square to, Square cap1, Square cap2) : _from(from), _to(to), _cap1(cap1), _cap2(cap2) { }

	bool is_valid() { return _from.is_valid(); }

	Square from() { return _from; }
	Square to() { return _to; }
	Square cap1() { return _cap1; }
	Square cap2() { return _cap2; }
};

class CheckersPosition : public BoardBase<8, 8, Piece>
{
public:
	CheckersPosition()
	{
		for (auto& sq : this->get_squares())
		{
			// Make all white squares empty
			if (sq.is_white())
			{
				(*this)[sq] = Piece::None;
				continue;
			}

			// And black squares are, based on y coordinate:
			// chip, empty or otherchip
			switch (sq.y())
			{
			case 0: case 1: case 2: (*this)[sq] = Piece::Chip; continue;
			case 3: case 4: (*this)[sq] = Piece::None; continue;
			case 5: case 6: case 7: (*this)[sq] = Piece::OtherChip; continue;
			default: DCHECK_FAIL;
			}
		}
	}

	float Evaluate() { return 0; }

	void operator+=(Move move)
	{
		(*this)[move.to()] = (*this)[move.from()];
		(*this)[move.from()] = Piece::None;
		if (!move.cap1().is_valid())
			return;
		(*this)[move.cap1()] = Piece::None;
		if (!move.cap2().is_valid())
			return;
		(*this)[move.cap2()] = Piece::None;
	}

	void operator-=(Move move)
	{
		(*this)[move.from()] = (*this)[move.to()];
		(*this)[move.from()] = Piece::None;
		if (!move.cap1().is_valid())
			return;
		(*this)[move.cap1()] = Piece::None;
		if (!move.cap2().is_valid())
			return;
		(*this)[move.cap2()] = Piece::None;
	}

	bool easycheck_winning_move(Move move) { return false; }

	std::experimental::generator<Square> get_all_legal_squares()
	{
		Square sq;
		do
		{
			// Examine only black squares, e.g. black is A1
			if (sq.x() + sq.y() % 2 != 0)
				continue;

			co_yield sq;
		} while (++sq);
	}

	std::experimental::generator<Square> forward_squares(Player player, Square sq) const
	{
		Square sq2 = sq;
		if (player == Player::First)
		{
			sq2 = sq; if (sq2.move_upleft() && (*this)[sq2] == Piece::None) co_yield sq2;
			sq2 = sq; if (sq2.move_upright() && (*this)[sq2] == Piece::None) co_yield sq2;
		}
		else if (player == Player::Second)
		{
			sq2 = sq; if (sq2.move_downleft() && (*this)[sq2] == Piece::None) co_yield sq2;
			sq2 = sq; if (sq2.move_downright() && (*this)[sq2] == Piece::None) co_yield sq2;
		}
		else
		{
			DCHECK(false);
		}
	}

	std::experimental::generator<Move> jump_move(Player player, Square sq)
	{
#define JUMP_MOVE(move)												\
sq2 = sq;															\
if (sq2.move() && belongs_to((*this)[sq2], oponent(player)))		\
{																	\
	Square cap = sq2;												\
	if (sq2.move() && (*this)[sq2] == Piece::None)					\
		co_yield Move(sq, sq2, cap);								\
}

		Square sq2 = sq;
		if (turn() == Player::First)
		{
			JUMP_MOVE(move_upleft);
			JUMP_MOVE(move_upright);
		}
		else if (turn() == Player::Second)
		{
			JUMP_MOVE(move_downleft);
			JUMP_MOVE(move_downright);
		}
		else
		{
			DCHECK(false);
		}
	}
	
	std::experimental::generator<Move> jump_move_recursive(Player player, Square sq)
	{
		for (Move move : jump_move(player, sq))
		{
			bool any = false;
			for (Move move2 : jump_move(player, move.to()))
			{
				any = true;
			}
			if (!any)
				co_yield move;
		}
	}

	std::experimental::generator<Move> all_legal_moves_played()
	{
		for (auto sq : get_black_squares())
		{
			if (!belongs_to((*this)[sq], turn()))
				continue;
			
			for (auto sq2 : forward_squares(turn(), sq))
			{
				co_yield Move(sq, sq2);
			}
		}
	}

private:
};

}