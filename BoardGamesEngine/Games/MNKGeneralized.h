#pragma once

#include <experimental/generator>
#include <array>
#include "..\core.h"

/// <summary>
/// Properties of dimension
/// </summary>
enum DimProp
{
	None = 0,
	Gravity = 1,	// for example Connect4
	Circular = 2	// the first and the last are adjecent
};

enum class Field : int8_t
{
	Empty = 0, X = 1, O = -1
};

inline std::ostream& operator<<(std::ostream& os, Field field)
{
	if (field == Field::X)
		os << 'X';
	else if (field == Field::O)
		os << 'O';
	else
		os << ' ';
	return os;
}

template <int W, int H>
struct Move
{
	SquareBase<W, H> square;
	Field field;
	bool is_valid() const
	{
		return square.is_valid();
	}
	std::string chess_notation() const
	{
		return square.chess_notation();
	}
};

template <
	int W, DimProp dpw,	// width of the board and width properties
	int H, DimProp dph,	// height of the board and height properties
	int R,				// the number of required <tokens> in a row to win
	bool ExactRequired, // overline not allowed, see Gomoku
	bool OnlyXs,		// Both players put X, see https://en.wikipedia.org/wiki/Notakto
	bool OneFirstOnly>
class MNKGeneralized : public BoardBase<W, H, Field>
{
	static_assert(W > 0 && H > 0 && R > 0, "W, H and R must be positive");
	static_assert(R <= W && R <= H, "R may not be greater than either dimension");

public:
	float Evaluate()
	{
		return 0;
	}

	MNKGeneralized()
	{
		SquareBase<W, H> sq(0);
		do
		{
			(*this)[sq] = Field::Empty;
		} while (++sq);
	}

#pragma region Horizontal and Vertical
	std::experimental::generator<Field> horizontal_fields(int y) const
	{
		SquareBase<W, H> sq(0, y);
		do
		{
			co_yield (*this)[sq];
		} while (sq.move_right());
	}

	std::experimental::generator<Field> vertical_fields(int x) const
	{
		SquareBase<W, H> sq(x, 0);
		do
		{
			co_yield (*this)[sq];
		} while (sq.move_up());
	}
#pragma endregion

#pragma region Main diagonals
	std::experimental::generator<Field> X0_diagonal_fields(int y) const
	{
		SquareBase<W, H> sq(0, y);
		do
		{
			co_yield (*this)[sq];
		} while (sq.move_upright());
	}

	std::experimental::generator<Field> Y0_diagonal_fields(int x) const
	{
		SquareBase<W, H> sq(x, 0);
		do
		{
			co_yield (*this)[sq];
		} while (sq.move_upright());
	}
#pragma endregion

#pragma region Other diagonals
	std::experimental::generator<Field> YMAX_diagonal_fields(int x) const
	{
		SquareBase<W, H> sq(x, H - 1);
		do
		{
			co_yield (*this)[sq];
		} while (sq.move_downright());
	}

	std::experimental::generator<Field> XMAX_diagonal_fields(int y) const
	{
		SquareBase<W, H> sq(W - 1, y);
		do
		{
			co_yield (*this)[sq];
		} while (sq.move_upleft());
	}
#pragma endregion

	std::experimental::generator<SquareBase<W, H>> empty_squares() const
	{
		for (SquareBase<W, H> sq : SquareBase<W, H>::all_squares())
		{
			if ((*this)[sq] == Field::Empty)
				co_yield sq;
		}
	}

	std::experimental::generator<std::experimental::generator<Field>> all_directions() const
	{
		if constexpr (R <= W)
			for (int y = 0; y < H; y++)
				co_yield horizontal_fields(y);

		if constexpr (R <= H)
			for (int x = 0; x < W; x++)
				co_yield vertical_fields(x);

		for (int y = 0; y <= H - R; y++)
		{
			co_yield X0_diagonal_fields(y);
			co_yield XMAX_diagonal_fields(y);
		}

		co_yield Y0_diagonal_fields(0);
		for (int x = 1; x <= W - R; x++)
		{
			co_yield Y0_diagonal_fields(x);
			co_yield YMAX_diagonal_fields(x);
		}
	}

