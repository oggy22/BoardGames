#pragma once

#include <string>
#include <experimental/generator>

inline void failure()
{

}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)
#define DCHECK(assertion)           \
{                                   \
    if (!(assertion))               \
        failure();                  \
}

#define DCHECK_FAIL throw(AT)

#define SIZE long long

enum class Player : int
{
    // The player who has turn
    First = 1,

    // The other player
    Second = -1
};

Player oponent(Player player);

template<int W, int H>
class SquareBase
{
public:
    SquareBase() : _square(0) {}
    SquareBase(int n) : _square(n)
    {
        DCHECK(n >= 0 && n < W * H);
    }
    SquareBase(int x, int y) : _square(x + y * W)
    {
        DCHECK(0 <= x && x < W);
        DCHECK(0 <= y && y < H);
    }

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

    SquareBase flip_horizontally() { return Square(W-x()-1, y()); }
    SquareBase flip_vertifaclly() { return Square(x(), H-y() - 1); }
    SquareBase rotate_180() { return Square(W*H - 1 - _square); }

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

    operator int() { return int(_square); }

    bool is_valid()
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
    std::string chess_notation() const
    {
        static_assert(W <= 26 && H < 10);
        std::string ret("  ");
        ret[0] = 'A' + x();
        ret[1] = '1' + y();
        return ret;
    }

protected:
    static_assert(W * H < (1 << 16));
    uint16_t _square;
};

template<int W, int H, typename piece_t>
class BoardBase
{
    Player _turn;
    int ply;
protected:
    piece_t table[W * H];

    piece_t& operator[](SquareBase<W, H> sq)
    {
        return table[int(sq)];
    }

    void move()
    {
		_turn = oponent(_turn);
		ply++;
	}

    void reverse_move()
    {
        ply++;
        DCHECK(ply >= 0);
    }

    void invert()
    {
        _turn = oponent(_turn);
    }
public:
    Player turn() const { return _turn; }
    BoardBase() : _turn(Player::First), ply(0) { }

    piece_t operator[](SquareBase<W, H> sq) const
    {
        return table[int(sq)];
    }

    std::experimental::generator<piece_t> get_pieces()
    {
        SquareBase<W,H> sq;
        do
        {
            co_yield (*this)[sq];
        } while (++sq);
    }

    std::experimental::generator<piece_t> get_pieces_reversed()
    {
        SquareBase<W, H> sq(W * H - 1);
        do
        {
            co_yield (*this)[sq];
        } while (--sq);
    }

    std::experimental::generator<SquareBase<W, H>> get_squares()
    {
        SquareBase<W, H> sq;
        do
        {
            co_yield sq;
        } while (++sq);
    }

    std::experimental::generator<SquareBase<W, H>> get_squares_reversed()
    {
        SquareBase<W, H> sq(W * H - 1);
        do
        {
            co_yield sq;
        } while (--sq);
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

