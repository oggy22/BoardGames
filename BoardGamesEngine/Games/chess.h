#pragma once

#include <coroutine>
#include <experimental/generator>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <iostream>
#include <sstream>

#include "..\core.h"

namespace chess {

    enum class Piece : int8_t
    {
        None = 0,
        King = 1, Queen = 2, Rook = 3, Bishop = 4, Knight = 5, Pawn = 6,
        OtherKing = -1, OtherQueen = -2, OtherRook = -3, OtherBishop = -4, OtherKnight = -5, OtherPawn = -6
    };

#pragma region Material
    const int QUEEN_VALUE = 9;  // it may be 10 as well

    constexpr inline int piece_to_value(Piece piece)
    {
        switch (piece)
        {
        case Piece::None: return 0;
        case Piece::King: return 0;
        case Piece::Queen: return QUEEN_VALUE;
        case Piece::Rook: return 5;
        case Piece::Bishop: return 3;
        case Piece::Knight: return 3;
        case Piece::Pawn: return 1;
        case Piece::OtherKing: return 0;
        case Piece::OtherQueen: return -QUEEN_VALUE;
        case Piece::OtherRook: return -5;
        case Piece::OtherBishop: return -3;
        case Piece::OtherKnight: return -3;
        case Piece::OtherPawn: return -1;
        default: DCHECK_FAIL;
        }
    }

    static inline int decrease_value(int value)
    {
        if (value > 0)
            return value-1;
        if (value < 0)
			return value+1;
        DCHECK_FAIL;
    }
#pragma endregion

    static Piece other(Piece piece)
    {
        return Piece(0 - int8_t(piece));
    }

    static Piece abs(Piece piece)
    {
        return int8_t(piece) > 0 ? piece : other(piece);
    }

    static Player sgn(Piece piece)
    {
        if (int8_t(piece) > 0) return Player::First;
        if (int8_t(piece) < 0) return Player::Second;
        return Player(0);
    }

    static inline bool belongs_to(Piece piece, Player player)
    {
        return player == Player::First ? (int8_t(piece) > 0) : (int8_t(piece) < 0);
    };

    static inline char Piece_to_char(Piece piece)
    {
        switch (piece)
        {
        case Piece::King: return 'K';
        case Piece::Queen: return 'Q';
        case Piece::Rook: return 'R';
        case Piece::Bishop: return 'B';
        case Piece::Knight: return 'N';
        case Piece::Pawn: return 'P';

        case Piece::OtherKing: return 'k';
        case Piece::OtherQueen: return 'q';
        case Piece::OtherRook: return 'r';
        case Piece::OtherBishop: return 'b';
        case Piece::OtherKnight: return 'n';
        case Piece::OtherPawn: return 'p';

		case Piece::None: return '.';

        default: DCHECK_FAIL;
        }
    }

    static Piece char_to_piece(char c)
    {
        switch (c)
        {
        case 'K': return Piece::King;
        case 'Q': return Piece::Queen;
        case 'R': return Piece::Rook;
        case 'B': return Piece::Bishop;
        case 'N': return Piece::Knight;
        case 'P': return Piece::Pawn;

        case 'k': return Piece::OtherKing;
        case 'q': return Piece::OtherQueen;
        case 'r': return Piece::OtherRook;
        case 'b': return Piece::OtherBishop;
        case 'n': return Piece::OtherKnight;
        case 'p': return Piece::OtherPawn;

        default: DCHECK_FAIL;
        }
    }

    class Square : public SquareBase<8, 8>
    {
    public:
        constexpr Square() : SquareBase() { } // invalid square
        constexpr Square(SquareBase sq) : SquareBase(sq) { }
        constexpr Square(int n) : SquareBase(n) { }
        constexpr Square(int x, int y) : SquareBase(y * 8 + x) {}
        constexpr Square(char letter, char number) : SquareBase(int(letter -'A'), int(number - '1')) { }
        constexpr Square(const char s[3]) : SquareBase(s) { }

		Square(std::string s) : Square(s[0] - 'a', s[1] - '1')
		{
			DCHECK(s.length() == 2);
			DCHECK(s[0] >= 'a' && s[0] <= 'h');
			DCHECK(s[1] >= '1' && s[1] <= '8');
		}

        static std::experimental::generator<Square> all_squares()
        {
            Square square(0);
            do
            {
                co_yield square;
            } while (++square);
        }

        Square flip_horizontally() { return Square(8 - x() - 1, y()); }
        Square flip_vertifaclly() { return Square(x(), 8 - y() - 1); }