	// At most one of X and O have winning strech of R in a row
	bool is_legal() const
	{
		bool XStreakFound = false, OStreakFound = false;
		for (auto generator : all_directions())
		{
			// Count Xs and Os in a row and if they both count to R,
			// the position is not legal.
			int Xs = 0, Os = 0;
			for (Field field : generator)
			{
				switch (field)
				{
				case Field::Empty:
					Xs = Os = 0;
					break;
				case Field::X:
					Xs++; Os = 0;
					if (Xs == R) { XStreakFound = true; if (OStreakFound) return false; }
					break;
				case Field::O:
					Os++; Xs = 0;
					if (Os == R) { OStreakFound = true; if (XStreakFound) return false; }
					break;
				}
			}
		}
		return true;
	}

	bool is_legal(Move<W,H> move) const
	{
		DCHECK(move.field != Field::Empty);
		if ((*this)[move.square] != Field::Empty)
			return false;
		if constexpr (dph == DimProp::Gravity)
		{
			if (move.square.y() > 0 && (*this)[SquareBase<W, H>(move.square.x(), move.square.y()-1)] == Field::Empty)
				return false;
		}
		if constexpr (dpw == DimProp::Gravity)
		{
			if (move.square.x() > 0 && (*this)[SquareBase<W, H>(move.square.x() - 1, move.square.y())] == Field::Empty)
				return false;
		}
		return true;
	}

	bool easycheck_winning_move(Move<W, H> move)
	{
		SquareBase<W, H> sq;

#define COUNT_DIRECTION(MOVE, COUNTER) { sq = move.square; while (sq.MOVE() && this->square(sq) == move.field) { COUNTER++; } };

		// Check vertically
		int up = 0, down = 0;
		COUNT_DIRECTION(move_down, down);
		if constexpr (dph != DimProp::Gravity)
		{
			COUNT_DIRECTION(move_up, up);
		}
		if (up + down + 1 >= R) return true;

		// Check horizontally
		int left = 0, right = 0;
		COUNT_DIRECTION(move_left, left);
		if constexpr (dpw != DimProp::Gravity)
		{
			COUNT_DIRECTION(move_right, right);
		}
		if (left + right + 1 >= R) return true;

		// Check main diagonal
		int down_left = 0, up_right = 0;
		COUNT_DIRECTION(move_downleft, down_left);
		if constexpr (dpw != DimProp::Gravity || dph != DimProp::Gravity)
		{
			COUNT_DIRECTION(move_upright, up_right);
		}
		if (down_left + up_right + 1 >= R) return true;

		// Check other diagonal
		int up_left = 0, down_right = 0;
		COUNT_DIRECTION(move_upleft, up_left);
		COUNT_DIRECTION(move_downright, down_right);
		if (down_right + up_left + 1 >= R) return true;

		return false;
	}

	void operator+=(Move<W, H> move)
	{
		this->move();
		this->square(move.square) = move.field;
	}

	void operator-=(Move<W, H> move)
	{
		this->reverse_move();
		this->square(move.square) = Field::Empty;
	}
};

// Fields sorted by distance from the center of WxH board
template<int W, int H>
struct FieldsOptimized {
	//TODO: using consteval std::abs should be possible in c++23
	// https://en.cppreference.com/w/cpp/numeric/math/abs
	static consteval int abs(int i)
	{
		if (i < 0) return -i;
		return i;
	}

	static constexpr int distance(SquareBase<W, H> sq)
	{
		return
			abs(W - 1 - 2 * sq.x()) +
			abs(H - 1 - 2 * sq.y());
	}

	consteval FieldsOptimized() : arr()
	{
		// Initialize
		for (int i = 0; i < W * H; ++i)
			arr[i] = SquareBase<W, H>(i);

		//TODO: use std::sort
		// Sort them
		for (int i = 0; i < W * H; ++i)
			for (int j = i + 1; j < W * H; ++j)
			{
				if (distance(arr[i]) > distance(arr[j]))
					std::swap(arr[i], arr[j]);
			}
	}

public:
	static const constexpr SquareBase<W, H> arr[W * H];
};

