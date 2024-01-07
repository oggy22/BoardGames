#include "pch.h"
#include "..\BoardGamesEngine\Games\chess.h"
#include "..\BoardGamesEngine\Algorithms.h"

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
	EXPECT_FALSE(pos.is_checked(Player::First));
	EXPECT_FALSE(pos.is_checked(Player::Second));

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
		EXPECT_NE(chess::Piece::None, pos[move.from()])
			<< "move: " << move.chess_notation();
		EXPECT_EQ(chess::Piece::None, pos[move.to()])
			<< "move: " << move.chess_notation();

		all_moves += move.chess_notation() + " ";

		pos += move;
		EXPECT_FALSE(pos.is_checked(Player::First));
		EXPECT_FALSE(pos.is_checked(Player::Second));
		int count2 = 0;
		for (chess::Move move2 : pos.all_legal_moves())
		{
			pos += move2;
			EXPECT_FALSE(pos.is_checked(Player::First));
			EXPECT_FALSE(pos.is_checked(Player::Second));
			count2++;
			pos -= move2;
		}
		pos -= move;
		EXPECT_EQ(count2, 20);
		count++;
	}

	chess::ChessPosition<true> pos_init;
	EXPECT_EQ(pos, pos_init);

	EXPECT_EQ(count, 20)
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

TEST(chess, castle) {
	chess::ChessPosition<true> pos(std::string("4k3/8/8/8/8/8/8/R3K2R"));
	chess::ChessPosition<true> copy(pos);

	bool left_castle = false, right_castle = false;
	for (chess::Move move : pos.all_legal_moves())
	{
		EXPECT_EQ(pos, copy)
			<< "Last move: " << move.chess_notation() << std::endl;
		if (move == chess::Move("E1", "G1"))
			right_castle = true;
		if (move == chess::Move("E1", "C1"))
			left_castle = true;
	}
	EXPECT_TRUE(right_castle);
	EXPECT_TRUE(left_castle);
}

TEST(chess, castle_none) {
	chess::ChessPosition<true> pos(std::string("4k3/8/8/8/8/8/1q4q1/R3K2R"));
	chess::ChessPosition<true> copy(pos);

	bool left_castle = false, right_castle = false;
	for (chess::Move move : pos.all_legal_moves())
	{
		EXPECT_EQ(pos, copy)
			<< "Last move: " << move.chess_notation() << std::endl;
		if (move == chess::Move("E1", "G1"))
			right_castle = true;
		if (move == chess::Move("E1", "C1"))
			left_castle = true;
	}
	EXPECT_FALSE(right_castle);
	EXPECT_FALSE(left_castle);
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

TEST(chess, minmax_vs_random) {
	for (int round = 1; round <= DebugRelease(20, 4); round++)
	{
		chess::ChessPosition<true> pos;
		pos.track_png();
		
		for (int ply = 0; ply < DebugRelease(100, 50); ply++)
		{
			// round1: White=minmax, Black=random
			// round2: White=random, Black=minmax
			// round3: White=minmax, Black=random
			// etc
			bool random = (round + pos.ply()) % 2 == 0;
			chess::Move move = random ?
				random_move<chess::ChessPosition<true>, chess::Move>(pos, 0) :
				MinMax<chess::ChessPosition<true>, chess::Move>::FindBestMove(pos, DebugRelease(2, 4));

			if (!move.is_valid())
			{
				EXPECT_TRUE(random) << "round:" << round << " game:" << pos.png() << std::endl;
				EXPECT_TRUE(pos.is_checked(pos.turn())) << pos.png() << std::endl;
				break;
			}

			EXPECT_TRUE(pos.is_legal(move));
			pos += move;
		}
		std::string pgn = pos.png();
	}
}


TEST(chess, random_games) {
	for (int seed = 1; seed < DebugRelease(20, 500); seed++)
	{
		chess::ChessPosition<true> pos;
		pos.track_png();
		for (int i = 0; i < 1500; i++)
		{
			Player player = pos.turn();
			EXPECT_FALSE(pos.is_checked(oponent(player)));

			// Get a random move and check that the board isn't affected
			chess::ChessPosition<true> pos_backup = pos;
			chess::Move move = random_move<chess::ChessPosition<true>, chess::Move>(pos, seed);
			EXPECT_EQ(pos, pos_backup)
				<< pos.fen() << std::endl
				<< pos_backup.fen() << std::endl;
			if (pos != pos_backup)
			{
				std::string png = pos.png();
				FAIL();
			}

			if (!move.is_valid())
			{
				std::string png = pos.png();
				break;
			}

			// Check if drawn with 2 kings only
			int count_pieces = 64 - pos.count_piece(chess::Piece::None);
			if (count_pieces == 2)
			{
				std::string png = pos.png();
				break;
			}
			if (count_pieces == 3)
			{
				bool should_break = false;
				for (chess::Piece piece : pos.get_pieces())
				{
					if (abs(piece) == chess::Piece::Bishop
						|| abs(piece) == chess::Piece::Knight)
					{
						std::string png = pos.png();
						should_break = true;
						break;
					}
				}
				if (should_break)
					break;
			}
			
			EXPECT_TRUE(pos.is_legal(move));

			pos += move;

			// No pawns on the 1st and 8th rows
			chess::Square first("A1"), last("A8");
			do
			{
				chess::Piece f = abs(pos[first]), l = abs(pos[last]);
				EXPECT_NE(chess::Piece::Pawn, f);
				EXPECT_NE(chess::Piece::Pawn, l);
			} while (first.move_right() && last.move_right());

			// Only 2 kings and no check to the player played
			EXPECT_EQ(1, pos.count_piece(chess::Piece::King))
				<< i << " " << move.chess_notation() << std::endl
				<< pos.fen() << std::endl;
			EXPECT_EQ(1, pos.count_piece(chess::Piece::OtherKing))
				<< i << " " << move.chess_notation() << std::endl
				<< pos.fen() << std::endl;
			EXPECT_FALSE(pos.is_checked(player))
				<< i << " " << move.chess_notation() << std::endl
				<< pos.fen() << std::endl;
		}
	}
}