        bool operator==(const Square& other) const { return _square == other._square; }
        bool operator!=(const Square& other) const { return _square != other._square; }

        int row() { return _square >> 3; }
        int column() { return _square & 0b111; }
        std::string name()
        {
            char chRow = '1' + column();
            char chCol = 'A' + row();
            return "" + chCol + chRow;
        }

        bool move_knight1() { return move_upleft() && move_up(); }
        bool move_knight2() { return move_upleft() && move_left(); }
        bool move_knight3() { return move_upright() && move_up(); }
        bool move_knight4() { return move_upright() && move_right(); }

        bool move_knight5() { return move_downleft() && move_down(); }
        bool move_knight6() { return move_downleft() && move_left(); }
        bool move_knight7() { return move_downright() && move_down(); }
        bool move_knight8() { return move_downright() && move_right(); }

        std::experimental::generator<Square> knight_moves()
        {
            Square sq;
            sq = (*this); if (sq.move_knight1()) co_yield sq;
            sq = (*this); if (sq.move_knight2()) co_yield sq;
            sq = (*this); if (sq.move_knight3()) co_yield sq;
            sq = (*this); if (sq.move_knight4()) co_yield sq;

            sq = (*this); if (sq.move_knight5()) co_yield sq;
            sq = (*this); if (sq.move_knight6()) co_yield sq;
            sq = (*this); if (sq.move_knight7()) co_yield sq;
            sq = (*this); if (sq.move_knight8()) co_yield sq;
        }

        operator int() { return int(_square); }

        int king_distance(Square sq)
        {
            int dx = std::abs(x() - sq.x());
            int dy = std::abs(y() - sq.y());
            return std::max(dx, dy);
        }

        bool operator++()
        {
            return SquareBase::operator++();
        }

        bool operator--()
        {
            return SquareBase::operator--();
        }

        void operator+=(int inc)
        {
            _square += inc;
        }

        std::string chess_notation(bool lowercase = false) const
        {
            return SquareBase<8,8>::chess_notation(lowercase);
        }
    };

#define S(str) chess::Square(str)

    class Move
    {
    public:
        bool is_valid() const { return _from.is_valid(); }
        constexpr Move() : _from() { }    // invalid move
        Move(Square from, Square to, Piece piece, Piece captured = Piece::None, Piece promotion = Piece::None) :
            _from(from),
            _to(to),
			_piece(piece),
            _captured(captured),
            _promotion(promotion)
        {
            DCHECK(piece != Piece::None);
            DCHECK(captured != Piece::King && captured != Piece::OtherKing);
            DCHECK(from.is_valid());
            DCHECK(to.is_valid());
            DCHECK(from != to);
        }
        bool operator==(const Move&) const = default;
        void flip_rows();
        void flip_columns();
        void flip_rows_and_columns();
        friend std::ostream& operator<<(std::ostream& os, Move move);
        Square from() const { return _from; }
        Square to() const { return _to; }
        Piece piece() const { return _piece; }
        Piece captured() const { return _captured; }
        Piece promotion() const { return _promotion; }
        std::string chess_notation() const
        {
            std::string ret;
            ret.reserve(5);
            ret += _from.chess_notation();
            ret += "-";
            ret += _to.chess_notation();
            ret += ""; // captured
            return ret;
        }

        int material_change()
        {
            int ret = 0;
            ret -= piece_to_value(_captured);
            if (_promotion != Piece::None)
                ret += decrease_value(piece_to_value(_promotion));
            return ret;
        }

    private:
        Square _from, _to;
        Piece _piece;
        Piece _captured;
        Piece _promotion;
    };

    class ChessPosition;

    class ConverterSimple;

    class ChessPosition : public BoardBase<8, 8, Piece>
    {
        friend class ConverterSimple;
        bool _track_pgn = false;
        
        bool construct_from_pgn(std::string pgn);

