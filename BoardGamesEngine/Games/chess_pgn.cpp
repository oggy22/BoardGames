#include "chess.h"

using namespace chess;

std::string ChessPosition::pgn() const
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

bool ChessPosition::construct_from_pgn(std::string pgn)
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
        chess::Move move = this->pgn_to_move(wmove);
        (*this) += move;
        if (ss.eof())
            return false;

        // Get black move
        std::string bmove;
        ss >> bmove;
        if (bmove == "")
            break;
        move = this->pgn_to_move(bmove);
        (*this) += move;
    } while (!ss.eof());
    return false;
}

std::string ChessPosition::move_to_pgn(Move move) const
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

template <Piece abs_piece>
Move ChessPosition::_pgn_to_move(Square sq_to, SquareRequirements req) const
{
    Piece piece = this->turn() == Player::First ? abs_piece : other(abs_piece);
    Square sq;
    Move move;
    int results = 0;
    if constexpr (abs_piece == Piece::Queen || abs_piece == Piece::Bishop)
    {
        sq = sq_to; if (go_until_piece<Direction::upleft>(sq) == piece && req.satisfies(sq)) { move = Move(sq, sq_to, square(sq), square(sq_to)); results++; }
        sq = sq_to; if (go_until_piece<Direction::upright>(sq) == piece && req.satisfies(sq)) { move = Move(sq, sq_to, square(sq), square(sq_to)); results++; }
        sq = sq_to; if (go_until_piece<Direction::downleft>(sq) == piece && req.satisfies(sq)) { move = Move(sq, sq_to, square(sq), square(sq_to)); results++; }
        sq = sq_to; if (go_until_piece<Direction::downright>(sq) == piece && req.satisfies(sq)) { move = Move(sq, sq_to, square(sq), square(sq_to)); results++; }
    }
    if constexpr (abs_piece == Piece::Queen || abs_piece == Piece::Rook)
    {
        sq = sq_to; if (go_until_piece<Direction::up>(sq) == piece && req.satisfies(sq)) { move = Move(sq, sq_to, square(sq), square(sq_to)); results++; }
        sq = sq_to; if (go_until_piece<Direction::down>(sq) == piece && req.satisfies(sq)) { move = Move(sq, sq_to, square(sq), square(sq_to)); results++; }
        sq = sq_to; if (go_until_piece<Direction::left>(sq) == piece && req.satisfies(sq)) { move = Move(sq, sq_to, square(sq), square(sq_to)); results++; }
        sq = sq_to; if (go_until_piece<Direction::right>(sq) == piece && req.satisfies(sq)) { move = Move(sq, sq_to, square(sq), square(sq_to)); results++; }
    }
    if constexpr (abs_piece == Piece::Knight)
    {
        sq = sq_to; if (sq.move_knight1() && square(sq) == piece && req.satisfies(sq)) { move = Move(sq, sq_to, square(sq), square(sq_to)); results++; }
        sq = sq_to; if (sq.move_knight2() && square(sq) == piece && req.satisfies(sq)) { move = Move(sq, sq_to, square(sq), square(sq_to)); results++; }
        sq = sq_to; if (sq.move_knight3() && square(sq) == piece && req.satisfies(sq)) { move = Move(sq, sq_to, square(sq), square(sq_to)); results++; }
        sq = sq_to; if (sq.move_knight4() && square(sq) == piece && req.satisfies(sq)) { move = Move(sq, sq_to, square(sq), square(sq_to)); results++; }
        sq = sq_to; if (sq.move_knight5() && square(sq) == piece && req.satisfies(sq)) { move = Move(sq, sq_to, square(sq), square(sq_to)); results++; }
        sq = sq_to; if (sq.move_knight6() && square(sq) == piece && req.satisfies(sq)) { move = Move(sq, sq_to, square(sq), square(sq_to)); results++; }
        sq = sq_to; if (sq.move_knight7() && square(sq) == piece && req.satisfies(sq)) { move = Move(sq, sq_to, square(sq), square(sq_to)); results++; }
        sq = sq_to; if (sq.move_knight8() && square(sq) == piece && req.satisfies(sq)) { move = Move(sq, sq_to, square(sq), square(sq_to)); results++; }
    }
    DCHECK(results == 1);
    return move;
}

Move ChessPosition::pgn_to_move(std::string str) const
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
            if (square(sq_from) != Piece::Pawn)
                sq_from.move_down();
            DCHECK(square(sq_from) == Piece::Pawn);
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
        Square sq_to(str.substr(1));
        switch (str[0])
        {
        case 'Q': return _pgn_to_move<Piece::Queen>(sq_to);
        case 'R': return _pgn_to_move<Piece::Rook>(sq_to);
        case 'B': return _pgn_to_move<Piece::Bishop>(sq_to);
        case 'N': return _pgn_to_move<Piece::Knight>(sq_to);
        case 'K':
        {
            Piece piece = turn() == Player::First ? Piece::Knight : Piece::OtherKnight;

            Square king_sq = turn() == Player::First ? King1 : King2;
            Square sq;
            DCHECK(king_sq.king_distance(sq_to) == 1);

            return Move(king_sq, sq_to, square(king_sq), square(sq_to));
        }
        }

        DCHECK_FAIL;
    }

    // Promotion
    if (str[str.length() - 2] == '=')
    {
        Square sq_to(str.substr(str.length() - 4, 2));
        Square sq_from = sq_to; if (turn() == Player::First) sq_from.move_down(); else sq_from.move_up();

        // Capture
        if (str[1] == 'x')
        {
            sq_from = Square(str[0] - 'a', sq_from.y());
        }

        Piece promotion = char_to_piece(str.back()); if (turn() == Player::Second) promotion = other(promotion);
        return Move(sq_from, sq_to, square(sq_from), square(sq_to), promotion);
    }

    if (str.length() == 4)
    {

        Square sq_to(str.substr(2));

        // Pawn capture
        if (str[1] == 'x')
        {
            Square sq_from(str[0] - 'a', sq_to.y() - (turn() == Player::First ? 1 : -1));
            return Move(sq_from, sq_to, square(sq_from), square(sq_to));
        }

        SquareRequirements req(str.substr(1, 1));
        switch (str[0])
        {
        case 'Q': return _pgn_to_move<Piece::Queen>(sq_to, req);
        case 'R': return _pgn_to_move<Piece::Rook>(sq_to, req);
        case 'B': return _pgn_to_move<Piece::Bishop>(sq_to, req);
        case 'N': return _pgn_to_move<Piece::Knight>(sq_to, req);
        default: DCHECK_FAIL;
        }
    }
    DCHECK_FAIL;
    return Move();
}
