#include "chess.h"

using namespace chess;

int ChessPosition::get_material_full() const
{
    int ret = 0;
    for (int i = 0; i < 64; i++)
    {
        Piece piece = table[i];
        ret += piece_to_value(piece);
    }
    return ret;
}

int ChessPosition::get_material() const
{
    if (track_material_on)
        return tracked_material;

    return get_material_full();
}

int ChessPosition::king_protection_eval(Square king) const
{
    int ret = 0;
    int minx = std::max(0, king.x() - 1);
    int maxx = std::min(7, king.x() + 1);
    int miny = std::max(0, king.y() - 1);
    int maxy = std::min(7, king.y() + 1);
    for (int x = minx; x <= maxx; x++)
        for (int y = miny; y <= maxy; y++)
        {
            Piece piece = table[Square(x, y)];
            ret += int(sgn(piece));
        }
    return ret;
}

int ChessPosition::king_protection_eval() const
{
    return king_protection_eval(this->King1)
        + king_protection_eval(this->King2);
}

int ChessPosition::count_center_pieces() const
{
    return
        int(sgn(table[S("D4")])) +
        int(sgn(table[S("E4")])) +
        int(sgn(table[S("D5")])) +
        int(sgn(table[S("E5")]));
}

//template <
//    int material, //Qs*9+Rs*5+Bs*3+Ks*3+Ps*1
//    //int almost_promoted = 0, // white pawns on 7th rank - black pawns on 2nd rank
//    int king_protection, // count pieces around the king
//    //int coverage = 0, // count squares controlled by white - black
//    int legal_moves, // number of legal moves
//    int control_center // number of pieces in the center
//>
//int ChessPosition::evaluate() const
//{
//    int ret = 0;
//    if constexpr (material > 0) ret += material * get_material();
//    //if const (almost_promoted)ret += almost_promoted * almost_promoted_evaluate();
//    if constexpr (king_protection > 0) ret += king_protection * king_protection_eval();
//    //if const (coverage)ret += coverage * coverage_evaluate();
//    if constexpr (legal_moves > 0) ret += legal_moves * count_all_legal_moves() * int(turn());
//    if constexpr (control_center > 0) ret += control_center * count_center_pieces();
//    return ret;
//}
