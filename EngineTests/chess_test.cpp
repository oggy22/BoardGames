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

		EXPECT_EQ(pos[square], other(pos[square_flip]))
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
		if (move == chess::Move("E1", "G1", chess::Piece::King))
			right_castle = true;
		if (move == chess::Move("E1", "C1", chess::Piece::King))
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
		if (move == chess::Move("E1", "G1", chess::Piece::King))
			right_castle = true;
		if (move == chess::Move("E1", "C1", chess::Piece::King))
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

TEST(chess, is_legal) {
	std::unordered_set<chess::Move> all_moves;
	for (int seed = 1; seed <= DebugRelease(10, 20); seed++)
	{
		chess::ChessPosition<true> pos;
		for (int ply = 0; ply < DebugRelease(100, 200); ply++)
		{
			// Get all legal moves
			std::unordered_set<chess::Move> legal_moves;
			for (auto move : pos.all_legal_moves())
			{
				legal_moves.insert(move);
				all_moves.insert(move);
			}

			// Verify each move is_legal iff contained in legal_moves
			for (auto move : all_moves)
			{
				if (legal_moves.contains(move))
					EXPECT_TRUE(pos.is_legal(move)) << move.chess_notation() << " " << pos.fen() << " " << ply;
				else
					EXPECT_FALSE(pos.is_legal(move)) << move.chess_notation() << " " << pos.fen() << " " << ply;
			}
			size_t number_of_moves;
			chess::Move move = random_move<chess::ChessPosition<true>, chess::Move>(pos, seed, number_of_moves);
			if (number_of_moves == 0)
				break;
			pos += move;
		}
	}
}

TEST(chess, minmax_vs_random) {
	for (int round = 1; round <= DebugRelease(1, 2); round++)
	{
		chess::ChessPosition<true> pos;
		pos.turn_on_material_tracking();
		pos.track_pgn();
		
		while (true)
		{
			// round1: White=minmax, Black=random
			// round2: White=random, Black=minmax
			// round3: White=minmax, Black=random
			// etc
			bool random = (round + pos.ply()) % 2 == 0;
			chess::Move move = random ?
				random_move<chess::ChessPosition<true>, chess::Move>(pos, 0) :
				MinMax<chess::ChessPosition<true>, chess::Move, KillerOptions::Fixed3, true>::FindBestMove(
					pos,
					DebugRelease(4, 6),
                    [](chess::ChessPosition<true>& pos) -> int { return pos.evaluate<1>(); });

			if (!move.is_valid())
			{
				EXPECT_TRUE(random) << "round:" << round << " game:" << pos.pgn() << std::endl;
				EXPECT_TRUE(pos.is_checked(pos.turn())) << pos.pgn() << std::endl;

				GTEST_LOG_(INFO) << pos.pgn() << std::endl;
				break;
			}

			EXPECT_TRUE(pos.is_legal(move));
			pos += move;
		}
		std::string pgn = pos.pgn();
	}
}

void print_stats(std::ofstream& file, stats stat, std::string name)
{
	file << name << ",";
	file << stat.min() << ",";
	file << stat.max() << ",";
	file << stat.avg() << ",";
	file << stat.n() << std::endl;
}

TEST(chess, flip_board_test_initial)
{
	chess::ChessPosition<true> pos;
	chess::ChessPosition<true> pos_copy = pos;
	pos.flip_board();
	EXPECT_EQ(pos, pos_copy);
}

