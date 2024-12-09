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
    const int QUEEN_VALUE = 9;

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

    static int decrease_value(int value)
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

    static bool belongs_to(Piece piece, Player player)
    {
        return player == Player::First ? (int8_t(piece) > 0) : (int8_t(piece) < 0);
    };

    static char Piece_to_char(Piece piece)
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
        Move() : _from() { }    // invalid move
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

    template <bool QPO> // Queen Promotions Only
    class ChessPosition;

    template <bool QPO>
    class ConverterSimple;

    template <bool QPO> // Queen Promotions Only
    class ChessPosition : public BoardBase<8, 8, Piece>
    {
        friend class ConverterSimple<QPO>;
        bool _track_pgn = false;

        bool construct_from_pgn(std::string pgn)
        {
            std::stringstream ss(pgn);
            int expected_number = 1;
            do
            {
                // Get number
                int number;
                ss >> number;
                if (number != expected_number++)
                    return false;

                // Get dot after the number
                char dot;
                ss >> dot;
                if (dot != '.')
                    return false;
                if (ss.eof())
                    return false;

                // Get white move
                std::string wmove;
                ss >> wmove;
                Move move = this->pgn_to_move(wmove);
                (*this) += move;
                if (ss.eof())
                    return false;

                // Get black move
                std::string bmove;
                ss >> bmove;
                move = this->pgn_to_move(bmove);
                (*this) += move;
            } while (!ss.eof());
            return false;
        }

        bool construct_from_fen(std::string fen)
        {
            Square sq(0);
            for (char c : fen)
            {
                if (c == '/')
                    continue;

                if (c >= '1' && c <= '8')
                {
                    int count = c - '0';
                    for (int i = 0; i < count; i++)
                    {
                        table[sq] = Piece::None;
                        ++sq;
                    }
                    continue;
                }
                char upper_c = toupper(c);
                Piece piece = char_to_piece(upper_c);
                if (upper_c != c)
                    piece = other(piece);

                if (c == 'K') King1 = sq;
                if (c == 'k') King2 = sq;

                table[sq] = piece;
                ++sq;
            }
            this->flip_rows();
            return true;
        }

    public:
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
            
            initialize_transpositional_tables();
        }

#pragma region FEN
        ChessPosition(const std::string& png_or_fen) : ChessPosition(false)
        {
            // If it is a FEN it has exactly 7 slashes
            if (std::count(png_or_fen.begin(), png_or_fen.end(), '/') == 7)
                construct_from_fen(png_or_fen);
            else
                construct_from_pgn(png_or_fen);
        }

        std::string fen() const
        {
            auto nonConstThis = const_cast<ChessPosition*>(this);

            nonConstThis->flip_rows();
            Square sq(0);
            std::string fen;
            do
            {
                if (table[sq] != Piece::None)
                {
                    fen += Piece_to_char(table[sq]);
                }
                else
                {
                    if (fen.length() > 0 && '1' <= fen.back() && fen.back() <= '7')
                        fen.back()++;
                    else
                        fen += '1';
                }

                if (sq.x() == 7)
                    fen += '/';
            } while (++sq);
            nonConstThis->flip_rows();

            return fen.substr(0, fen.length() - 1);
        }
#pragma endregion

        int32_t Evaluate() const { return 0; }

