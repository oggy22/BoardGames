#pragma once
#include <experimental/generator>
#include "..\core.h"

enum class Field : int8_t
{
	Empty = 0, X = 1, O=-1
};

template <int W, int H>
class Square : SquareBase<W, H>
{
	int index;
public:
	Square(int x, int y) : index(x + y * W)
	{
		DCHECK(0 <= x && x < W);
		DCHECK(0 <= y && y < H);
	}
	bool move_left();
	bool move_right();
	bool move_up();
	bool move_down();

	bool move_upleft();
	bool move_upright();
	bool move_downleft();
	bool move_downright();
};

template <int W, int H>
struct Move
{
	Square<W,H> square;
	Field field;
};

enum Gravity
{
	None = 0, Down = 1, Both = 2
};

// W - Board Width
// H - Board Height
// R - Required stretch length
template <int W, int H, int R, Gravity grav = Gravity::None>
class TicTacToePosition : public BoardBase<W, H, Field>
{
	static_assert(W > 0 && H > 0 && R > 0, "W, H and R must be positive");
	static_assert(R <= W && R <= H, "R may not be greater than dimensions");
	//Field table[W * H];
	int ply = 0;
public:
	Field& operator[](SquareBase<W,H> sq)
	{
		//this->xyz();
		//xyz();
		return this->table[sq];
	}

	TicTacToePosition()
	{
		ply = 0;
		for (int i = 0; i < W * H; i++)
			this->table[i] = Field::Empty;
	}

	std::experimental::generator<Field> horizontal_fields(int y)
	{
		for (int x = 0; x < W; x++)
			co_yield (*this)(x, y);
	}

	std::experimental::generator<Field> vertical_fields(int x)
	{
		for (int y = 0; y < H; y++)
			co_yield (*this)(x, y);
	}

	std::experimental::generator<Field> Y0_diagonal_fields(int xS)
	{
		for (int x = xS, y = 0; x < W && y < H; x++, y++)
			co_yield (*this)(x, y);
	}

	std::experimental::generator<Field> X0_diagonal_fields(int yS)
	{
		for (int x = 0, y = yS; x < W && y < H; x++, y++)
			co_yield (*this)(x, y);
	}

	//TODO: The remaining two generators missing

	std::experimental::generator<std::experimental::generator<Field>> all_directions()
	{
		if constexpr (R <= W)
			for (int y = 0; y < H; y++)
				co_yield horizontal_fields(y);

		if constexpr (R <= H)
			for (int x = 0; x < W; x++)
				co_yield vertical_fields(x);

		for (int y = 0; y <= H-R; y++)
			co_yield X0_diagonal_fields(y);

		for (int x = 1; x <= W-R; x++)
			co_yield Y0_diagonal_fields(x);
	}
	
	// At most one of X and O have winning strech of R in a row
	bool is_legal()
	{
		bool XStreakFound = false, OStreakFound = false;
		for (auto generator : all_directions())
		{
			// Cound Xs and Os in a row and if they both count to R,
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
					if (Os == R) { OStreakFound = true; if (XStreakFound) return false;	}
					break;
				}
			}
		}
		return false;
	}

	//get_all_legal_moves
	
	bool is_winning_move(Move<W,H> move)
	{
		Square<W, H> sq;
		int up = 0, down = 0;
		sq = move.square; while (sq.move_down() && (*this)(sq) == Field::Empty) { down++; }
		if constexpr (grav == Gravity::None)
		{
			sq = move.square; while (sq.move_up() && (*this)(sq) == Field::Empty) { up++; }
		}
		if (up + down + 1 >= R) return true;

		int left = 0, right = 0;
		sq = move.square; int left = 0; while (sq.move_left() && (*this)(sq) == Field::Empty) { left++; }
		if constexpr (grav != Gravity::Both)
		{
			sq = move.square; while (sq.move_right() && (*this)(sq) == Field::Empty) { right++; }
		}
		if (left + right + 1 >= R) return true;

		return false;
	}
};

class EndTableTTT
{
	int ply;
public:

};

template <int W, int H, int R, Gravity grav = Gravity::None>
class ConverterSimple
{
public:
	static constexpr SIZE size()
	{
		SIZE size = 1;
		for (int i = 0; i < W * H; i++)
			size *= 3;

		return size;
	}

	static SIZE index(TicTacToePosition<W, H, R, grav>& position)
	{
		SIZE index = 0;
		for (Field field : position.get_pieces())
		{
			index *= 3;
			index += (int8_t(field) + 1);
		}
		return index;
	}

	static TicTacToePosition<W, H, R, grav> create(SIZE index)
	{
		TicTacToePosition<W, H, R, grav> position;
		for (auto sq : position.get_squares_reversed())
		{
			position[sq] = Field((index % 3) - 1);
			index /= 3;
		}
		//for (Field& field : position.get_pieces_reversed())
		//{
		//	//field = Field((index % 3) - 1);
		//	field = Field::Empty;
		//}
		return position;
	}
};
