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

#include "..\core.h"

namespace chess {

    enum Piece
    {
        None = 0,
        King = 1, Queen = 2, Rook = 3, Bishop = 4, Knight = 5, Pawn = 6,
        OtherKing = -1, OtherQueen = -2, OtherRook = -3, OtherBishop = -4, OtherKnight = -5, OtherPawn = -6
    };

    static Piece other(Piece piece)
    {
        return Piece(0 - piece);
    }

    static Piece abs(Piece piece)
    {
        return piece > 0 ? piece : Piece(0-piece);
    }

    static bool belongs_to(Piece piece, Player player)
    {
        return player == Player::First ? (piece > 0) : (piece < 0);
    };

    static char Piece_to_char(Piece piece)
    {
        switch (piece)
        {
        case Piece::None: return ' ';

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
        Square() : SquareBase() { } // invalid square
        Square(int n) : SquareBase(n) { }
        Square(int x, int y) : SquareBase(y * 8 + x) {}
        Square(char letter, char number) : SquareBase(int(letter -'A'), int(number - '1')) { }
        Square(const char s[3]) : Square(s[0] - 'A', s[1] - '1')
        {
            DCHECK(s[0] >= 'A' && s[0] <= 'H');
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

        std::string chess_notation() const
        {
            return SquareBase<8,8>::chess_notation();
        }

    private:
        /// <summary>
        /// A1=0, B1=1, C1=3, ..., H1=7, A2=8, B2=9, ..., H2=15, ..., H8 = 63  
        /// </summary>
        //uint8_t _square;
    };

    class Move
    {
    public:
        bool is_valid() { return _from.is_valid(); }
        Move() : _from() { }    // invalid move
        Move(Square from, Square to, Piece captured = Piece::None, Piece promotion = Piece::None) :
            _from(from),
            _to(to),
            _captured(captured),
            _promotion(promotion)
        {
            DCHECK(captured != Piece::King);
            DCHECK(captured != Piece::OtherKing);
            DCHECK(from.is_valid());
            DCHECK(to.is_valid());
            DCHECK(from != to);
        }
        void flip_rows();
        void flip_columns();
        void flip_rows_and_columns();
        friend std::ostream& operator<<(std::ostream& os, Move move);
        Square from() { return _from; }
        Square to() { return _to; }
        Piece captured() { return _captured; }
        Piece promotion() { return _promotion; }
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

    private:
        Square _from, _to;
        Piece _captured;
        Piece _promotion;
    };

    template <bool QPO> // Queen Promotions Only
    class ChessPosition : public BoardBase<8, 8, Piece>
    {
        friend struct ConverterSimple;
    public:

        ChessPosition(bool only_kings = false)
        {
            for (int i = 0; i < 64; i++)
                table[i] = Piece::None;
            King1 = Square("E1");   table[King1] = Piece::King;
            King2 = Square("E8");   table[King2] = Piece::OtherKing;
            if (only_kings)
                return;

            table[Square("D1")] = Piece::Queen;
            table[Square("D8")] = Piece::OtherQueen;

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
        }

        ChessPosition(std::string FEN)
        {
            Square sq(0);
            for (char c : FEN)
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
                    piece = Piece(0 - piece);

                if (c == 'K') King1 = sq;
                if (c == 'k') King2 = sq;
            
                table[sq] = piece;
                ++sq;
            }
            this->flip_rows();
        }

        std::string fen()
        {
            this->flip_rows();
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
            this->flip_rows();
            
            return fen.substr(0, fen.length()-1);
        }

        int Evaluate() const { return 0; }

        bool play_if_legal(Move move)
        {
            if (move.captured() != table[move.to()])
                return false;
            (*this) += move;
            if (!is_checked(turn()))
                return true;
            
            // If still checked revert the move and return false
            (*this) -= move;
            return false;
        }
        
        bool operator==(const ChessPosition<QPO>& other) const
        {
            if (King1 != other.King1 || King2 != other.King2)
                return false;
            for (int i = 0; i < 64; i++)
            {
                if (table[i] != other.table[i])
                    return false;
            }
            return true;
        }

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
                    table[bottom] = Piece(0 - topPiece);
                    //table[top] = Piece(0 - bottomPiece);
                }
                BoardBase::invert();
            }
        }

        // Pawns one step before promotion?
        bool almost_promotion()
        {
            Square sq = Square(8);
            do { if ((*this)[sq] == Piece::OtherPawn) return true; ++sq; } while (sq < Square(16));
            sq = Square(64 - 16);
            do { if ((*this)[sq] == Piece::Pawn) return true; ++sq; } while (sq < Square(64 - 8));
            return false;
        }
        bool any_pawns() const;
        bool is_legal(Move move) { return square(move.to()) == move.captured(); }
        bool is_reverse_legal(Move move) { return square(move.from()) == Piece::None && square(move.to()) != Piece::None; }
        bool is_legal() { throw ""; }

        Piece operator[](Square square) const { return table[square]; }
        void operator+=(Move move)
        {
            DCHECK(square(move.to()) == move.captured());
            square(move.to()) = move.promotion() == Piece::None ? square(move.from()) : move.promotion();
            square(move.from()) = Piece::None;
            if (move.from() == King1) King1 = move.to();
            if (move.from() == King2) King2 = move.to();
            BoardBase::move();
        }

        void operator-=(Move move)
        {
            DCHECK(square(move.from()) == 0);
            square(move.from()) =
                move.promotion() == Piece::None
                ? square(move.to())
                : (move.promotion() > 0 ? Piece::Pawn : Piece::OtherPawn);
            square(move.to()) = move.captured();
            if (move.to() == King1) King1 = move.from();
            if (move.to() == King2) King2 = move.from();
            BoardBase::reverse_move();
        }
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
    Move move(sq, sq2, square(sq2));                            \
    PLAY_AND_YIELD                                              \
    if (move.captured() != Piece::None)                         \
        break;                                                  \
}