#pragma region Material
        int get_material_full() const
        {
            int ret = 0;
            for (int i = 0; i < 64; i++)
            {
                Piece piece = table[i];
                ret += piece_to_value(piece);
            }
			return ret;
        }

        int get_material() const
		{
            if (track_material_on)
                return tracked_material;

			return get_material_full();
		}

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
        int king_protection_eval(Square king)
        {
            int ret = 0;
            int minx = std::max(0, king.x() - 1);
            int maxx = std::min(7, king.x() + 1);
            int miny = std::max(0, king.y() - 1);
            int maxy = std::min(7, king.y() + 1);
            for (int x = minx; x<=maxx; x++)
                for (int y = miny; y <= maxy; y++)
                {
                    Piece piece = table[Square(x, y)];
                    ret += int(sgn(piece));
                }
            return ret;
        }

        int king_protection_eval()
        {
            return king_protection_eval(this->King1)
                + king_protection_eval(this->King2);
        }

        int count_center_pieces()
        {
			return
                int(sgn(table[S("D4")])) +
                int(sgn(table[S("E4")])) +
                int(sgn(table[S("D5")])) +
                int(sgn(table[S("E5")]));
        }

        template <
            int material = 1, //Qs*9+Rs*5+Bs*3+Ks*3+Ps*1
            //int almost_promoted = 0, // white pawns on 7th rank - black pawns on 2nd rank
            int king_protection = 0, // count pieces around the king
            //int coverage = 0, // count squares controlled by white - black
            int legal_moves = 0, // number of legal moves
			int control_center = 0 // number of pieces in the center
        >
        int evaluate()
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

        bool play_if_legal(Move move)
        {
            //BUG: this shouldn't be here, but minmax provides invalid moves sometimes
            if (!move.is_valid())
                return false;

            Piece abs_piece = abs(move.piece());
			// The assumption is that the move is legal in one chess position,
			// so there is a number of things that don't have to be checked:
			DCHECK(move.promotion() != Piece::None ==
                ((move.to().y() == 0 || move.to().y() == 7) && abs_piece == Piece::Pawn));

            Player player = this->turn();
            if (square(move.from()) != move.piece())
                return false;

            if (square(move.to()) != move.captured())
                return false;

			if (sgn(square(move.from())) != player)
				return false;
         
			// For all non-knight pieces check if there are no pieces on the way
            if (abs_piece != Piece::Knight)
            {
				Direction dir = move.from().get_direction_to(move.to());
                Square sq = move.from();
                DCHECK(sq.move(dir));
                while (sq != move.to())
                {
                    if (square(sq) != Piece::None)
                        return false;

                    DCHECK(sq.move(dir));
                }

                // Castling?
                if (abs_piece == Piece::King && move.from().king_distance(move.to()) == 2)
                {
					if (is_checked(player))
						return false;

                    Square sq = move.from();
                    DCHECK(sq.move(dir));
					if (this->is_controlled_by(sq, oponent(player))) 
						return false;

                    // Keep moving in the same direction to the Rook's square
                    Piece rook = player == Player::First ? Piece::Rook : Piece::OtherRook;
					while (sq.move(dir))
					{
                        // When rook encountered it should be the last square
                        if (square(sq) == rook)
                        {
                            if (sq.move(dir))
                                return false;

							goto perform_move;
                        }

						if (square(sq) != Piece::None)
							return false;
					}
                    
                    return false;
                }
            }

perform_move:
            (*this) += move;
            if (King1.king_distance(King2) >= 2 && !is_checked(player))
                return true;

            // If still checked revert the move and return false
            (*this) -= move;
            return false;
        }
        
        bool operator==(const ChessPosition<QPO>& other) const
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
#define VERIFY_DIRECTION(DIR)                   \
for (sq1.DIR(); sq1 < sq2; sq1.DIR())           \
{                                               \
    if (square(sq1) != Piece::None)             \
        return false;                           \
}                                               \
return true;                                    \

        bool rook_move(Square sq1, Square sq2) const
        {
            DCHECK(sq1 != sq2);
            if (sq1 > sq2) std::swap(sq1, sq2);
            if (sq1.x() == sq2.x())
            {
                VERIFY_DIRECTION(move_up);
            }
            if (sq1.y() == sq2.y())
            {
                VERIFY_DIRECTION(move_right);
            }
            return false;
        }

        bool bishop_move(Square sq1, Square sq2)
        {
            DCHECK(sq1 != sq2);
            if (sq1 > sq2) std::swap(sq1, sq2);
            int dy = sq2.y() - sq1.y();
            DCHECK(dy >= 0);
            if (dy == sq2.x() - sq1.x())
            {
                VERIFY_DIRECTION(move_upright);
            }
            if (dy == sq1.x() - sq2.x())
            {
                VERIFY_DIRECTION(move_upright);
            }
            return false;
        }

        bool queen_move(Square sq1, Square sq2)
        {
            return rook_move(sq1, sq2) || bishop_move(sq1, sq2);
        }

        void track_pgn() { _track_pgn = true;  }

        std::string pgn()
        {
            std::string ret = "";
            for (int i = 0; i < _ply; i++)
            {
                if (i % 2 == 0)
                {
                    int turn = i / 2 + 1;
                    ret += std::to_string(turn);
                    ret += ". ";
                }
                ret += pgns[i] + " ";
            }
            return ret;
        }

        std::vector<std::string> pgns;

        std::string move_to_pgn(Move move)
        {
            Piece piece = square(move.from());
            switch (abs(piece))
            {
            //King = 1, Queen = 2, Rook = 3, Bishop = 4, Knight = 5, Pawn = 6,
            case Piece::King:
            {
                if (move.to().king_distance(move.from()) == 2)
                {
                    if (move.to().x() == 2) // "C1" or "C8"
                        return "O-O-O";
                    if (move.to().x() == 6) // "G1" or "G8"
                        return "O-O";
                    DCHECK_FAIL;
                }
                return "K" + move.to().chess_notation(true);
            }
            case Piece::Queen:
            {
                bool x_diff = true, y_diff = true;
                bool another = false;
                for (Square sq : get_squares(piece))
                {
                    if (move.from() != sq && queen_move(sq, move.to()))
                    {
                        another = true;
                        if (sq.x() == move.from().x())
                            x_diff = false;
                        if (sq.y() == move.from().y())
                            y_diff = false;
                    }
                }
                std::string extra = "";
                if (another)
                {
                    if (x_diff) extra = 'a' + move.from().x();
                    else if (y_diff) extra = '1' + move.from().y();
                    else extra = move.from().chess_notation(true);
                }
                return "Q" + extra + move.to().chess_notation(true);
            }
            case Piece::Rook:
            {
                bool x_diff = true, y_diff = true;
                bool another = false;
                for (Square sq : get_squares(piece))
                {
                    if (move.from() != sq && rook_move(sq, move.to()))
                    {
                        another = true;
                        if (sq.x() == move.from().x())
                            x_diff = false;
                        if (sq.y() == move.from().y())
                            y_diff = false;
                    }
                }
                std::string extra = "";
                if (another)
                {
                    if (x_diff) extra = 'a' + move.from().x();
                    else if (y_diff) extra = '1' + move.from().y();
                    else extra = move.from().chess_notation(true);
                }
                return "R" + extra + move.to().chess_notation(true);
            }
            case Piece::Bishop: return "B" + move.to().chess_notation(true);
            case Piece::Knight:
            {
                bool x_diff = true, y_diff = true;
                bool another = false;
                for (Square sq : move.to().knight_moves())
                {
                    if (sq == move.from() || square(sq) != piece)
                        continue;

                    another = true;
                    if (sq.x() == move.from().x())
                        x_diff = false;
                    if (sq.y() == move.from().y())
                        y_diff = false;
                }
                std::string extra = "";
                if (another)
                {
                    if (x_diff) extra = 'a' + move.from().x();
                    else if (y_diff) extra = '1' + move.from().y();
                    else extra = move.from().chess_notation(true);
                }
                return "N" + extra + move.to().chess_notation(true);
            }
            case Piece::Pawn:
            {
                std::string pre, post;
                if (move.captured() != Piece::None)
                {
                    pre = move.from().chess_notation(true);
                    pre[1] = 'x';
                }
                if (move.promotion() != Piece::None)
                {
                    post = std::string("=") + Piece_to_char(abs(move.promotion()));
                }
                return pre + move.to().chess_notation(true) + post;
            }
            default:
            {
                DCHECK_FAIL;
                return "";
            }
            };
        }

        Move pgn_to_move(std::string str)
        {
            if (str == "O-O" && turn() == Player::First)
            {
                return Move(King1, S("G1"), Piece::King);
            }
            if (str == "O-O-O" && turn() == Player::First)
            {
                return Move(King1, S("C1"), Piece::King);
            }
            if (str == "O-O" && turn() == Player::Second)
            {
                return Move(King2, S("G8"), Piece::OtherKing);
            }
            if (str == "O-O-O" && turn() == Player::Second)
            {
                return Move(King2, S("C8"), Piece::OtherKing);
            }
            if (str.length() == 2)
            {
                Square sq(str), sq_from = sq;
                if (turn() == Player::First)
                {
                    sq_from.move_down();
					if (this->operator[](sq_from) != Piece::Pawn)
                        sq_from.move_down();
                    DCHECK(this->operator[](sq_from) == Piece::Pawn);
                }
                else
                {
                    sq_from.move_up();
                    if (this->operator[](sq_from) != Piece::OtherPawn)
                        sq_from.move_up();
                    DCHECK(this->operator[](sq_from) == Piece::OtherPawn);
                }

            	return Move(sq_from, sq, square(sq_from));
            }
            if (str.length() == 3)
            {
#define CHECKDIRECTION(MOVE) sq = sq_to; while (sq.MOVE()) { if ((*this)[sq] == piece) return Move(sq, sq_to, square(sq)); }

				Square sq_to(str.substr(1));
                switch (str[0])
                {
				    case 'Q':
                    {
						Piece piece = turn() == Player::First ? Piece::Queen : Piece::OtherQueen;
                        Square sq;
                        CHECKDIRECTION(move_up);
                        CHECKDIRECTION(move_down);
                        CHECKDIRECTION(move_left);
                        CHECKDIRECTION(move_right);

                        CHECKDIRECTION(move_upleft);
                        CHECKDIRECTION(move_upright);
                        CHECKDIRECTION(move_downleft);
                        CHECKDIRECTION(move_downright);
                        DCHECK_FAIL;
                    }
                    case 'R':
                    {
                        Piece piece = turn() == Player::First ? Piece::Rook : Piece::OtherRook;
                        Square sq;
                        CHECKDIRECTION(move_up);
                        CHECKDIRECTION(move_down);
                        CHECKDIRECTION(move_left);
                        CHECKDIRECTION(move_right);
                        DCHECK_FAIL;
                    }
                    case 'B':
                    {
                        Piece piece = turn() == Player::First ? Piece::Bishop : Piece::OtherBishop;
                        Square sq;
                        CHECKDIRECTION(move_upleft);
                        CHECKDIRECTION(move_upright);
                        CHECKDIRECTION(move_downleft);
                        CHECKDIRECTION(move_downright);
                        DCHECK_FAIL;
                    }

#define CHECKMOVE(MOVE) sq = sq_to; if (sq.MOVE()) { if ((*this)[sq] == piece) return Move(sq, sq_to, square(sq)); }

                    case 'N':
                    {
						Piece piece = turn() == Player::First ? Piece::Knight : Piece::OtherKnight;
                        Square sq;
						CHECKMOVE(move_knight1);
                        CHECKMOVE(move_knight2);
                        CHECKMOVE(move_knight3);
                        CHECKMOVE(move_knight4);
                        CHECKMOVE(move_knight5);
                        CHECKMOVE(move_knight6);
                        CHECKMOVE(move_knight7);
                        CHECKMOVE(move_knight8);
                        DCHECK_FAIL;
                    }
                }

                DCHECK_FAIL;
            }
            //TODO: 4 and more characters
            if (str.length() == 4)
            {
                DCHECK_FAIL;
            }
            DCHECK_FAIL;
            return Move();
        }
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
            auto* nonConstThis = const_cast<chess::ChessPosition<QPO>*>(this);

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

        void operator+=(Move move)
        {
            if (_track_pgn)
            {
                if (_ply == pgns.size())
                    pgns.push_back(move_to_pgn(move));
                else
                    pgns[_ply] = move_to_pgn(move);
            }

            DCHECK(square(move.from()) == move.piece());
            DCHECK(square(move.to()) == move.captured());
            square(move.to()) = move.promotion() == Piece::None ? square(move.from()) : move.promotion();
            square(move.from()) = Piece::None;
            if (move.from() == King1)
            {
                if (move.from() == S("E1"))
                {
                    castle_rook(chess::Piece::King, S("G1"), chess::Piece::Rook, S("H1"), S("F1"));
                    castle_rook(chess::Piece::King, S("C1"), chess::Piece::Rook, S("A1"), S("D1"));
                }
                King1 = move.to();
            }
            if (move.from() == King2)
            {
                if (move.from() == S("E8"))
                {
                    castle_rook(chess::Piece::OtherKing, S("G8"), chess::Piece::OtherRook, S("H8"), S("F8"));
                    castle_rook(chess::Piece::OtherKing, S("C8"), chess::Piece::OtherRook, S("A8"), S("D8"));
                }
                King2 = move.to();
            }
            BoardBase::move();
            if (track_material_on)
                tracked_material += move.material_change();
        }

        void operator-=(Move move)
        {
            DCHECK(square(move.to()) == move.piece() || square(move.to()) == move.promotion());
            DCHECK(square(move.from()) == Piece::None);
            if (move.to() == King1)
            {
                if (move.from() == S("E1"))
                {
                    castle_rook(chess::Piece::King, S("G1"), chess::Piece::Rook, S("F1"), S("H1"));
                    castle_rook(chess::Piece::King, S("C1"), chess::Piece::Rook, S("D1"), S("A1"));
                }
                King1 = move.from();
            }
            if (move.to() == King2)
            {
                if (move.from() == S("E8"))
                {
                    castle_rook(chess::Piece::OtherKing, S("G8"), chess::Piece::OtherRook, S("F8"), S("H8"));
                    castle_rook(chess::Piece::OtherKing, S("C8"), chess::Piece::OtherRook, S("D8"), S("A8"));
                }
                King2 = move.from();
            }
            square(move.from()) =
                move.promotion() == Piece::None
                ? square(move.to())
                : (belongs_to(move.promotion(), Player::First) ? Piece::Pawn : Piece::OtherPawn);
            square(move.to()) = move.captured();
            BoardBase::reverse_move();
            if (track_material_on)
                tracked_material -= move.material_change();
        }
