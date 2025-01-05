#include "chess.h"

using namespace chess;

void ChessPosition::operator+=(Move move)
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

void ChessPosition::operator-=(Move move)
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

std::experimental::generator<Move> ChessPosition::all_legal_moves_played()
{
    Player player = this->turn();
    Player other_player = oponent(player);

#pragma region Count Checks
    Direction dir_check = Direction(0);
    int number_of_checks = 0;
    auto King = (turn() == Player::First ? King1 : King2);
    if (is_controlled_by_from_direction<Direction::up>(King, other_player))
    {
        dir_check |= Direction::up; number_of_checks++;
    }
    if (is_controlled_by_from_direction<Direction::down>(King, other_player))
    {
        dir_check |= Direction::down; number_of_checks++;
    }
    if (is_controlled_by_from_direction<Direction::left>(King, other_player))
    {
        dir_check |= Direction::left; number_of_checks++;
    }
    if (is_controlled_by_from_direction<Direction::right>(King, other_player))
    {
        dir_check |= Direction::right; number_of_checks++;
    }
    if (is_controlled_by_from_direction<Direction::upleft>(King, other_player))
    {
        dir_check |= Direction::upleft; number_of_checks++;
    }
    if (is_controlled_by_from_direction<Direction::upright>(King, other_player))
    {
        dir_check |= Direction::upright; number_of_checks++;
    }
    if (is_controlled_by_from_direction<Direction::downleft>(King, other_player))
    {
        dir_check |= Direction::downleft; number_of_checks++;
    }
    if (is_controlled_by_from_direction<Direction::downright>(King, other_player))
    {
        dir_check |= Direction::downright; number_of_checks++;
    }

    // Knights
    if (is_controlled_by_from_direction<Direction::knight1>(King, other_player))
    {
        dir_check |= Direction::knight1; number_of_checks++;
    }
    if (is_controlled_by_from_direction<Direction::knight2>(King, other_player))
    {
        dir_check |= Direction::knight2; number_of_checks++;
    }
    if (is_controlled_by_from_direction<Direction::knight3>(King, other_player))
    {
        dir_check |= Direction::knight3; number_of_checks++;
    }
    if (is_controlled_by_from_direction<Direction::knight4>(King, other_player))
    {
        dir_check |= Direction::knight4; number_of_checks++;
    }
    if (is_controlled_by_from_direction<Direction::knight5>(King, other_player))
    {
        dir_check |= Direction::knight5; number_of_checks++;
    }
    if (is_controlled_by_from_direction<Direction::knight6>(King, other_player))
    {
        dir_check |= Direction::knight6; number_of_checks++;
    }
    if (is_controlled_by_from_direction<Direction::knight7>(King, other_player))
    {
        dir_check |= Direction::knight7; number_of_checks++;
    }
    if (is_controlled_by_from_direction<Direction::knight8>(King, other_player))
    {
        dir_check |= Direction::knight8; number_of_checks++;
    }

#pragma endregion

#define PLAY_AND_YIELD                                                                              \
(*this) += move;                                                                                    \
if (dir_check!=Direction::none && is_controlled_by_from_direction(King, other_player, dir_check))   \
    (*this) -= move;                                                                                \
else if (dir_from!=Direction::none && is_controlled_by_from_direction(King, other_player, dir_from))\
    (*this) -= move;                                                                                \
else                                                                                                \
    co_yield move;

#define PLAY_AND_YIELD_ALL_PROMOTIONS                                                               \
(*this) += move;                                                                                    \
if (dir_check!=Direction::none && is_controlled_by_from_direction(King, other_player, dir_check))   \
  {  (*this) -= move; }                                                                             \
else if (dir_from!=Direction::none && is_controlled_by_from_direction(King, other_player, dir_from))\
    { (*this) -= move; }                                                                            \
else if (move.promotion() == Piece::None)                                                           \
{   co_yield move;  }                                                                               \
else                                                                                                \
{                                                                                                   \
    co_yield move;																				    \
    while (move.next_promotion()) { (*this) += move; co_yield move; } ; }                           \


#define MOVE_IN_DIRECTION(MOVE)                                 \
sq2 = sq;                                                       \
while (sq2.MOVE() && !belongs_to(square(sq2), player))          \
{                                                               \
    Direction dir_to = King.get_direction_to<true>(sq2);        \
    if (dir_check!=Direction(0) && (dir_check!=dir_to))         \
    {   if (square(sq2) == Piece::None) continue; else break;}  \
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

#define MOVE_KING(MOVE)                                                                 \
sq2 = sq;                                                                               \
if (sq2.MOVE() && !belongs_to(square(sq2), player) && sq2.king_distance(other_king) >= 2)\
{                                                                                       \
    Move move(sq, sq2, square(sq), square(sq2));                                        \
    (*this) += move;                                                                    \
    if (is_controlled_by(sq2, other_player))                                            \
		(*this) -= move;                                                                \
	else                                                                                \
		co_yield move;                                                                  \
}

#define MOVE_ONCE(MOVE) MOVE_ONCE_WITH_COND(MOVE, true)
#define MOVE_PAWN(MOVE) MOVE_ONCE_WITH_COND(MOVE, true)

    // Store pgn flag
    bool stored_pgn = _track_pgn;
    _track_pgn = false;

    DCHECK(number_of_checks <= 2);
    Square sq(0);
    do
    {
        Piece piece = square(sq);
        Direction dir_from = King.get_direction_to<false>(sq);
        if (!belongs_to(piece, player))
            continue;
        piece = abs(piece);
        Square sq2;
        if (number_of_checks < 2)
        {
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
                    PLAY_AND_YIELD_ALL_PROMOTIONS
                }
                Square sqL = sqF, sqR = sqF;
                if (sqL.move_left() && belongs_to(square(sqL), oponent(player)))
                {
                    Move move(sq, sqL, square(sq), square(sqL), sqL.y() == 0 || sqL.y() == 7 ? queen : Piece::None);
                    PLAY_AND_YIELD_ALL_PROMOTIONS
                }
                if (sqR.move_right() && belongs_to(square(sqR), oponent(player)))
                {
                    Move move(sq, sqR, square(sq), square(sqR), sqR.y() == 0 || sqR.y() == 7 ? queen : Piece::None);
                    PLAY_AND_YIELD_ALL_PROMOTIONS
                }
                if (square(sqF) == Piece::None)
                {
                    if (sqF.y() == 2 && player == Player::First && sqF.move_up() && square(sqF) == Piece::None)
                    {
                        Move move(sq, sqF, square(sq));
                        PLAY_AND_YIELD
                    }
                    else if (sqF.y() == 5 && player == Player::Second && sqF.move_down() && square(sqF) == Piece::None)
                    {
                        Move move(sq, sqF, square(sq));
                        PLAY_AND_YIELD
                    }
                }
            } // if (number_of_checks < 2)
        }
        if (piece == Piece::King)
        {
            Square other_king = this->turn() == Player::First ? King2 : King1;

            MOVE_KING(move_up);
            MOVE_KING(move_down);
            MOVE_KING(move_left);
            MOVE_KING(move_right);
            MOVE_KING(move_upleft);
            MOVE_KING(move_upright);
            MOVE_KING(move_downleft);
            MOVE_KING(move_downright);
        }
    } while (++sq);

    int y = player == Player::First ? 0 : 7;
    Square king(4, y);

    if (number_of_checks == 0 && abs(square(king)) == Piece::King && belongs_to(square(king), player))
    {
        Square right_rook(7, y);
        if (right_castle(king, right_rook, player))
        {
            std::swap(square(king), square(king + 2));
            std::swap(square(right_rook), square(right_rook - 2));
            (player == Player::First ? King1 : King2) = king + 2;
            BoardBase::move();
            co_yield Move(king, king + 2, square(king + 2));
        }

        Square left_rook(0, y);
        if (left_castle(king, left_rook, player))
        {
            std::swap(square(king), square(king - 2));
            std::swap(square(left_rook), square(left_rook + 3));
            (player == Player::First ? King1 : King2) = king - 2;
            BoardBase::move();
            co_yield Move(king, king - 2, square(king - 2));
        }
    }

    // Restore pgn flag
    _track_pgn = stored_pgn;
}

