#pragma once
#include <experimental/generator>

enum Field
{
	Empty = 0, X = 1, O=-1
};

template <int W, int H>
class Square
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

// W - Board Width
// H - Board Height
// R - Required stretche Length
template <int W, int H, int R>
class TicTacToePosition
{
	static_assert(W > 0 && H > 0 && R > 0);
	Field field[W * H];
public:
	Field operator[](Square<W,H> sq)
	{
		return field[sq];
	}

	TicTacToePosition()
	{
		for (int i = 0; i < W * H; i++)
			field[i] = Field::Empty;
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
				case Empty:
					Xs = Os = 0;
					break;
				case X:
					Xs++; Os = 0;
					if (Xs == R) { XStreakFound = true; if (OStreakFound) return false; }
					break;
				case O:
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
		sq = move.square; int up = 0; while (sq.move_up() && (*this)(sq) == Field::Empty) { up++; }
		sq = move.square; int down = 0; while (sq.move_down() && (*this)(sq) == Field::Empty) { down++; }
		if (up + down + 1 >= R) return true;
		sq = move.square; int left = 0; while (sq.move_left() && (*this)(sq) == Field::Empty) { left++; }
		sq = move.square; int right = 0; while (sq.move_right() && (*this)(sq) == Field::Empty) { right++; }
		if (left + right + 1 >= R) return true;
		return false;
	}
};