#pragma endregion

        std::experimental::generator<Move> all_legal_moves_played()
        {
#define PLAY_AND_YIELD  \
(*this) += move;        \
if (is_checked(player)) \
    (*this) -= move;    \
else                    \
    co_yield move;


#define MOVE_IN_DIRECTION(MOVE)                                 \
sq2 = sq;                                                       \
while (sq2.MOVE() && !belongs_to(square(sq2), player))          \
{                                                               \
    Move move(sq, sq2, square(sq), square(sq2));                \
    PLAY_AND_YIELD                                              \
    if (move.captured() != Piece::None)                         \
        break;                                                  \
}

#define MOVE_ONCE_WITH_COND(MOVE, CONDITION)                    \
sq2 = sq;                                                       \
if (sq2.MOVE() && !belongs_to(square(sq2), player) && CONDITION)\
{                                                               \
     Move move(sq, sq2, square(sq), square(sq2));                \
    PLAY_AND_YIELD                                              \
}

#define MOVE_ONCE(MOVE) MOVE_ONCE_WITH_COND(MOVE, true)
#define MOVE_PAWN(MOVE) MOVE_ONCE_WITH_COND(MOVE, true)

            // Store pgn flag
            bool stored_pgn = _track_pgn;
            _track_pgn = false;

            Player player = this->turn();
            Player other_player = oponent(player);

            //DCHECK(Player::First)
            Square sq(0);
            do
            {
                Piece piece = square(sq);
                if (!belongs_to(piece, player))
                    continue;
                piece = abs(piece);
                Square sq2;
                if (piece == Piece::Queen || piece == Piece::Rook)
                {
                    MOVE_IN_DIRECTION(move_left);
                    MOVE_IN_DIRECTION(move_right);
                    MOVE_IN_DIRECTION(move_up);
                    MOVE_IN_DIRECTION(move_down);
                }
                if (piece == Piece::Queen || piece == Piece::Bishop)
                {
                    MOVE_IN_DIRECTION(move_upleft);
                    MOVE_IN_DIRECTION(move_upright);
                    MOVE_IN_DIRECTION(move_downleft);
                    MOVE_IN_DIRECTION(move_downright);
                }
                if (piece == Piece::Knight)
                {
                    MOVE_ONCE(move_knight1);
                    MOVE_ONCE(move_knight2);
                    MOVE_ONCE(move_knight3);
                    MOVE_ONCE(move_knight4);
                    MOVE_ONCE(move_knight5);
                    MOVE_ONCE(move_knight6);
                    MOVE_ONCE(move_knight7);
                    MOVE_ONCE(move_knight8);
                }
                if (piece == Piece::King)
                {
                    Square other_king = this->turn() == Player::First ? King2 : King1;

                    MOVE_ONCE_WITH_COND(move_up,      sq2.king_distance(other_king) >= 2);
                    MOVE_ONCE_WITH_COND(move_down,    sq2.king_distance(other_king) >= 2);
                    MOVE_ONCE_WITH_COND(move_left,    sq2.king_distance(other_king) >= 2);
                    MOVE_ONCE_WITH_COND(move_right,   sq2.king_distance(other_king) >= 2);
                    MOVE_ONCE_WITH_COND(move_upleft,  sq2.king_distance(other_king) >= 2);
                    MOVE_ONCE_WITH_COND(move_upright, sq2.king_distance(other_king) >= 2);
                    MOVE_ONCE_WITH_COND(move_downleft,sq2.king_distance(other_king) >= 2);
                    MOVE_ONCE_WITH_COND(move_downright,sq2.king_distance(other_king) >= 2);
                }
                if (piece == Piece::Pawn)
                {
                    Square sqF = sq, sqFF = sq;
                    Piece queen;
                    if (this->turn() == Player::First)
                    {
                        sqF.move_up();
                        queen = Piece::Queen;
                    }
                    else
                    {
                        sqF.move_down();
                        queen = Piece::OtherQueen;
                    }
                    if (square(sqF) == Piece::None)
                    {
                        Move move(sq, sqF, square(sq), Piece::None, sqF.y() == 0 || sqF.y() == 7 ? queen : Piece::None);
                        PLAY_AND_YIELD
                    }
                    Square sqL = sqF, sqR = sqF;
                    if (sqL.move_left() && belongs_to(square(sqL), oponent(player)))
                    {
                        Move move(sq, sqL, square(sq), square(sqL), sqL.y() == 0 || sqL.y() == 7 ? queen : Piece::None);
                        PLAY_AND_YIELD
                    }
                    if (sqR.move_right() && belongs_to(square(sqR), oponent(player)))
                    {
                        Move move(sq, sqR, square(sq), square(sqR), sqR.y() == 0 || sqR.y() == 7 ? queen : Piece::None);
                        PLAY_AND_YIELD
                    }
                    if (square(sqF) == Piece::None)
                    {
                        if (sqF.y() == 2 && player == Player::First && sqF.move_up() && square(sqF) == Piece::None)
                        {
                            Move move(sq, sqF, square(sq), Piece::None);
                            PLAY_AND_YIELD
                        } else
                        if (sqF.y() == 5 && player == Player::Second && sqF.move_down() && square(sqF) == Piece::None)
                        {
                            Move move(sq, sqF, square(sq), Piece::None);
                            PLAY_AND_YIELD
                        }
                    }
                }
            } while (++sq);

            int y = player == Player::First ? 0 : 7;
            Square king(4, y);

            if (abs(square(king)) == Piece::King && belongs_to(square(king), player))
            {
                Square right_rook(7, y);
                if (right_castle(king, right_rook, player))
                {
                    std::swap(square(king), square(king + 2));
                    std::swap(square(right_rook), square(right_rook - 2));
                    (player == Player::First ? King1 : King2) = king + 2;
                    BoardBase::move();
                    co_yield Move(king, king + 2, square(king+2));
                }
                
                Square left_rook(0, y);
                if (left_castle(king, left_rook, player))
                {
                    std::swap(square(king), square(king - 2));
                    std::swap(square(left_rook), square(left_rook + 3));
                    (player == Player::First ? King1 : King2) = king - 2;
                    BoardBase::move();
                    co_yield Move(king, king - 2, square(king-2));
                }
            }

            // Restore pgn flag
            _track_pgn = stored_pgn;
        }

        bool right_castle(Square king, Square rook, Player player)
        {
            // Rook on place
            if (abs(square(rook)) != Piece::Rook || !belongs_to(square(rook), player))
                return false;

            // Empty squares in between
            Square sq = king;
            for (sq.move_right(); sq != rook; sq.move_right())
            {
                if (square(sq) != Piece::None)
                    return false;
            }

            // King.from(), middle, King.to() are not checked
            Player other_player = oponent(player);
            Square king_to = king + 2;
            for (sq = king; sq <= king_to; sq.move_right())
            {
                if (is_controlled_by(sq, other_player))
                    return false;
            }

            return true;
        }
        
        bool left_castle(Square king, Square rook, Player player)
        {
            // Rook on place
            if (abs(square(rook)) != Piece::Rook || !belongs_to(square(king), player))
                return false;

            // Empty squares in between
            Square sq = king;
            for (sq.move_left(); sq != rook; sq.move_left())
            {
                if (square(sq) != Piece::None)
                    return false;
            }

            // King.from(), middle, King.to() are not checked
            Player other_player = oponent(player);
            Square king_to = king - 2;
            for (sq = king; sq >= king_to; sq.move_left())
            {
                if (is_controlled_by(sq, other_player))
                    return false;
            }

            return true;
        }

        bool exists_move(bool (*func)(const ChessPosition&)) const
        {
            for (auto move : all_legal_moves_played())
            {
                bool exists = func(*this);
                (*this) -= move;
                if (exists)
                    return true;
            }
            return false;
        }

        bool for_each_move(bool (*func)(const ChessPosition&)) const
        {
            for (auto move : all_legal_moves_played())
            {
                bool ret = func(*this);
                (*this) -= move;
                if (!ret)
                    return false;
            }
            return true;
        }

        std::experimental::generator<Move> all_legal_moves() const
        {
            auto nonConstThis = const_cast<ChessPosition*>(this);

            for (auto move : nonConstThis->all_legal_moves_played())
            {
                (*nonConstThis) -= move;
                co_yield move;
            }
        }

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
        
        // Square start may or may not be occupied.
        // If it is occupied by a piece, this effectively checks if Player player
        // can capture or protects this piece.
        bool is_controlled_by(Square start, Player player) const
        {
#define CHECK_DIRECTION(START, MOVE, PIECES)    \
sq = START;                                     \
while (sq.MOVE())                               \
{                                               \
    Piece piece = square(sq);                   \
    if (piece != Piece::None) {                 \
        if (!belongs_to(piece, player))         \
            break;                              \
        piece = abs(piece);                     \
        if (PIECES)                             \
            return true;                        \
        break;                                  \
    }                                           \
};
            Square sq;
            bool other_piece = false;

            // Check horizontal and vertical directions for queen and rook
            CHECK_DIRECTION(start, move_left, piece == Piece::Queen || piece == Piece::Rook);
            CHECK_DIRECTION(start, move_right, piece == Piece::Queen || piece == Piece::Rook);
            CHECK_DIRECTION(start, move_up, piece == Piece::Queen || piece == Piece::Rook);
            CHECK_DIRECTION(start, move_down, piece == Piece::Queen || piece == Piece::Rook);

            // Check diagonal directions for queen and bishop
            CHECK_DIRECTION(start, move_upleft, piece == Piece::Queen || piece == Piece::Bishop);
            CHECK_DIRECTION(start, move_upright, piece == Piece::Queen || piece == Piece::Bishop);
            CHECK_DIRECTION(start, move_downleft, piece == Piece::Queen || piece == Piece::Bishop);
            CHECK_DIRECTION(start, move_downright, piece == Piece::Queen || piece == Piece::Bishop);


#define CHECK_SINGLE(START, MOVE, PIECE)                                        \
sq = START;                                                                     \
if (sq.MOVE() && abs(square(sq)) == PIECE && belongs_to(square(sq), player))    \
    return true;
            // Check knights
            CHECK_SINGLE(start, move_knight1, Piece::Knight);
            CHECK_SINGLE(start, move_knight2, Piece::Knight);
            CHECK_SINGLE(start, move_knight3, Piece::Knight);
            CHECK_SINGLE(start, move_knight4, Piece::Knight);
            CHECK_SINGLE(start, move_knight5, Piece::Knight);
            CHECK_SINGLE(start, move_knight6, Piece::Knight);
            CHECK_SINGLE(start, move_knight7, Piece::Knight);
            CHECK_SINGLE(start, move_knight8, Piece::Knight);

            // Check pawns
            if (player == Player::First)
            {
                CHECK_SINGLE(start, move_downright, Piece::Pawn);
                CHECK_SINGLE(start, move_downleft, Piece::Pawn);
            }
            else
            {
                CHECK_SINGLE(start, move_upright, Piece::Pawn);
                CHECK_SINGLE(start, move_upleft, Piece::Pawn);
            }

            return false;
        }

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
		/// Flip rows and invert pieces. Initial position is restored.
		/// White pawns at 7th rank convert to black pawns at 2nd rank.
		/// White king at initial position converts to black king at initial position.
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
            for (auto pair : get_pieces())
            {
                ret += pair.first;
            }
            return ret;
        }

        static friend std::ostream& operator<<(std::ostream& os, const ChessPosition<QPO>& position)
        {
            Square row("H1");
            Square column = row;
            do
            {
                do
                {
                    Piece piece = position[column];
                    char c = Piece_to_char(piece);
                    os << c;
                } while (column.move_left());
                os << std::endl;
            } while (row.move_down());
            return os;
        }

