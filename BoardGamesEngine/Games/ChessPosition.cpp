//#include "chess.h"
//
//#define MOVE_IN_DIRECTION(MOVE)                                 \
//sq2 = sq;                                                       \
//while (sq2.MOVE() && !belongs_to(square(sq2), Player::First))   \
//{                                                               \
//    Move move(sq, sq2, square(sq2));                            \
//    (*this) += move;                                            \
//    bool valid = !is_checked(Player::First);                    \
//    (*this) -= move;                                            \
//    co_yield move;                                              \
//    if (move.captured() != Piece::None)                         \
//        break;                                                  \
//}
//
//#define MOVE_ONCE_WITH_COND(MOVE, CONDITION)                    \
//sq2 = sq;                                                       \
//if (sq2.MOVE() && !belongs_to(square(sq2), Player::First) && CONDITION)      \
//{                                                               \
//    Move move(sq, sq2, square(sq2));                            \
//    (*this) += move;                                            \
//    bool valid = !is_checked(Player::First);                    \
//    (*this) -= move;                                            \
//    co_yield move;                                              \
//    if (move.captured() != Piece::None)                         \
//        break;                                                  \
//}
//
//#define MOVE_ONCE(MOVE) MOVE_ONCE_WITH_COND(MOVE, true)
//#define MOVE_PAWN(MOVE) MOVE_ONCE_WITH_COND(MOVE, true)
//
///// <summary>
///// Generates all legal moves, but also updates the position with them
///// </summary>
//template <bool QPO>
//std::experimental::generator<chess::Move> chess::ChessPosition<QPO>::get_all_legal_moves()
//{
//    //DCHECK(Player::First)
//    Square sq;
//    do
//    {
//        Piece piece = square(sq);
//        if (!belongs_to(piece, Player::First))
//            continue;
//        Square sq2;
//        if (piece == Piece::Queen || piece == Piece::Rook)
//        {
//            MOVE_IN_DIRECTION(move_left);
//            MOVE_IN_DIRECTION(move_right);
//            MOVE_IN_DIRECTION(move_up);
//            MOVE_IN_DIRECTION(move_down);
//        }
//        if (piece == Piece::Queen || piece == Piece::Bishop)
//        {
//            MOVE_IN_DIRECTION(move_upleft);
//            MOVE_IN_DIRECTION(move_upright);
//            MOVE_IN_DIRECTION(move_downleft);
//            MOVE_IN_DIRECTION(move_downright);
//        }
//        if (piece == Piece::Knight)
//        {
//            MOVE_ONCE(move_knight1);
//            //todo: ...
//            MOVE_ONCE(move_knight8);
//        }
//        if (piece == Piece::King)
//        {
//            MOVE_ONCE(move_up);
//            MOVE_ONCE(move_down);
//            MOVE_ONCE(move_left);
//            MOVE_ONCE(move_right);
//            MOVE_ONCE(move_upleft);
//            MOVE_ONCE(move_upright);
//            MOVE_ONCE(move_downleft);
//            MOVE_ONCE(move_downright);
//        }
//        if (piece == Piece::Pawn)
//        {
//            MOVE_ONCE_WITH_COND(move_up, square(sq2) == Piece::None);
//            MOVE_ONCE_WITH_COND(move_upleft, square(sq2) != Piece::None);
//            MOVE_ONCE_WITH_COND(move_upright, square(sq2) != Piece::None);
//        }
//    } while (++sq);
//}
