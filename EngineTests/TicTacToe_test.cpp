#include "pch.h"
#include "..\BoardGamesEngine\Games\TicTacToe.h"

#define ConverterTest(W,H,R)									\
TEST(TicTacToe, ConverterSimple ## W ## H) {					\
	typedef ConverterSimple<W,H,R> Converter;					\
	typedef TicTacToePosition<W,H,R> TTT;						\
	for (SIZE index = 0; index < Converter::size(); index++)	\
	{															\
		TTT ttt = Converter::create(index);						\
		SIZE index2 = Converter::index(ttt);					\
		EXPECT_EQ(index, index2);								\
	}															\
}

ConverterTest(2, 2, 2)

ConverterTest(3, 2, 2)

ConverterTest(3, 3, 3)