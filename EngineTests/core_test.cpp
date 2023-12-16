#include "pch.h"
#include "..\BoardGamesEngine\core.h"

TEST(Core, oponent) {
  EXPECT_EQ(oponent(Player::First), Player::Second);
  EXPECT_EQ(oponent(Player::Second), Player::First);
}

TEST(Core, SquareBase1_1) {
	SquareBase<1, 1> sb1; EXPECT_FALSE(sb1.move_down());
	SquareBase<1, 1> sb2; EXPECT_FALSE(sb2.move_up());
	SquareBase<1, 1> sb3; EXPECT_FALSE(sb3.move_left());
	SquareBase<1, 1> sb4; EXPECT_FALSE(sb4.move_right());

	SquareBase<1, 1> sb5; EXPECT_FALSE(sb1.move_upleft());
	SquareBase<1, 1> sb6; EXPECT_FALSE(sb1.move_upright());
	SquareBase<1, 1> sb7; EXPECT_FALSE(sb1.move_downleft());
	SquareBase<1, 1> sb8; EXPECT_FALSE(sb1.move_downright());

	SquareBase<1, 1> inc; EXPECT_FALSE(++inc);
	SquareBase<1, 1> dec; EXPECT_FALSE(--dec);
}

TEST(Core, SquareBase8_8) {
	SquareBase<8, 8> inc;
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

	//for (auto piece
}