        bool construct_from_fen(std::string fen);
    public:
		using Move = chess::Move;
        ChessPosition(bool only_kings = false)
        {
            for (int i = 0; i < 64; i++)
                table[i] = Piece::None;
            King1 = S("E1");   table[King1] = Piece::King;
            King2 = S("E8");   table[King2] = Piece::OtherKing;
            if (only_kings)
                return;

            table[S("D1")] = Piece::Queen;
            table[S("D8")] = Piece::OtherQueen;

            // Pawns
            for (Square pw("A2"), pb("A7"); ;)
            {
                table[pw] = Piece::Pawn;
                table[pb] = Piece::OtherPawn;
                bool inside = pw.move_right() && pb.move_right();
                if (!inside)
                    break;
            }

            Square BottomLeft("A1"), BottomRight("H1");
            Square TopLeft("A8"), TopRight("H8");
            Piece pieces[3] = { Piece::Rook, Piece::Knight, Piece::Bishop };

            for (int i = 0; i < 3; i++)
            {
                // Set squares
                table[BottomLeft] = pieces[i];
                table[BottomRight] = pieces[i];
                table[TopLeft] = other(pieces[i]);
                table[TopRight] = other(pieces[i]);

                // Move squares
                BottomLeft.move_right(); BottomRight.move_left();
                TopLeft.move_right(); TopRight.move_left();
            }
            
            initialize_transposition_tables();
        }

#pragma region FEN
        ChessPosition(const std::string& pgn_or_fen) : ChessPosition(false)
        {
            // If it is a FEN it has exactly 7 slashes
            if (std::count(pgn_or_fen.begin(), pgn_or_fen.end(), '/') == 7)
                construct_from_fen(pgn_or_fen);
            else
                construct_from_pgn(pgn_or_fen);
        }
        std::string fen() const;

#pragma endregion

        int32_t Evaluate() const { return 0; }

#pragma region Material

        int get_material_full() const;

        int get_material() const;

        bool track_material_on = false;
		
		void turn_on_material_tracking()
		{
			if (track_material_on)
				return;

            track_material_on = true;
			tracked_material = get_material_full();
        }

        void turn_off_material_tracking()
        {
			track_material_on = false;
        }

        int tracked_material = 0;
#pragma endregion

        // Increments for each white piece and
        // decrements for each black piece.

        bool play_if_legal(Move move);

        #pragma region Evaluation
        int king_protection_eval(Square king) const;

        int king_protection_eval() const;

        int count_center_pieces() const;

        template <
            int material = 1, //Qs*9+Rs*5+Bs*3+Ks*3+Ps*1
            //int almost_promoted = 0, // white pawns on 7th rank - black pawns on 2nd rank
            int king_protection = 0, // count pieces around the king
            //int coverage = 0, // count squares controlled by white - black
            int legal_moves = 0, // number of legal moves
            int control_center = 0 // number of pieces in the center
        >
        int evaluate() const
        {
            int ret = 0;
            if constexpr (material > 0) ret += material * get_material();
            //if const (almost_promoted)ret += almost_promoted * almost_promoted_evaluate();
            if constexpr (king_protection > 0) ret += king_protection * king_protection_eval();
            //if const (coverage)ret += coverage * coverage_evaluate();
            if constexpr (legal_moves > 0) ret += legal_moves * count_all_legal_moves() * int(turn());
            if constexpr (control_center > 0) ret += control_center * count_center_pieces();
            return ret;
        }
        #pragma endregion

        bool operator==(const ChessPosition& other) const
        {
            if (King1 != other.King1 || King2 != other.King2)
                return false;
            if (_ply != other._ply)
                return false;
            for (int i = 0; i < 64; i++)
            {
                if (table[i] != other.table[i])
                    return false;
            }
            return true;
        }

#pragma region PGN
        void track_pgn() { _track_pgn = true;  }

        bool rook_move(Square sq1, Square sq2) const;
        bool bishop_move(Square sq1, Square sq2) const;
        bool queen_move(Square sq1, Square sq2) const;

        std::string pgn() const;

        std::vector<std::string> pgns;

        void turn_off_all_trackings()
		{
			_track_pgn = false;
		}

        std::string move_to_pgn(Move move) const;

        template <Piece abs_piece>
        Move _pgn_to_move(Square sq_to, SquareRequirements req = SquareRequirements("")) const;

        Move pgn_to_move(std::string str) const;
#pragma endregion
        
        void invert()
        {
            for (int column = 0; column < 8; column++)
            {
                Square bottom(column, 0);
                Square top(column, 7);
                for (int i = 0; i < 4; i++)
                {
                    Piece bottomPiece = (*this)[bottom];
                    Piece topPiece = (*this)[top];
                    table[bottom] = other(topPiece);
                }
                BoardBase::invert();
            }
        }

        // Pawns one step before promotion?
        bool almost_promotion() const
        {
            Square sq = Square(8);
            do { if ((*this)[sq] == Piece::OtherPawn) return true; ++sq; } while (sq < Square(16));
            sq = Square(64 - 16);
            do { if ((*this)[sq] == Piece::Pawn) return true; ++sq; } while (sq < Square(64 - 8));
            return false;
        }

