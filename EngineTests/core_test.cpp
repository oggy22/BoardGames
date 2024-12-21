#include "pch.h"
#include "..\BoardGamesEngine\core.h"

TEST(Core, oponent) {
  EXPECT_EQ(oponent(Player::First), Player::Second);
  EXPECT_EQ(oponent(Player::Second), Player::First);
}

TEST(Core, SquareBase1_1) {
	SquareBase<1, 1> sb1(0); EXPECT_FALSE(sb1.move_down());
	SquareBase<1, 1> sb2(0); EXPECT_FALSE(sb2.move_up());
	SquareBase<1, 1> sb3(0); EXPECT_FALSE(sb3.move_left());
	SquareBase<1, 1> sb4(0); EXPECT_FALSE(sb4.move_right());

	SquareBase<1, 1> sb5(0); EXPECT_FALSE(sb1.move_upleft());
	SquareBase<1, 1> sb6(0); EXPECT_FALSE(sb1.move_upright());
	SquareBase<1, 1> sb7(0); EXPECT_FALSE(sb1.move_downleft());
	SquareBase<1, 1> sb8(0); EXPECT_FALSE(sb1.move_downright());

	SquareBase<1, 1> inc(0); EXPECT_FALSE(++inc);
	SquareBase<1, 1> dec(0); EXPECT_FALSE(--dec);
}

TEST(Core, SquareBase8_8) {
	SquareBase<8, 8> inc(0);
	for (int i = 0; i < 63; i++)
		EXPECT_TRUE(++inc);
	EXPECT_FALSE(++inc);

	for (int x = 0; x < 8; x++)
		for (int y = 0; y < 8; y++)
		{
			SquareBase<8, 8> sq(x, y), sq2;
			sq2 = sq; EXPECT_EQ(sq2.move_left(), x != 0);
			sq2 = sq; EXPECT_EQ(sq2.move_right(), x != 7);
			sq2 = sq; EXPECT_EQ(sq2.move_up(), y != 7);
			sq2 = sq; EXPECT_EQ(sq2.move_down(), y != 0);
		}
}

TEST(Core, BoardBase8_8) {
	BoardBase<8, 8, int8_t> board;

	int count = 0;
	for (auto pair : board.get_pieces())
	{
		count++;
	}
	EXPECT_EQ(count, 8 * 8);


	count = 0;
	for (auto pair : board.get_pieces_reversed())
	{
		count++;
	}
	EXPECT_EQ(count, 8 * 8);
}

template <int W, int H>
void test_rotate(SquareBase<W, H> start)
{
	SquareBase<W, H> square180 = start.rotate_180();

	SquareBase<W, H> square = start.roate_90();
	EXPECT_TRUE(square.is_valid());

	for (int i = 1; i < 4; i++)
	{
		if (i == 2)
			EXPECT_EQ(square180, square);

		EXPECT_NE(start, square);
		square = square.roate_90();
		EXPECT_TRUE(square.is_valid());
	}
	EXPECT_EQ(start, square);
	
	EXPECT_EQ(start, square180.rotate_180());
}

TEST(Core, SquareBase_Rotate) {
	for (SquareBase<2, 2> sq(0); sq.is_valid(); ++sq)
	{
		test_rotate(sq);
	}

	for (SquareBase<3, 3> sq(0); sq.is_valid(); ++sq)
	{
		if (sq.x() == 1 && sq.y() == 1)
			continue;

		test_rotate(sq);
	}

	for (SquareBase<4, 4> sq(0); sq.is_valid(); ++sq)
	{
		test_rotate(sq);
	}
}

template <int W, int H>
void test_rotate(BoardBase<W, H, int>& start)
{
	for (SquareBase<W, H> sq(0); sq.is_valid(); ++sq)
	{
		start.square(sq) = int(sq);
	}

	BoardBase<W, H, int> board = start, board180 = start;
	board180.rotate_180();
	for (int i = 0; i < 4; i++)
	{
		if (i == 2)
			EXPECT_EQ(board180, board);

		board.roate_90();

		if (i == 3)
			break;

		EXPECT_NE(board, start);
	}

	EXPECT_EQ(board, start);
}

TEST(Core, BoardBase_Rotate) {
	BoardBase<2, 2, int> board;
	test_rotate<2, 2>(board);

	BoardBase<3, 3, int> board3;
	test_rotate<3, 3>(board3);

	BoardBase<4, 4, int> board4;
	test_rotate<4, 4>(board4);
}

// Verify that get_direction_to(sq) and move(dir) are consistent.
TEST(Core, Directions)
{
	for (int i = 0; i < 64; i++)
	{
		SquareBase<8, 8> sq_start(i);
		Direction dir = Direction(1);

		do
		{
			SquareBase<8, 8> sq = sq_start;

			bool first = true;
			while (sq.move(dir))
			{
				Direction dir2 = sq_start.get_direction_to<true>(sq);
				if (first || is_queen_direction(dir))
					EXPECT_EQ(dir, dir2);
				else
					EXPECT_NE(dir, dir2);

				first = false;
			}
		} while ((dir = next_direction(dir)) != Direction::none);
	}
}