#pragma region Transpositional tables
        inline static uint64_t hash_white[64][6], hash_black[64][6], hash_turn;

        inline static bool transpositional_tables_initialized = false;
        static void initialize_transpositional_tables()
        {
			if (transpositional_tables_initialized)
				return;
            std::mt19937 gen(0);
            std::uniform_int_distribution<uint64_t> dist(
                std::numeric_limits<uint64_t>::min(),
                std::numeric_limits<uint64_t>::max());

            for (int i = 0; i < 64; i++)
                for (int j = 0; j < 6; j++)
                {
                    hash_white[i][j] = dist(gen);
                    hash_black[i][j] = dist(gen);
                }
			hash_turn = dist(gen);
			transpositional_tables_initialized = true;
        }

        consteval static bool implements_hash() { return true; }

		template <bool include_turn = false>
        uint64_t get_hash()
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


    template <bool QPO>
    /// <summary>
    /// 64^n
    /// </summary>
    class ConverterSimple
    {
    public:
		using Position = chess::ChessPosition<QPO>;
        using Move = chess::Move;
        using Key = std::string;
        static std::string name() { return "ConverterSimple"; }
        static SIZE KeyToSize(std::string key)
        {
            return (SIZE)(1) << (6 * key.length());
        }

    	static std::experimental::generator<std::string> get_dependent_tables(std::string key)
    	{
            if (key == "KK")
                co_return;

    		for (int i = 0; i < key.size(); i++)
    		{
    			co_yield key.substr(0, i) + key.substr(i + 1);
    		}
    	}

        static std::string get_opponent_table(std::string key)
        {
            size_t second_king = key.find('K', 1);
            DCHECK(second_king != std::string::npos);
            std::string first = key.substr(second_king);
            std::string second = key.substr(0, second_king);
            return first + second;
        }

        static SIZE PositionToIndex(const ChessPosition<QPO>& position)
        {
            SIZE factor = 1;
            SIZE ret = 0;
            for (auto pair : position.get_piece_squares())
            {
                ret += pair.second * factor;
                factor *= 64;
            }
            return ret;
        }

        static bool KeyIndexToPosition(std::string key, SIZE index, ChessPosition<QPO>& pos)
        {
            //TODO: finish implementation
            SIZE factor = 1;
            for (int i = 0; i < key.size(); i++)
            {
                //pos.table[index % 64] = char_to_piece(key[i]);
                //index /= 64;
            }
            return false;
            //return pos.is_legal();
        }

        static std::string PositionToKey(const ChessPosition<QPO>& position)
        {
            std::string key;
            for (auto pair : position.get_piece_squares())
            {
				key += pair.first;
            }
            return key;
        }

        static void flip_if_needed(ChessPosition<QPO>& pos)
        {
			//todo: implement
        }
        
    };

    template <bool QPO>
    /// <summary>
	/// 64x63x62x...x(64-n+1)
    /// </summary>
    struct ConverterReducing
    {
        using Position = chess::ChessPosition<QPO>;
        using Move = chess::Move;
        using Key = std::string;

        static std::string name() { return "ConverterReducing"; }

        static SIZE KeyToSize(std::string key)
        {
            SIZE ret = 1;
            for (int i = 64; i > 64 - key.length(); i--)
            {
                ret *= i;
            }
            return ret;
        }

        static std::string get_opponent_table(std::string key)
        {
            size_t second_king = key.find('K', 1);
            DCHECK(second_king != std::string::npos);
            std::string first = key.substr(second_king);
            std::string second = key.substr(0, second_king);
            return first + second;
        }

        static std::experimental::generator<std::string> get_dependent_tables(std::string key)
        {
            size_t second_king = key.find('K', 1);
            DCHECK(second_king != std::string::npos);

            for (size_t drop_index = second_king+1; drop_index < key.size(); drop_index++)
            {
				std::string first = key.substr(second_king, drop_index-second_king) + key.substr(drop_index+1);
                std::string second = key.substr(0, second_king);
                co_yield first + second;
            }
        }

        static std::string PositionToKey(const ChessPosition<QPO>& position)
        {
            //TODO: imlement
            return "KK";
        }

        static bool KeyIndexToPosition(std::string key, SIZE index, ChessPosition<QPO>& pos)
        {
            return false;
        }

        static SIZE PositionToIndex(const ChessPosition<QPO>& position)
        {
            return 0;
        }

        static ChessPosition<QPO> IndexToPosition(SIZE index)
        {
            return ChessPosition<QPO>();
        }

        static void flip_if_needed(ChessPosition<QPO>& pos)
        {

        }
    };

    struct ConverterReducingWithPawns
    {
        static std::string name() { return "ConverterReducingWithPawns"; }
        // Without pawns:
        // 10 x (64-1) x (64-2) x (64-3) x ... x (64-n+1)
        // The king is at first quadrant (first 4 rows and first 4 columns) and below diagonal. That is 1+2+3+4=10 fields.

        // With pawns:
        // 48/2 x 47 ... (48-p+1) x (64-p) x ... x (64-n+1)
        // Place pawns first. Each pawn cannot take 1st and 8th row so 6x8=48 places.
        // The first one is on the left side, if not flip rows so 48/2=24
        //static SIZE size(const EndTable& end_table)
        //{
        //    if (!end_table.has_pawns())
        //    {
        //        SIZE ret = 10;  // 10 = 1+2+3+4
        //        for (SIZE i = 63; i > 64 - SIZE(end_table.get_type().length()); i--)
        //        {
        //            ret *= i;
        //        }
        //        return ret;
        //    }
        //}

        template <bool QPO>
        static SIZE Enumerate(const ChessPosition<QPO>&);
        template <bool QPO>
        static std::tuple<std::string, SIZE> PositionToTable(const ChessPosition<QPO>&);
        template <bool QPO>
        static ChessPosition<QPO> TableToPosition(const std::string& type, SIZE index);
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