bool ChessPosition::play_if_legal(Move move)
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
        Direction dir = move.from().get_direction_to<false>(move.to());
        DCHECK(dir != Direction::none);
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

#define VERIFY_DIRECTION(DIR)                   \
for (sq1.DIR(); sq1 < sq2; sq1.DIR())           \
{                                               \
    if (square(sq1) != Piece::None)             \
        return false;                           \
}                                               \
return true;

bool ChessPosition::rook_move(Square sq1, Square sq2) const
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

bool ChessPosition::bishop_move(Square sq1, Square sq2) const
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

bool ChessPosition::queen_move(Square sq1, Square sq2) const
{
    return ChessPosition::rook_move(sq1, sq2) || ChessPosition::bishop_move(sq1, sq2);
}

bool ChessPosition::right_castle(Square king, Square rook, Player player) const
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

bool ChessPosition::left_castle(Square king, Square rook, Player player) const
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

std::experimental::generator<Move> ChessPosition::all_legal_moves() const
{
    auto nonConstThis = const_cast<ChessPosition*>(this);

    for (auto move : nonConstThis->all_legal_moves_played())
    {
        (*nonConstThis) -= move;
        co_yield move;
    }
}

// Square start may or may not be occupied
// If it is occupied by a piece, this effectively checks if Player player
// can capture or protects this piece.
bool ChessPosition::is_controlled_by(Square start, Player player) const
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

bool ChessPosition::is_controlled_by_from_direction(Square start, Player player, Direction dir) const
{
    switch (dir)
    {
    case Direction::up: return is_controlled_by_from_direction<Direction::up>(start, player);
    case Direction::down: return is_controlled_by_from_direction<Direction::down>(start, player);
    case Direction::left: return is_controlled_by_from_direction<Direction::left>(start, player);
    case Direction::right: return is_controlled_by_from_direction<Direction::right>(start, player);
    case Direction::upleft: return is_controlled_by_from_direction<Direction::upleft>(start, player);
    case Direction::upright: return is_controlled_by_from_direction<Direction::upright>(start, player);
    case Direction::downleft: return is_controlled_by_from_direction<Direction::downleft>(start, player);
    case Direction::downright: return is_controlled_by_from_direction<Direction::downright>(start, player);

    case Direction::knight1: return is_controlled_by_from_direction<Direction::knight1>(start, player);
    case Direction::knight2: return is_controlled_by_from_direction<Direction::knight2>(start, player);
    case Direction::knight3: return is_controlled_by_from_direction<Direction::knight3>(start, player);
    case Direction::knight4: return is_controlled_by_from_direction<Direction::knight4>(start, player);
    case Direction::knight5: return is_controlled_by_from_direction<Direction::knight5>(start, player);
    case Direction::knight6: return is_controlled_by_from_direction<Direction::knight6>(start, player);
    case Direction::knight7: return is_controlled_by_from_direction<Direction::knight7>(start, player);
    case Direction::knight8: return is_controlled_by_from_direction<Direction::knight8>(start, player);
    default: DCHECK_FAIL;
    }
    //TODO: check if start is occupied by player
    return false;
}