// Array sorted by distance from the center
template<int N>
struct ArrayOptimized
{
	static consteval int abs(int i)
	{
		if (i < 0) return -i;
		return i;
	}

	static consteval int distance(int i)
	{
		return abs(N - 1 - 2 * i);
	}

	static consteval std::array<int, N> InitializeArray() {
		std::array<int, N> result;
		for (int i = 0; i < N; ++i) {
			result[i] = i;
		}

		for (int i = 0; i < N; ++i)
			for (int j = i + 1; j < N; ++j)
			{
				if (distance(result[i]) > distance(result[j]))
					std::swap(result[i], result[j]);
			}
		return result;
	}

public:
	static const constexpr std::array<int, N> arr = InitializeArray();
};

template <
	int W, 
	int H, 
	int R				// the number of required <tokens> in a row to win
>
class MNK : public MNKGeneralized<W, DimProp::None, H, DimProp::None, R, false, false, false>
{
public:
	MNK() { }

	MNK(std::string fen)
	{
		SquareBase<W, H> sq(0);
		for (int i = 0; i < fen.length(); i++)
		{
			DCHECK(sq.is_valid());
			if (fen[i] == '/')
			{
				DCHECK(i != 0);
				DCHECK(i != fen.length() - 1);
				DCHECK(fen[i - 1] != '/');
				DCHECK(sq.x() == 0);
				continue;
			}
			switch (fen[i])
			{
			case 'X': case 'x': this->square(sq) = Field::X; ++sq; break;
			case 'O': case 'o': this->square(sq) = Field::O; ++sq; break;
			case '0': case '1':
			case '2': case '3':
			case '4': case '5':
			case '6': case '7':
			case '8': case '9': for (int j = 0; j < fen[i] - '0'; j++, ++sq)
			{
				this->square(sq) = Field::Empty;
			} break;
			default: DCHECK(false);
			};
		}
	}

	static FieldsOptimized<W, H> fo;

	std::experimental::generator<Move<W, H>> all_legal_moves_played()
	{
		for (SquareBase<W, H> sq : this->empty_squares())
		{
			Move<W, H> move;
			move.square = sq;
			move.field = this->turn() == Player::First ? Field::X : Field::O;
			this->move();
			this->square(move.square) = move.field;
			co_yield move;
		}
	}

	std::experimental::generator<Move<W, H>> all_legal_moves() const
	{
		for (SquareBase<W, H> sq : this->empty_squares())
		{
			Move<W, H> move;
			move.square = sq;
			move.field = this->turn() == Player::First ? Field::X : Field::O;
			co_yield move;
		}
	}
};

template <
	int W,
	int H,
	int R				// the number of required <tokens> in a row to win
>
class MNKGravity : public MNKGeneralized<W, DimProp::None, H, DimProp::Gravity, R, false, false, false>
{
public:
	std::experimental::generator<Move<W, H>> all_legal_moves() const
	{
		for (int i = 0; i < W; i++)
		{
			SquareBase<W, H> sq(ArrayOptimized<W>::arr[i], 0);
			Move<W, H> move;
			move.square = sq;

			bool inside = true;
			while (inside && this->square(move.square) != Field::Empty)
			{
				inside = move.square.move_up();
			}

			if (inside)
			{
				move.field = (this->turn() == Player::First ? Field::X : Field::O);
				co_yield move;
			}
		}
	}

	std::experimental::generator<Move<W, H>> all_legal_moves_played()
	{
		for (auto move : all_legal_moves())
		{
			(*this) += move;
			co_yield move;
		}
	}
};

class TicTacToe : public MNK<3,3,3>
{

};

template <
	int W, DimProp dpw,
	int H, DimProp dph,
	int R,
	bool ExactRequired,
	bool OnlyXs,
	bool OneFirstOnly>
inline std::ostream& operator<<(std::ostream& os, const MNKGeneralized<W, dpw, H, dph, R, ExactRequired, OnlyXs, OneFirstOnly>& mnk)
{
	for (int y = H - 1; y >= 0; y--)
	{
		os << '|';
		for (int x = 0; x < W; x++)
			os << mnk(x, y);
		os << '|' << std::endl;
	}	
	return os;
}


