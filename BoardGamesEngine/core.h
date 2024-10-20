#pragma once

#include <experimental/generator>
#include <random>
#include <string>
#include <vector>

inline void failure()
{
    //TODO: find a better way to fail
    throw "failure";
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

#ifdef _DEBUG
#define DCHECK(assertion)           \
{                                   \
    if (!(assertion))               \
        failure();                  \
}
#else
#define DCHECK(assertion) __assume (assertion)
#endif


#define DCHECK_FAIL DCHECK(false); throw "fail";

#define SIZE long long

enum class Player : int
{
    // The player who has turn
    First = 1,

    // The other player
    Second = -1
};

Player constexpr oponent(Player player)
{
    return Player(0 - int(player));
}

template<int W, int H>
class SquareBase
{
public:
    constexpr SquareBase() : _square(W*H) {}  // invalid square
    SquareBase(int n) : _square(n)
    {
        DCHECK(n >= 0 && n < W * H);
    }
    constexpr SquareBase(int x, int y) : _square(x + y * W)
    {
        DCHECK(0 <= x && x < W);
        DCHECK(0 <= y && y < H);
    }

    bool operator==(const SquareBase& sq) const = default;

    int x() const
    {
        return _square % W;
    }

    int y() const
    {
        return _square / W;
    }

    bool move_left() { return ((_square--) % W) != 0; }
    bool move_right() { return ((++_square) % W) != 0; }
    bool move_up() { _square += W; return _square < W * H; }
    bool move_down() { if (_square < W) return false; _square -= W; return true; }

    bool move_upleft() { return move_up() && move_left(); }
    bool move_upright() { return move_up() && move_right(); }
    bool move_downleft() { return move_down() && move_left(); }
    bool move_downright() { return move_down() && move_right(); }

    SquareBase flip_horizontally() { return SquareBase(W-x()-1, y()); }
    SquareBase flip_vertifaclly() { return SquareBase(x(), H-y() - 1); }
    SquareBase rotate_180() { return SquareBase(W*H - 1 - _square); }
    SquareBase roate_90()
    {
        static_assert(W == H, "Allowed only for square boards");
        return SquareBase(W - 1 - y(), x());
    }

    bool operator++()
    {
        return ++_square < W * H;
    }

    bool operator--()
    {
        if (_square == 0)
            return false;
        _square--;
        return true;
    }

    operator int() const { return int(_square); }

    bool is_valid() const
    {
        return _square < W * H;
    }

    static std::experimental::generator<SquareBase<W, H>> all_squares()
    {
        SquareBase<W, H> sq(0);
        do
        {
            co_yield sq;
        } while (++sq);
    }

    /// <summary>
    /// Chess like notation works for bigger boards than 8x8
    /// </summary>
    /// <returns>For example "A1" for the first square</returns>
    std::string chess_notation(bool lowercase = false) const
    {
        static_assert(W <= 26 && H < 10);
        std::string ret("  ");
        ret[0] = (lowercase ? 'a' : 'A') + x();
        ret[1] = '1' + y();
        return ret;
    }

    /// <summary>
    /// Extends the chess field color. A1 is black, A2 is white, etc.
    /// </summary>
    /// <returns>True for A1,A3...,B2,B4.., false otherwise</returns>
    bool is_black() const
    {
        return (x() + y()) % 2 == 0;
    }

    bool is_white() const
    {
        return (x() + y()) % 2 == 1;
    }
protected:
    static_assert(W * H < (1 << 16));

    /// <summary>
    /// General:
    /// A1=0, B1=1, C1=3, ..., H1=7
    /// For chess:
    /// A2=8, B2=9, ..., H2=15, ..., H8 = 63  
    /// </summary>
    uint16_t _square;
};

template<int W, int H, typename piece_t>
class BoardBase
{
    Player _turn;
protected:
    int _ply;
    piece_t table[W * H];

    void move()
    {
		_turn = oponent(_turn);
		_ply++;
	}

    void reverse_move()
    {
        _turn = oponent(_turn);
        _ply--;
        DCHECK(_ply >= 0);
    }

    void invert()
    {
        _turn = oponent(_turn);
    }

    piece_t& operator[](const SquareBase<W, H> sq)
    {
        return table[int(sq)];
    }

public:
    piece_t operator[](const SquareBase<W, H> sq) const
    {
        return table[int(sq)];
    }

    piece_t operator()(int x, int y) const
    {
        return table[SquareBase<W, H>(x, y)];
    }

    Player turn() const { return _turn; }
    int ply() const{ return _ply; }
    BoardBase() : _turn(Player::First), _ply(0) { }

    bool operator==(const BoardBase& board) const = default;

    piece_t square(const SquareBase<W, H> sq) const { return table[sq]; }
    
    piece_t& square(const SquareBase<W, H> sq) { return table[sq]; }

    std::experimental::generator<piece_t> get_pieces() const
    {
        SquareBase<W, H> sq(0);
        do
        {
            co_yield (*this)[sq];
        } while (++sq);
    }

    std::experimental::generator<piece_t> get_pieces_reversed() const
    {
        SquareBase<W, H> sq(W * H - 1);
        do
        {
            co_yield (*this)[sq];
        } while (--sq);
    }

    static std::experimental::generator<SquareBase<W, H>> get_squares()
    {
        SquareBase<W, H> sq(0);
        do
        {
            co_yield sq;
        } while (++sq);
    }

    static std::experimental::generator<SquareBase<W, H>> get_black_squares()
    {
        SquareBase<W, H> sq(0);
        do
        {
            if (sq.is_black())
                co_yield sq;
        } while (++sq);
    }

    std::experimental::generator<SquareBase<W, H>> get_squares_reversed() const
    {
        SquareBase<W, H> sq(W * H - 1);
        do
        {
            co_yield sq;
        } while (--sq);
    }

    std::experimental::generator<SquareBase<W, H>> get_squares(piece_t piece) const
    {
        SquareBase<W, H> sq(0);
        do
        {
            if (piece == (*this)[sq])
                co_yield sq;
        } while (++sq);
    }

    int count_piece(piece_t piece) const
    {
        int ret = 0;
        for (auto p : get_pieces())
        {
            if (p == piece)
                ret++;
        }
        return ret;
    }

    void roate_90()
    {
        static_assert(W == H, "Allowed only for square boards");
        for (int x = 0; x < (W+1) / 2; x++)
            for (int y = 0; y < W / 2; y++)
            {
                SquareBase<W, H> start(x, y);
                piece_t buffer = square(start);
                
                SquareBase<W, H> sq = start;
                SquareBase<W, H> new_sq;
                for (int i = 0; i < 3; i++)
                {
                    new_sq = sq.roate_90();
                    square(sq) = square(new_sq);
                    sq = new_sq;
                }
                square(sq) = buffer;
                DCHECK(sq.roate_90() == start);
            }
    }

    void rotate_180()
    {
        for (SquareBase<W, H> start(0), end(W* H - 1); start < end; ++start, --end)
        {
            std::swap(square(start), square(end));
        }
    }

    //TODO: consteval results in ""call to immediate function is not a constant expression"
    // but intent is to optimize this function out.
    constexpr bool is_checked(Player player) const
    {
        return false;
    }
};

/// <summary>
/// Represents a single board position in the end games tables.
/// Each entry can represent win, lose or open position.
/// Win and lose positions are denoted with number representing the distance from the win or lose respectively.
/// </summary>
class TableEntry
{
    static_assert(std::numeric_limits<int8_t>::min() == 0 - std::numeric_limits<int8_t>::max() - 1);
    const int8_t Open = 0;
    const int8_t CheckMate = std::numeric_limits<int8_t>::min();
    TableEntry(int8_t data) : data(data) { }
public:
    TableEntry()
    {
        data = Open;
    }

    TableEntry(bool check_mate)
    {
        data = check_mate ? CheckMate : Open;
    }

    bool operator==(const TableEntry& other)
    {
        return data == other.data;
    }

    bool operator>(const TableEntry& entry)
    {
        return data > entry.data;
    }

    // Win with number of full moves. Win(1) is half move to win, Win(2) is 3 half moves to win.
    static TableEntry Win(int moves)
    {
        DCHECK(moves > 0 && moves <= std::numeric_limits<int8_t>::max());
        TableEntry entry; entry.data = std::numeric_limits<int8_t>::max() - moves;
        return entry;
    }

    // Lose with number of full moves. Lose(1) is one full move to check-mate, Lose(2) is 2 full moves to check-mate.
    static TableEntry Lose(int moves)
    {
        DCHECK(moves > 0 && moves < std::numeric_limits<int8_t>::max());
        TableEntry entry; entry.data = std::numeric_limits<int8_t>::min() + moves;
        return entry;
    }

private:
    int8_t data;
};

class stats
{
    int _min, _max;
    int _sum, _n;
public:
    void reset()
    {
        _min = std::numeric_limits<int8_t>::max();
        _max = std::numeric_limits<int8_t>::min();
        _sum = 0;
        _n = 0;
    }

    stats()
    {
        reset();
    }

    float avg()
    {
		return float(_sum) / _n;
	}

    int n()
    {
        return _n;
    }

    int min()
    {
		return _min;
	}

    int max()
    {
        return _max;
    }

    void add(int number)
    {
        _sum += number;
        _n++;
        if (number < _min)
            _min = number;
        if (number > _max)
            _max = number;
    }
};

static size_t s_number_of_moves;

template <typename T, typename Q>
concept BoardPosition = requires(T pos, Q move, Player player)
{
    { pos.all_legal_moves() } -> std::convertible_to<std::experimental::generator<Q>>;
    { pos.all_legal_moves_played() } -> std::convertible_to<std::experimental::generator<Q>>;
    { pos.easycheck_winning_move(move) } -> std::convertible_to<bool>;
    { pos.turn() } -> std::convertible_to<Player>;
    { pos.is_checked(player) } -> std::convertible_to<bool>;
    { pos.Evaluate() } -> std::convertible_to<int>;
    { pos += move } -> std::convertible_to<void>;
    { pos -= move } -> std::convertible_to<void>;
};

template <typename Board, typename Move>
	requires BoardPosition<Board, Move>
Move random_move(Board& board, int seed = 0, size_t& number_of_moves = s_number_of_moves)
{
    std::vector<Move> moves;
    for (Move move : board.all_legal_moves())
    {
        moves.push_back(move);
    }

    number_of_moves = moves.size();

    if (moves.size() == 0)
        return Move();

    //std::random_device rd;
    static std::mt19937 gen(seed);
    std::uniform_int_distribution<> dist(0, int(moves.size()) - 1);
    int index = dist(gen);
    return moves[index];
}
