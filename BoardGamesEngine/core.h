#pragma once

#include <experimental/generator>
#include <random>
#include <string>
#include <vector>
#include <ostream>

inline void failure()
{
    //TODO: find a better way to fail
    throw "failure";
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)

#ifdef _DEBUG
#define DCHECK(expression)           \
{                                   \
    if (!(expression))               \
        failure();                  \
}
#else
#define DCHECK(expression) { auto _result = expression; __assume (_result); }
#endif


#define DCHECK_FAIL DCHECK(false); throw "fail";

#define SIZE size_t

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

enum class Direction : uint16_t
{
    none = 0,
    up = 1,
    down = 2,
    left = 4,
    right = 8,
    upleft = 16,
    upright = 32,
    downleft = 64,
    downright = 128,
	all_rooks = up | down | left | right,
	all_bishops = upleft | upright | downleft | downright,
	all_queens = all_rooks | all_bishops,
    knight1 = 256,
	knight2 = 512,
	knight3 = 1024,
	knight4 = 2048,
	knight5 = 4096,
	knight6 = 8192,
	knight7 = 16384,
	knight8 = 32768,
	all_knights = knight1 | knight2 | knight3 | knight4 | knight5 | knight6 | knight7 | knight8,
	all = all_queens | all_knights
};

inline std::ostream& operator<<(std::ostream& os, const Direction dir)
{
	if (dir == Direction::none)
		return os << "none";
	if (dir == Direction::all)
		return os << "all";
	if (dir == Direction::all_rooks)
		return os << "all_rooks";
	if (dir == Direction::all_bishops)
		return os << "all_bishops";
	if (dir == Direction::all_queens)
		return os << "all_queens";
	if (dir == Direction::all_knights)
		return os << "all_knights";

	if (dir == Direction::up)
		return os << "up";
	if (dir == Direction::down)
		return os << "down";
	if (dir == Direction::left)
		return os << "left";
	if (dir == Direction::right)
		return os << "right";
	if (dir == Direction::upleft)
		return os << "upleft";
	if (dir == Direction::upright)
		return os << "upright";
	if (dir == Direction::downleft)
		return os << "downleft";
	if (dir == Direction::downright)
		return os << "downright";

	if (dir == Direction::knight1)
		return os << "knight1";
	if (dir == Direction::knight2)
		return os << "knight2";
	if (dir == Direction::knight3)
		return os << "knight3";
	if (dir == Direction::knight4)
		return os << "knight4";
	if (dir == Direction::knight5)
		return os << "knight5";
	if (dir == Direction::knight6)
		return os << "knight6";
	if (dir == Direction::knight7)
		return os << "knight7";
	if (dir == Direction::knight8)
		return os << "knight8";

	return os << "unknown";
}

inline constexpr Direction operator|(Direction lhs, Direction rhs) {
    return static_cast<Direction>(
        static_cast<std::underlying_type_t<Direction>>(lhs) |
        static_cast<std::underlying_type_t<Direction>>(rhs)
        );
}

inline constexpr Direction operator&(Direction lhs, Direction rhs) {
    return static_cast<Direction>(
        static_cast<std::underlying_type_t<Direction>>(lhs) &
        static_cast<std::underlying_type_t<Direction>>(rhs)
        );
}

inline constexpr Direction& operator|=(Direction& lhs, Direction rhs) {
    lhs = lhs | rhs;
    return lhs;
}

inline constexpr bool is_knight_direction(Direction dir) {
    return (dir & Direction::all_knights) != Direction::none;
}

inline constexpr bool is_rook_direction(Direction dir) {
    return (dir & Direction::all_rooks) != Direction::none;
}

inline constexpr bool is_bishop_direction(Direction dir) {
    return (dir & Direction::all_bishops) != Direction::none;
}

inline constexpr bool is_queen_direction(Direction dir) {
    return (dir & Direction::all_queens) != Direction::none;
}

inline constexpr Direction next_direction(Direction dir)
{
    return Direction(uint16_t(dir) * 2);
}

