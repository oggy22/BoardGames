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

enum Piece
{
    None = 0, King = 1, Queen = 2, Rook = 3, Bishop = 4, Knight = 5, Pawn = 6,
    OtherKing = -1, OtherQueen = -2, OtherRook = -3, OtherBishop = -4, OtherKnight = -5, OtherPawn = -6
};

static Piece abs(Piece piece)
{
    return (Piece)(0 - piece);
}

static bool belongs_to(Piece piece, Player player)
{
    return player == Player::First ? (piece > 0) : (piece < 0);
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
    default: DCHECK_FAIL;
    }
}

class Square : public SquareBase<8, 8>
{
public:
    Square() : _square(0) {}
    Square(int n) : _square(n) {}
    Square(const char s[3])
    {
        DCHECK(s[0] >= 'A' && s[0] <= 'H');
        DCHECK(s[1] >= '1' && s[1] <= '8');
        _square = 8 * (s[1] - '0') + (s[0] - 'A');
    }

    bool operator==(const Square& other) { return _square == other._square; }

    int row() { return _square >> 3; }
    int column() { return _square & 0b111; }
    std::string name()
    {
        char chRow = '1' + column();
        char chCol = 'A' + row();
        return "" + chCol + chRow;
    }

    bool move_knight1() { return move_up() && move_up() && move_left(); }
    //todo...
    bool move_knight8() { return move_up() && move_left() && move_left(); }

    operator int() { return int(_square); }

    //bool operator++()
    //{
    //    _square++;
    //    return _square < 64;
    //}

    //bool operator--()
    //{
    //    _square--;
    //    return _square >= 0;
    //}

private:
    /// <summary>
    /// A1=0, B1=1, C1=3, ..., H1=7, A2=8, B2=9, ..., H2=15, ..., H8 = 63  
    /// </summary>
    uint8_t _square;
};

#define CHECK_DIRECTION(START, MOVE, PIECES)    \
sq = START;                                     \
while (sq.MOVE())                               \
{                                               \
    Piece piece = square(sq);                   \
    if (belongs_to(piece, player))              \
        break;                                  \
    piece = abs(piece);                         \
    if (PIECES)                                 \
        return true;                            \
    if (piece != Piece::None)                   \
        break;                                  \
};

#define CHECK_SINGLE(START, MOVE, PIECE)                                    \
sq = START;                                                                 \
if (sq.MOVE() && abs(square(sq)) == PIECE && !belongs_to(square(sq), player)) \
    return true;

class Move
{
public:
    bool is_valid() { return int(_from) != -1; }
    Move() : _from(-1) { }
    Move(Square from, Square to, Piece captured = Piece::None) : _from(from), _to(to), _captured(captured) {}
    void flip_rows();
    void flip_columns();
    void flip_rows_and_columns();
    friend std::ostream& operator<<(std::ostream& os, Move move);
    Square from() { return _from; }
    Square to() { return _to; }
    Piece captured() { return _captured; }
    Piece promotion() { return _promotion; }

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
    bool is_legal() { throw ""; }
    Piece& square(Square sq) { return table[sq]; }
    void operator+=(Move move)
    {
        DCHECK(square(move.to()) == move.captured());
        square(move.to()) = square(move.from());
        square(move.from()) = Piece::None;
        if (move.to() == King1) King1 = move.from();
        if (move.to() == King2) King2 = move.from();
    }
    void operator-=(Move move)
    {
        square(move.from()) = square(move.to());
        square(move.to()) = move.captured();
        if (move.from() == King1) King1 = move.to();
        if (move.from() == King2) King2 = move.to();
    }
    Piece operator[](Square square) { return table[square]; }
    std::experimental::generator<Move> get_all_legal_moves();
    bool exists_move(bool (*func)(const ChessPosition&))
    {
        for (auto move : get_all_legal_moves())
        {
            (*this) += move;
            bool exists = func(*this);
            (*this) -= move;
            if (exists)
                return true;
        }
        return false;
    }
    bool for_each_move(bool (*func)(const ChessPosition&))
    {
        for (auto move : get_all_legal_moves())
        {
            (*this) += move;
            bool ret = func(*this);
            (*this) -= move;
            if (!ret)
                return false;
        }
        return true;
    }

    bool any_legal_moves(Player) { return false; }
    //{
    //    for (auto m : get_all_legal_mexperimentaloves())
    //    {
    //        co_yield return true;
    //    }
    //    co_yield return false;
    //}

    bool is_checked(Player player)
    {
        Square start, sq;
        start = player == Player::First ? King1 : King2;
        bool other_piece = false;

        // Check horizontal and vertical directions for queen and rook
        CHECK_DIRECTION(start, move_left,   piece == Piece::Queen || piece == Piece::Rook);
        CHECK_DIRECTION(start, move_right,  piece == Piece::Queen || piece == Piece::Rook);
        CHECK_DIRECTION(start, move_up,     piece == Piece::Queen || piece == Piece::Rook);
        CHECK_DIRECTION(start, move_down,   piece == Piece::Queen || piece == Piece::Rook);

        // Check diagonal directions for queen and bishop
        CHECK_DIRECTION(start, move_upleft,     piece == Piece::Queen || piece == Piece::Bishop);
        CHECK_DIRECTION(start, move_upright,    piece == Piece::Queen || piece == Piece::Bishop);
        CHECK_DIRECTION(start, move_downleft,   piece == Piece::Queen || piece == Piece::Bishop);
        CHECK_DIRECTION(start, move_downright,  piece == Piece::Queen || piece == Piece::Bishop);

        // Check knights
        CHECK_SINGLE(start, move_knight1, Piece::Knight);
        //...
        CHECK_SINGLE(start, move_knight8, Piece::Knight);

        // Check pawns
        if (player == Player::First)
        {
            CHECK_SINGLE(start, move_upright, Piece::Pawn);
            CHECK_SINGLE(start, move_upleft, Piece::Pawn);
        }
        else
        {
            //DHECK(player == Player::Second)
            CHECK_SINGLE(start, move_downright, Piece::Pawn);
            CHECK_SINGLE(start, move_downleft, Piece::Pawn);
        }

        return false;
    }


    bool is_check_mate(Player player = Player::First) { return is_checked(player) && !any_legal_moves(player); }
    bool no_pawns_1and8();
    bool is_valid(Player)
    {
        return is_checked(Player::Second) && no_pawns_1and8();
    }

    void flip_rows();
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

    ChessPosition()
    {
        King1 = King2 = -1;
        for (int i = 0; i < 64; i++)
            table[i] = Piece::None;
    }

    friend std::ostream& operator<<(std::ostream& os, ChessPosition& position)
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
    Piece table[64];
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
            tables[pair.first][Piece::None] = Get(pair.second);
        }

        // Promotion tables
        std::map<Piece, TableEntry*> table_prom;
        for (auto pair : get_promotions(Piece::Queen))
        {
            tables[Piece::None][Piece::Queen] = Get(pair.second);
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
            for (auto move : position.get_all_legal_moves())
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