        bool any_pawns() const;
        
        bool is_legal(Move move) const
        {
            auto* nonConstThis = const_cast<chess::ChessPosition*>(this);

            bool legal = nonConstThis->play_if_legal(move);
            if (!legal)
                return false;
            (*nonConstThis) -= move;
            return true;
        }

        bool is_reverse_legal(Move move) const { return square(move.from()) == Piece::None && square(move.to()) != Piece::None; }

        Piece operator[](Square square) const { return table[square]; }

#pragma region Move handling
        bool castle_rook(Piece king_piece, chess::Square king, Piece rook_piece, chess::Square rook1, chess::Square rook2)
        {
            if (square(king) == king_piece)
            {
                DCHECK(square(rook1) == rook_piece);
                DCHECK(square(rook2) == chess::Piece::None);
                square(rook1) = chess::Piece::None;
                square(rook2) = rook_piece;
                return true;
            }
            return false;
        }

        void operator+=(Move move);

        void operator-=(Move move);
#pragma endregion

        std::experimental::generator<Move> all_legal_moves_played();

        bool right_castle(Square king, Square rook, Player player) const;

        bool left_castle(Square king, Square rook, Player player) const;
        
        std::experimental::generator<Move> all_legal_moves() const;

        int count_all_legal_moves() const
        {
            int count = 0;
            for (auto m : all_legal_moves())
            {
                count++;
            }
            return count;
        }

		bool any_legal_moves() const
        {
            for (auto m : all_legal_moves())
            {
                return true;
            }
            return false;
        }

        bool is_checked() const
        {
            return is_checked(turn());
        }

        bool is_checked(Player player) const
        {
            return is_controlled_by(player == Player::First ? King1 : King2, oponent(player));
        }

        template <Direction dir>
        inline bool is_controlled_by_from_direction(Square start, Player player) const
        {
            Square sq = start;
            bool first_square = true;
            while (sq.move<dir>())
            {                 
                Piece piece = square(sq);
                if (piece != Piece::None) {
                    if (!belongs_to(piece, player))
                        return false;
                    piece = abs(piece);
                    if constexpr (
                        dir == Direction::down ||
                        dir == Direction::up ||
                        dir == Direction::left ||
                        dir == Direction::right)
                    {
						if (piece == Piece::Queen || piece == Piece::Rook)
							return true;
                    }
					else if constexpr (
                        dir == Direction::upleft ||
						dir == Direction::upright ||
						dir == Direction::downleft ||
						dir == Direction::downright)
                    {
                        if (piece == Piece::Queen || piece == Piece::Bishop)
                            return true;

                        if (first_square && piece == Piece::Pawn)
                        {
							if (player == Player::First && (dir == Direction::downleft || dir == Direction::downright))
								return true;
							if (player == Player::Second && (dir == Direction::upleft || dir == Direction::upright))
                                return true;
                            return false;
                        }
                    }
                    else if constexpr (
                        dir == Direction::knight1 ||
                        dir == Direction::knight2 ||
                        dir == Direction::knight3 ||
                        dir == Direction::knight4 ||
                        dir == Direction::knight5 ||
                        dir == Direction::knight6 ||
                        dir == Direction::knight7 ||
                        dir == Direction::knight8
                        )
                    {
                        if (piece == Piece::Knight)
                            return true;
                        return false;
                    }
                    else static_assert(false, "Unknown direction");

                    return false;
                }

                first_square = false;

                // Knights moves are not incremental
				if ((dir & Direction::all_knights) != Direction::none)
                    return false;
            };
            return false;
        }

        bool is_controlled_by_from_direction(Square start, Player player, Direction dir) const;

        bool is_controlled_by(Square start, Player player) const;

        constexpr bool easycheck_winning_move(Move move) const
        {
            return false;
        }

        bool is_check_mate() const { return is_checked(turn()) && !any_legal_moves(); }

		bool is_lost() const { return is_check_mate(); }

        void flip_rows()
        {
            Square bottom(0, 0);
            do
            {
                Square top(bottom.x(), 7 - bottom.y());
                do
                {
                    std::swap(square(bottom), square(top));
                    top.move_right();
                } while (bottom.move_right());
            } while (bottom.y() < 4);
            King1 = Square(King1.x(), 7 - King1.y());
            King2 = Square(King2.x(), 7 - King2.y());
        }

