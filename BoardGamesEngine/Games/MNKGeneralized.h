#pragma once

#include <experimental/generator>
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
	static_assert(R <= W && R <= H, "R may not be greater than dimensions");
};

template <
	int W, 
	int H, 
	int R				// the number of required <tokens> in a row to win
>
class MNK : public MNKGeneralized<W, DimProp::None, H, DimProp::None, R, false, false, false>
{
public:
	std::experimental::generator<SquareBase<W, H>> get_all_squares() const
	{
		for (int i = 0; i < W * H; i++)
			if (this->get(i) == Field::Empty)
				co_yield SquareBase<W, H>(i);
	}
};

template <
	int W,
	int H,
	int R				// the number of required <tokens> in a row to win
>
class MNKGravity : public MNKGeneralized<W, DimProp::None, H, DimProp::Gravity, R, false, false, false>
{

};

template<int W, int H>
struct FieldsOptimized {
	static int distance(SquareBase<W, H> sq)
	{
		return
			abs(W - 1 - 2*sq.x()) +
			abs(H - 1 - 2*sq.y());
	}

	constexpr FieldsOptimized() : arr()
	{
		// Initialize
		for (int i = 0; i < W * H; ++i)
			arr[i] = SquareBase<W, H>(i);

		// Sort them
		for (int i = 0; i < W * H; ++i)
			for (int j = i + 1; j < W * H; ++j)
			{
				if (distance(arr[i]) > distance(arr[j]))
					std::swap(arr[i], arr[j]);
			}
	}
	static SquareBase<W,H> arr[W * H];
};