template<int W, int H>
class SquareBase
{
public:
    constexpr SquareBase() : _square(W*H) {}  // invalid square
    constexpr SquareBase(int n) : _square(n)
    {
        DCHECK(n >= 0 && n < W * H);
    }
    constexpr SquareBase(int x, int y) : _square(x + y * W)
    {
        DCHECK(0 <= x && x < W);
        DCHECK(0 <= y && y < H);
    }
    constexpr SquareBase(const char s[3]) : SquareBase(s[0] - 'A', s[1] - '1')
    {
        static_assert(W <= 26 && H < 10);
		DCHECK(s[0] >= 'A' && s[0] < 'A' + W);
		DCHECK(s[1] >= '1' && s[1] < '1' + H);
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

    inline bool move_left() { return ((_square--) % W) != 0; }
    inline bool move_right() { return ((++_square) % W) != 0; }
    inline bool move_up() { _square += W; return _square < W * H; }
    inline bool move_down() { if (_square < W) return false; _square -= W; return true; }

    inline bool move_upleft() { return move_up() && move_left(); }
    inline bool move_upright() { return move_up() && move_right(); }
    inline bool move_downleft() { return move_down() && move_left(); }
    inline bool move_downright() { return move_down() && move_right(); }

    template <Direction dir>
    inline bool move()
    {
        switch (dir)
        {
        case Direction::up: return move_up();
        case Direction::down: return move_down();
        case Direction::left: return move_left();
        case Direction::right: return move_right();
        case Direction::upleft: return move_upleft();
        case Direction::upright: return move_upright();
        case Direction::downleft: return move_downleft();
        case Direction::downright: return move_downright();

        case Direction::knight1: return move_upleft() && move_up();
        case Direction::knight2: return move_upleft() && move_left();
        case Direction::knight3: return move_upright() && move_up();
        case Direction::knight4: return move_upright() && move_right();
        case Direction::knight5: return move_downleft() && move_down();
        case Direction::knight6: return move_downleft() && move_left();
        case Direction::knight7: return move_downright() && move_down();
        case Direction::knight8: return move_downright() && move_right();

        default: DCHECK_FAIL;
        }
    }

    inline bool move(Direction dir)
    {
        switch (dir)
        {
        case Direction::up: return move_up();
        case Direction::down: return move_down();
        case Direction::left: return move_left();
        case Direction::right: return move_right();
        case Direction::upleft: return move_upleft();
        case Direction::upright: return move_upright();
        case Direction::downleft: return move_downleft();
        case Direction::downright: return move_downright();
        
        case Direction::knight1: return move_upleft() && move_up();
        case Direction::knight2: return move_upleft() && move_left();
        case Direction::knight3: return move_upright() && move_up();
        case Direction::knight4: return move_upright() && move_right();
        case Direction::knight5: return move_downleft() && move_down();
        case Direction::knight6: return move_downleft() && move_left();
        case Direction::knight7: return move_downright() && move_down();
        case Direction::knight8: return move_downright() && move_right();
        default: DCHECK_FAIL;
        }
    }

	template <bool include_knights>
    Direction get_direction_to(SquareBase sq)
    {
        if (sq == *this)
            return Direction::none;

        int dx = sq.x() - x();
        int dy = sq.y() - y();
        if (dx == 0)
            return dy > 0 ? Direction::up : Direction::down;
        if (dy == 0)
            return dx > 0 ? Direction::right : Direction::left;
        if (dx == dy)
            return dx > 0 ? Direction::upright : Direction::downleft;
        if (dx == -dy)
            return dx > 0 ? Direction::downright : Direction::upleft;

        if constexpr (include_knights)
        {
            if (std::abs(dx) + std::abs(dy) == 3)
            {
                if (dx == -1 && dy == 2)
					return Direction::knight1;
				if (dx == -2 && dy == 1)
					return Direction::knight2;
				if (dx == 1 && dy == 2)
					return Direction::knight3;
				if (dx == 2 && dy == 1)
					return Direction::knight4;

                if (dx == -1 && dy == -2)
					return Direction::knight5;
				if (dx == -2 && dy == -1)
					return Direction::knight6;
				if (dx == 1 && dy == -2)
					return Direction::knight7;
				if (dx == 2 && dy == -1)
					return Direction::knight8;
                DCHECK_FAIL;
            }
        }

        return Direction::none;
    }

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

class SquareRequirements
{
    /// <summary>
	/// Negative 1 denotes no requirements.
    /// </summary>
    int column, row;
public:
    SquareRequirements(std::string st)
    {
        switch (st.length())
        {
            case 0: column = -1; row = -1; break;
            case 1: if (st[0] >= 'a' && st[0] <= 'z')
            {
                column = st[0] - 'a';
                row = -1;
            }
            else
            {
                DCHECK(st[0] >= '1' && st[0] <= '9');
                row = st[0] - '1';
                column = -1;
            }
            break;
            case 2:
                column = st[0] - 'a';
				row = st[1] - '1';
                DCHECK(st[0] >= 'a' && st[0] <= 'z');
                break;
            default: DCHECK_FAIL;
        }
    }

    template<int W, int H>
    bool satisfies(SquareBase<W,H> sq) const
	{
		if (column != -1 && sq.x() != column)
			return false;
		if (row != -1 && sq.y() != row)
			return false;
		return true;
	}
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
    consteval static bool implements_hash() { return false; }

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

	// Starting from start square, go in the direction until a piece is found
	// or the end of the board is reached in which case zero is returned.
    template <Direction dir>
    inline piece_t go_until_piece(SquareBase<W, H>& start) const
    {
        static_assert(is_queen_direction(dir), "Only for queen directions");
        while (start.move<dir>())
        {
			piece_t piece = square(start);
			if (piece != piece_t(0))
				return piece;
        }
        return piece_t(0);
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
    const int8_t CheckMate = std::numeric_limits<int8_t>::min();
public:
    static const int8_t Open = 0;
    TableEntry()
    {
        data = Open;
    }
	TableEntry(int8_t data) : data(data) { }

    TableEntry& operator=(const TableEntry& other)
    {
        data = other.data;
        return *this;
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

    float avg() const
    {
		return float(_sum) / _n;
	}

    int n() const
    {
        return _n;
    }

    int min() const
    {
		return _min;
	}

    int max() const
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

template <typename T>
concept BoardPosition = requires(T pos, const T const_pos, T::Move move, Player player)
{
    { const_pos.all_legal_moves() } -> std::convertible_to<std::experimental::generator<typename T::Move>>;
    { pos.all_legal_moves_played() } -> std::convertible_to<std::experimental::generator<typename T::Move>>;
    { const_pos.easycheck_winning_move(move) } -> std::convertible_to<bool>;
    { const_pos.turn() } -> std::convertible_to<Player>;
    { const_pos.is_checked(player) } -> std::convertible_to<bool>;
    { const_pos.Evaluate() } -> std::convertible_to<int>;
    { pos += move } -> std::convertible_to<void>;
    { pos -= move } -> std::convertible_to<void>;
    { pos.turn_off_all_trackings() } -> std::convertible_to<void>;
};

template <typename Board, typename Move = Board::Move>
	requires BoardPosition<Board>
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
