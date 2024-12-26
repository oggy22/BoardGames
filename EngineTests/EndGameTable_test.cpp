#include "pch.h"

#include <experimental/generator>

#include <functional>
#include "..\BoardGamesEngine\endgametable.h"
#include "..\BoardGamesEngine\Games\chess_converters.h"
#include "..\BoardGamesEngine\Games\Connect4.h"

TEST(endtable_test, chess)
{
	int count = 0;

	chess::ChessPosition pos;
	EndTable<chess::ConverterSimple>::FindBestMove(pos);
	EndTable<chess::ConverterReducing>::FindBestMove(pos);
}

TEST(endtable_test, Connect4)
{
	int count = 0;

	Connect4 connect4;
	EndTable<MNKGGravityConverter<7,6,4>>::FindBestMove(connect4);
}
