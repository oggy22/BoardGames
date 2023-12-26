#include "pch.h"
#include "..\BoardGamesEngine\Games\chess.h"

TEST(chess, all_squares) {
	int count = 0;
	for (chess::Square square : chess::Square::all_squares())
	{
		count++;
	}
	EXPECT_EQ(count, 64);
}

TEST(chess, FED) {
	chess::ChessPosition<true> pos(false);
	chess::ChessPosition<true> pos_fen(std::string("4k3/8/8/8/8/8/8/4K3"));
	std::string fen2 = pos_fen.fen();
	EXPECT_EQ(std::string("4k3/8/8/8/8/8/8/4K3"), fen2);// << pos << std::endl << pos_fen;

	chess::ChessPosition<true> pos_full(true);
	chess::ChessPosition<true> pos_full_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
	EXPECT_EQ(pos_full, pos_full_fen);
}

TEST(chess, init_position) {
	chess::ChessPosition<true> pos;
	chess::ChessPosition<true> pos_init;

	for (chess::Square square : chess::Square::all_squares())
	{
		chess::Square square_flip = square.flip_vertifaclly();

		EXPECT_EQ(pos[square], -pos[square_flip])
			<< "comparing " << square.chess_notation() << " and " << square_flip.chess_notation() << std::endl;
	}

	int count = 0;
	std::string all_moves;
	for (chess::Move move : pos.all_legal_moves())
	{
		EXPECT_EQ(pos, pos_init)
			<< "After " << move.chess_notation() << std::endl << pos;

		EXPECT_NE(chess::Piece::None, pos[move.from()])
			<< "move: " << move.chess_notation();
		EXPECT_EQ(chess::Piece::None, pos[move.to()])
			<< "move: " << move.chess_notation();

		all_moves += move.chess_notation() + " ";
		count++;
	}
	EXPECT_EQ(count, 20)	// fails with 13
		<< all_moves << std::endl;
}

TEST(chess, invert) {
	chess::ChessPosition<true> pos;
	chess::ChessPosition<true> pos_copy = pos;
	
	// Invert and expect not equal
	pos_copy.invert();
	EXPECT_EQ(pos, pos_copy);

	// Invert again and expect not equal
	pos_copy.invert();
	//EXPECT_EQ(pos, pos_copy);
}

static bool squares_visited[64];
int count_knight_moves(chess::Square sq)
{
	EXPECT_FALSE(squares_visited[sq]);
	int count = 0;
	chess::Square sq2;
	sq2 = sq; if (sq2.move_knight1()) count++;
	sq2 = sq; if (sq2.move_knight2()) count++;
	sq2 = sq; if (sq2.move_knight3()) count++;
	sq2 = sq; if (sq2.move_knight4()) count++;
	sq2 = sq; if (sq2.move_knight5()) count++;
	sq2 = sq; if (sq2.move_knight6()) count++;
	sq2 = sq; if (sq2.move_knight7()) count++;
	sq2 = sq; if (sq2.move_knight8()) count++;
	squares_visited[sq] = true;
	return count;
}

TEST(chess, square_knight_moves)
{
	for (int i = 0; i < 64; i++) squares_visited[i] = false;

	// Corner square have 2 knight moves
	EXPECT_EQ(2, count_knight_moves(chess::Square("A1")));
	EXPECT_EQ(2, count_knight_moves(chess::Square("H1")));
	EXPECT_EQ(2, count_knight_moves(chess::Square("A8")));
	EXPECT_EQ(2, count_knight_moves(chess::Square("H8")));

	// Squares next to corners have 3 knight moves
	EXPECT_EQ(3, count_knight_moves(chess::Square("A2")));
	EXPECT_EQ(3, count_knight_moves(chess::Square("B1")));
	EXPECT_EQ(3, count_knight_moves(chess::Square("A7")));
	EXPECT_EQ(3, count_knight_moves(chess::Square("B8")));
	EXPECT_EQ(3, count_knight_moves(chess::Square("H2")));
	EXPECT_EQ(3, count_knight_moves(chess::Square("G1")));
	EXPECT_EQ(3, count_knight_moves(chess::Square("H7")));
	EXPECT_EQ(3, count_knight_moves(chess::Square("G8")));

	// Middle border squares have 4 knight moves
	for (int i = 2; i < 6; i++)
	{
		EXPECT_EQ(4, count_knight_moves(chess::Square(i, 0)));
		EXPECT_EQ(4, count_knight_moves(chess::Square(i, 7)));
		EXPECT_EQ(4, count_knight_moves(chess::Square(0, i)));
		EXPECT_EQ(4, count_knight_moves(chess::Square(7, i)));
	}

	// Inner corner squares have 4 knight moves
	EXPECT_EQ(4, count_knight_moves(chess::Square("B2")));
	EXPECT_EQ(4, count_knight_moves(chess::Square("G2")));
	EXPECT_EQ(4, count_knight_moves(chess::Square("B7")));
	EXPECT_EQ(4, count_knight_moves(chess::Square("G7")));

	// Middle border squares have 6 knight moves
	for (int i = 2; i < 6; i++)
	{
		EXPECT_EQ(6, count_knight_moves(chess::Square(i, 1)));
		EXPECT_EQ(6, count_knight_moves(chess::Square(i, 6)));
		EXPECT_EQ(6, count_knight_moves(chess::Square(1, i)));
		EXPECT_EQ(6, count_knight_moves(chess::Square(6, i)));
	}

	// All the middle squares have all 8 knight moves
	for (char letter = 'C'; letter <= 'F'; letter++)
		for (char number = '3'; number <= '6'; number++)
		{
			chess::Square sq(letter, number);
			EXPECT_EQ(8, count_knight_moves(sq));
		}

	for (int i = 0; i < 64; i++)
		EXPECT_TRUE(squares_visited[i])
			<< chess::Square(i).chess_notation();
}
