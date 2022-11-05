#pragma once

#include <string>
#include <experimental/generator>

#define DCHECK(assertion) { if (!(assertion)) exit(1); }
#define SIZE long long

enum class Player
{
    // The player who has turn
    First,

    // The other player
    Second
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

    int x()
    {
        return _square % W;
    }

    int y()
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

protected:
    static_assert(W * H < 1 << 8);
    uint8_t _square;
};

template<int W, int H, typename piece_t>
class BoardBase
{
protected:
    Player turn;
    piece_t table[W * H];
public:
    BoardBase() : turn(Player::First) { }
    piece_t& operator[](SquareBase<W, H> sq)
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
        SquareBase sq(W * H - 1);
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

class TableEntry
{
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