#define MOVE_ONCE_WITH_COND(MOVE, CONDITION)                    \
sq2 = sq;                                                       \
if (sq2.MOVE() && !belongs_to(square(sq2), player) && CONDITION)      \
{                                                               \
    Move move(sq, sq2, square(sq2));                            \
    PLAY_AND_YIELD                                              \
}

#define MOVE_ONCE(MOVE) MOVE_ONCE_WITH_COND(MOVE, true)
#define MOVE_PAWN(MOVE) MOVE_ONCE_WITH_COND(MOVE, true)

            //DCHECK(Player::First)
            Square sq(0);
            do
            {
                Piece piece = square(sq);
                Player player = this->turn();
                if (!belongs_to(piece, player))
                    continue;
                piece = Piece(std::abs(piece));
                Player other_player = oponent(player);
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
                        Move move(sq, sqF, Piece::None, sqF.y() == 0 || sqF.y() == 7 ? queen : Piece::None);
                        PLAY_AND_YIELD
                    }
                    Square sqL = sqF, sqR = sqF;
                    if (sqL.move_left() && belongs_to(square(sqL), oponent(player)))
                    {
                        Move move(sq, sqL, square(sqL), sqL.y() == 0 || sqL.y() == 7 ? queen : Piece::None);
                        PLAY_AND_YIELD
                    }
                    if (sqR.move_right() && belongs_to(square(sqR), oponent(player)))
                    {
                        Move move(sq, sqR, square(sqR), sqR.y() == 0 || sqR.y() == 7 ? queen : Piece::None);
                        PLAY_AND_YIELD
                    }
                    if (square(sqF) == Piece::None)
                    {
                        if (sqF.y() == 2 && player == Player::First && sqF.move_up() && square(sqF) == Piece::None)
                        {
                            Move move(sq, sqF, Piece::None);
                            PLAY_AND_YIELD
                        } else
                        if (sqF.y() == 5 && player == Player::Second && sqF.move_down() && square(sqF) == Piece::None)
                        {
                            Move move(sq, sqF, Piece::None);
                            PLAY_AND_YIELD
                        }
                    }
                }
            } while (++sq);
        }

        bool exists_move(bool (*func)(const ChessPosition&))
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

        bool for_each_move(bool (*func)(const ChessPosition&))
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

        std::experimental::generator<Move> all_legal_moves()
        {
            for (auto move : all_legal_moves_played())
            {
                (*this) -= move;
                co_yield move;
            }
        }

        bool any_legal_moves()
        {
            for (auto m : all_legal_moves())
            {
                return true;
            }
            return false;
        }

        bool is_checked(Player player) const
        {
            return is_controlled_by(player == Player::First ? King1 : King2, oponent(player));
        }
        
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

        bool easycheck_winning_move(Move move)
        {
            return false;
        }

        bool is_check_mate(Player player = Player::First) { return is_checked(player) && !any_legal_moves(); }
        bool no_pawns_1and8() { return true; }
        bool is_valid(Player)
        {
            return is_checked(Player::Second) && no_pawns_1and8();
        }

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

        void flip_columns();
        void flip_rows_and_columns();
        void flip_board();
        std::string generate_endtable_type();

        std::experimental::generator<std::pair<char, Square>> get_pieces() const
        {
            Square square;
            co_yield std::pair<char, Square>('K', King1);
            square = Square(); do { if (table[square] == 0 + Piece::Queen)  co_yield std::pair<char, Square>('Q', square); } while (++square);
            square = Square(); do { if (table[square] == 0 + Piece::Rook)   co_yield std::pair<char, Square>('R', square); } while (++square);
            square = Square(); do { if (table[square] == 0 + Piece::Bishop) co_yield std::pair<char, Square>('B', square); } while (++square);
            square = Square(); do { if (table[square] == 0 + Piece::Pawn)   co_yield std::pair<char, Square>('P', square); } while (++square);
            co_yield std::pair<char, Square>('K', King2);
            square = Square(); do { if (table[square] == 0 - Piece::Queen)  co_yield std::pair<char, Square>('Q', square); } while (++square);
            square = Square(); do { if (table[square] == 0 - Piece::Rook)   co_yield std::pair<char, Square>('R', square); } while (++square);
            square = Square(); do { if (table[square] == 0 - Piece::Bishop) co_yield std::pair<char, Square>('B', square); } while (++square);
            square = Square(); do { if (table[square] == 0 - Piece::Pawn)   co_yield std::pair<char, Square>('P', square); } while (++square);
        }

        void get_numbers(int& q1, int& r1, int& b1, int& n1, int& q2, int& r2, int& b2, int& n2)
        {
            q1 = r1 = b1 = n1 = q2 = r2 = b2 = n2 = 0;
            Square square;
            do
            {
                switch (table[square])
                {
                case 0 + Piece::Queen:  q1++; break;
                case 0 + Piece::Rook:   r1++; break;
                case 0 + Piece::Bishop: b1++; break;
                case 0 + Piece::Knight: n1++; break;
                case 0 - Piece::Queen:  q2++; break;
                case 0 - Piece::Rook:   r2++; break;
                case 0 - Piece::Bishop: b2++; break;
                case 0 - Piece::Knight: n2++; break;
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
    private:
        Square King1, King2;
    };

    //template <typename T>
    //concept bool HasFunc1 =
    //requires(T t) {
    //    { t.func1() } -> int;
    //};

    class EndTable
    {
        friend class std::map<std::string, EndTable>;
        //friend class std::pair<std::string, EndTable>;
        static std::map<std::string, EndTable> all_tables_;

    public:
        EndTable(const std::string& type);  //todo: this should be private, but build error
        static EndTable& Create(const std::string& type)
        {
            std::cout << "Creating " << type << std::endl;
            auto p = all_tables_.find(type);
            if (p != all_tables_.end())
                return p->second;
            all_tables_.emplace(type, type);
            EndTable& table = all_tables_.at(type);

            // Add descendant types
            for (auto desc : table.get_descendant_types())
            {
                Create(desc);
            }

            Create(table.get_inverse_type());

            return table;
        }

        static std::experimental::generator<EndTable> all_tables()
        {
            for (auto& pair : all_tables_)
            {
                co_yield pair.second;
            }
        }

        static SIZE total_tables()
        {
            return all_tables_.size();
        }

        std::string get_inverse_type();
        std::experimental::generator<std::pair<Piece, std::string>> get_captures();
        std::experimental::generator<std::pair<Piece, std::string>> get_promotions(Piece promo);

        std::experimental::generator<std::string> get_descendant_types();
        std::experimental::generator<std::string> get_descendant_types_recursive()
        {
            std::set<std::string> types;
            types.insert(type);
            //for (auto t : )
        }

        static std::experimental::generator<std::string> get_types(int len);

        //ChessPostion GetChessPostion(size index)
        //{
        //    return PositionTableConverter::TableToPosition(type, index);
        //}

        const std::string& get_type() const { return type; }

        bool has_pawns() const
        {
            return type.back() == 'P' || type[second_king - 1] == 'P';
        }

        template <typename T>
        SIZE get_size() const
        {
            return T::size(*this);
        }

        template <typename T>
        void initialize()
        {
            // Get tables
            table = new TableEntry[T::size(*this)];

            // [capture][promotion]
            std::map<Piece, std::map <Piece, int>> tables;

            // Inverse
            tables[Piece::None][Piece::None] = Create(get_inverse_type()).table = new TableEntry[T::size(*this)];

            // Captures tables
            std::map<Piece, TableEntry*> table_cap;
            for (auto pair : get_captures())
            {
                //tables[pair.first][Piece::None] = Get(pair.second);
            }

            // Promotion tables
            std::map<Piece, TableEntry*> table_prom;
            for (auto pair : get_promotions(Piece::Queen))
            {
                //tables[Piece::None][Piece::Queen] = Get(pair.second);
            }

            // Check mates first
            for (SIZE i = 0; i < size; i++)
            {
                ChessPosition position;
                if (!T::IndexToPosition(*this, i, position))
                {
                    table[i] = TableEntry();
                    continue;
                }

                if (position.is_check_mate())
                    table[i] = TableEntry(true);
                else
                    table[i] = TableEntry(false);
            }

            TableEntry search_for = TableEntry(true);
            TableEntry win = TableEntry::Win(1);
            TableEntry lose = TableEntry::Lose(1);
            for (SIZE i = 0; i < size; i++)
            {
                ChessPosition position;
                if (!T::IndexToPosition(*this, i, position))
                    continue;

                if (position.exists_move([](const ChessPosition& pos)
                    {
                        SIZE index = T::PositionToIndex(position);
                        //if (table[index] == )
                        return false;
                    }))
                    table[i] = win;

                    bool found = false;
                    for (auto move : position.all_legal_moves())
                    {
                        // TODO: This still needs some polishing. It will work only when no flipping needed, ConverterSimple
                        position += move;
                        position.flip_board();
                        SIZE index = T::PositionToIndex(position);

                        TableEntry* table_dep = tables[Piece::None][Piece::None];
                        if (move.captured() != Piece::None)
                            table_dep = table_cap[move.captured()];

                        if (table_dep[index] == search_for)
                        {
                            table[i] = win;
                            found = true;
                        }
                        position.flip_board();
                        position -= move;
                        if (found)
                            break;
                    }
            }
        }
    private:
        SIZE size = 0;
        std::string type;
        size_t second_king;
        TableEntry* table;
    };

    template <typename T>
    concept Converter = requires(T t)
    {
        // {   T::TableToPosition(std::string(), int()) } -> std::convertible_to<ChessPosition>;
        {   T::size(EndTable(std::string())) } -> std::convertible_to<SIZE>;
        {   T::name() } -> std::convertible_to<std::string>;
    };

    struct ConverterSimple
    {
        static std::string name() { return "ConverterSimple"; }
        static SIZE size(const EndTable& end_table)
        {
            return (SIZE)(1) << (6 * end_table.get_type().length());
        }

        template <bool QPO>
        static SIZE PositionToIndex(const ChessPosition<QPO>& position)
        {
            SIZE factor = 1;
            SIZE ret = 0;
            for (auto pair : position.get_pieces())
            {
                ret += pair.second * factor;
                factor *= 64;
            }
            return ret;
        }
        template <bool QPO>
        static ChessPosition<QPO> IndexToPosition(const EndTable& table, SIZE index)
        {
        }

        template <bool QPO>
        static bool IndexToPosition(const EndTable& table, SIZE index, ChessPosition<QPO>& position)
        {
            bool first = true;
            bool second = false;
            for (char c : table.get_type())
            {
                Square square = index & 63;
                if (position.table[square] != Piece::None)
                    return false;
                position.table[square] = char_to_piece(c);
                if (first)
                {
                    DCHECK(c == 'K');
                    first = false;
                    position.King1 = square;
                }
                else if (c == 'K') // second king
                {
                    position.King2 = square;
                    second = true;
                }
                DCHECK(second);
            }
            return position.is_legal();
        }
    };

    struct ConverterReducing
    {
        static std::string name() { return "ConverterReducing"; }
        static SIZE size(const EndTable& end_table)
        {
            SIZE ret = 1;
            for (int i = 64; i > 64 - end_table.get_type().length(); i--)
            {
                ret *= i;
            }
            return ret;
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
        // Place pawns first. Each pawn cannot take 1st and 8th row so 6x8=48 places. The first one is on the left side, if not flip rows so 48/2=24
        static SIZE size(const EndTable& end_table)
        {
            if (!end_table.has_pawns())
            {
                SIZE ret = 10;  // 10 = 1+2+3+4
                for (SIZE i = 63; i > 64 - SIZE(end_table.get_type().length()); i--)
                {
                    ret *= i;
                }
                return ret;
            }
        }

        template <bool QPO>
        static SIZE Enumerate(const ChessPosition<QPO>&);
        template <bool QPO>
        static std::tuple<std::string, SIZE> PositionToTable(const ChessPosition<QPO>&);
        template <bool QPO>
        static ChessPosition<QPO> TableToPosition(const std::string& type, SIZE index);
    };
}