        /// <summary>
		/// Flip rows and invert pieces.
		/// White pawns at 7th rank convert to black pawns at 2nd rank.
		/// White king at initial position converts to black king at initial position.
        /// By this process the initial position transforms into itself.
        /// </summary>
        void flip_board()
        {
            Square bottom(0, 0);
            do
            {
                Square top(bottom.x(), 7 - bottom.y());
                do
                {
                    std::swap(square(bottom), square(top));
                    square(bottom) = other(square(bottom));
                    square(top) = other(square(top));

                    top.move_right();
                } while (bottom.move_right());
            } while (bottom.y() < 4);
            King1 = Square(King1.x(), 7 - King1.y());
            King2 = Square(King2.x(), 7 - King2.y());
			std::swap(King1, King2);
            this->move();
            _ply--;
        }

        std::experimental::generator<std::pair<char, Square>> get_piece_squares() const
        {
            Square square;
            co_yield std::pair<char, Square>('K', King1);
            square = Square(); do { if (table[square] == Piece::Queen)  co_yield std::pair<char, Square>('Q', square); } while (++square);
            square = Square(); do { if (table[square] == Piece::Rook)   co_yield std::pair<char, Square>('R', square); } while (++square);
            square = Square(); do { if (table[square] == Piece::Bishop) co_yield std::pair<char, Square>('B', square); } while (++square);
            square = Square(); do { if (table[square] == Piece::Pawn)   co_yield std::pair<char, Square>('P', square); } while (++square);
            co_yield std::pair<char, Square>('K', King2);
            square = Square(); do { if (table[square] == Piece::OtherQueen)  co_yield std::pair<char, Square>('Q', square); } while (++square);
            square = Square(); do { if (table[square] == Piece::OtherRook)   co_yield std::pair<char, Square>('R', square); } while (++square);
            square = Square(); do { if (table[square] == Piece::OtherBishop) co_yield std::pair<char, Square>('B', square); } while (++square);
            square = Square(); do { if (table[square] == Piece::OtherPawn)   co_yield std::pair<char, Square>('P', square); } while (++square);
        }

        void get_numbers(int& q1, int& r1, int& b1, int& n1, int& q2, int& r2, int& b2, int& n2)
        {
            q1 = r1 = b1 = n1 = q2 = r2 = b2 = n2 = 0;
            Square square;
            do
            {
                switch (table[square])
                {
                case Piece::Queen:  q1++; break;
                case Piece::Rook:   r1++; break;
                case Piece::Bishop: b1++; break;
                case Piece::Knight: n1++; break;
                case Piece::OtherQueen:  q2++; break;
                case Piece::OtherRook:   r2++; break;
                case Piece::OtherBishop: b2++; break;
                case Piece::OtherKnight: n2++; break;
                default:
                    break;
                }
            } while (++square);
        }

        std::string get_type() const
        {
            std::string ret;
            for (auto piece : get_pieces())
            {
                ret += Piece_to_char(piece);
            }
            return ret;
        }

        static friend std::ostream& operator<<(std::ostream& os, const ChessPosition& position)
        {
            Square row("A8");
            do
            {
                Square column = row;
                do
                {
                    Piece piece = position[column];
                    char c = Piece_to_char(piece);
                    os << c;
                } while (column.move_right());
                os << std::endl;
            } while (row.move_down());
            return os;
        }

#pragma region Transpositional tables
        inline static uint64_t hash_white[64][6], hash_black[64][6], hash_turn;

        inline static bool transposition_tables_initialized = false;

        static void initialize_transposition_tables();

        consteval static bool implements_hash() { return true; }

		template <bool include_turn = false>
        uint64_t get_hash() const
        {
            uint64_t ret = 0;
            for (int i = 0; i < 64; i++)
            {
				auto piece = table[i];
				if (piece == Piece::None)
					continue;
				if (belongs_to(piece, Player::First))
					ret ^= hash_white[i][int8_t(abs(piece))];
				else
					ret ^= hash_black[i][int8_t(abs(piece))];
            }

			if constexpr (include_turn)
			{
				if (turn() == Player::Second)
					ret ^= hash_turn;
			}

            return ret;
        }
#pragma endregion
    private:
        Square King1, King2;
    };
}

namespace std {
    template <>
    struct hash<chess::Move> {
        std::size_t operator()(const chess::Move& move) const noexcept {
            std::size_t h1 = std::hash<int>{}(move.from());
            std::size_t h2 = std::hash<int>{}(move.to());
            //TODO: might need to combine other move elements
            return h1 ^ (h2 << 1); // Combine the hashes
        }
    };
}
