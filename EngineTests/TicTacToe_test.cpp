#include "pch.h"
#include "..\BoardGamesEngine\Games\TicTacToe.h"

TEST(TicTacToe, ConverterSimple) {
	typedef ConverterSimple<3, 3, 3> Converter;
	typedef TicTacToePosition<3, 3, 3> TTT;
	for (SIZE index = 0; index < Converter::size(); index++)
	{
		TTT ttt = Converter::create(index);
		SIZE index2 = Converter::index(ttt);
		EXPECT_EQ(index, index2);
	}
}