TEST(chess, random_games) {
	stats total, wins, draws, white_wins, black_wins, in_mats, stalemates;
	
	std::ofstream file_csv;
	file_csv.open("all_games.csv");

	for (int seed = 1; seed <= DebugRelease(20, 500); seed++)
	{
		chess::ChessPosition<true> pos;
		pos.track_pgn();
		stats moves;
		int ply;
		for (ply = 0; ply < 15000; ply++)
		{
			Player player = pos.turn();
			EXPECT_FALSE(pos.is_checked(oponent(player)));

			// Get a random move and check that the board isn't affected
			chess::ChessPosition<true> pos_backup = pos;
			size_t number_of_moves;
			chess::Move move = random_move<chess::ChessPosition<true>, chess::Move>(pos, seed, number_of_moves);
			if (number_of_moves > 0)
				moves.add(int(number_of_moves));
			
			EXPECT_EQ(pos, pos_backup)
				<< pos.fen() << std::endl
				<< pos_backup.fen() << std::endl;
			if (pos != pos_backup)
			{
				std::string pgn = pos.pgn();
				FAIL();
			}

			if (!move.is_valid())
			{
				if (!pos.is_checked())
				{
					file_csv << "Stalemate,";
					draws.add(ply);
					stalemates.add(ply);
				}
				else
				{
					wins.add(ply);
					if (ply % 2 == 1)
					{
						file_csv << "WhiteWins,";
						white_wins.add(ply);
					}
					else
					{
						file_csv << "BlackWins,";
						black_wins.add(ply);
					}
				}
				break;
			}

			// Check if drawn with 2 kings only
			int count_pieces = 64 - pos.count_piece(chess::Piece::None);
			if (count_pieces == 2)
			{
				file_csv << "Insufficient material,";
				draws.add(ply);
				in_mats.add(ply);
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
						std::string pgn = pos.pgn();
						should_break = true;
						file_csv << "Insufficient material,";
						draws.add(ply);
						in_mats.add(ply);
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
				<< ply << " " << move.chess_notation() << std::endl
				<< pos.fen() << std::endl;
			EXPECT_EQ(1, pos.count_piece(chess::Piece::OtherKing))
				<< ply << " " << move.chess_notation() << std::endl
				<< pos.fen() << std::endl;
			EXPECT_FALSE(pos.is_checked(player))
				<< ply << " " << move.chess_notation() << std::endl
				<< pos.fen() << std::endl;
		}
		total.add(ply);

		std::string png = pos.pgn();
		std::ofstream file_game;
		file_game.open(std::string("game") + std::to_string(seed) + ".pgn");
		file_game << png;

		file_csv << (ply % 2 == 1 ? "White" : "Black") << ",";
		file_csv << ply << ",";
		file_csv << (ply + 1) / 2 << ",";
		file_csv << moves.min() << ",";
		file_csv << moves.max() << ",";
		file_csv << moves.avg() << ",";
		file_csv << pos.fen() << ",";
		file_csv << pos.pgn() << std::endl;
	}

	std::ofstream all_stats;
	all_stats.open("all_stats.csv");
	all_stats << ", min ply, max ply, avg ply, #" << std::endl;
	print_stats(all_stats, white_wins, "WhiteWins");
	print_stats(all_stats, black_wins, "BlackWins");
	print_stats(all_stats, wins, "Wins");

	print_stats(all_stats, in_mats, "Insufficient material");
	print_stats(all_stats, stalemates, "Stalemates");
	print_stats(all_stats, draws, "Draws");

	print_stats(all_stats, total, "Total");
}

TEST(chess, material)
{
	for (int seed = 0; seed < 10; seed++)
	{
		chess::ChessPosition<true> pos;
		pos.turn_on_material_tracking();
		size_t number_of_moves;
		for (int i = 0; i < 1000; i++)
		{
			chess::Move move = random_move<chess::ChessPosition<true>, chess::Move>(pos, seed, number_of_moves);
			if (!move.is_valid())
				break;
			pos += move;
			int material = pos.get_material();
			int material_full = pos.get_material_full();
			EXPECT_EQ(material, material_full) << " ply:" << i << " move:" << move.chess_notation();
		}
	}
}

TEST(chess, hash)
{
	chess::ChessPosition<true> pos;
	
	std::unordered_map<uint64_t, chess::ChessPosition<true>> hash_positions;
	for (int seed = 0; seed < 10; seed++)
	{
		chess::ChessPosition<true> pos;
		size_t number_of_moves;
		for (int i = 0; i < 1000; i++)
		{
			chess::Move move = random_move<chess::ChessPosition<true>, chess::Move>(pos, seed, number_of_moves);
			if (!move.is_valid())
				break;
			pos += move;

			auto hash = pos.get_hash<false> ();
			if (hash_positions.contains(hash))
			{
				auto pos2 = hash_positions[hash];
				EXPECT_EQ(pos.fen(), pos2.fen()) << "These two have same hash:" << pos.fen() << " and " << pos2.fen();
			}
			else
			{
				hash_positions[hash] = pos;
			}
		}
	}
}
