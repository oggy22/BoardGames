#include "chess.h"

using namespace chess;

std::string ChessPosition::fen() const
{
    auto nonConstThis = const_cast<ChessPosition*>(this);

    nonConstThis->flip_rows();
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
    nonConstThis->flip_rows();
    //TODO: implement ends, for example " b "
    std::string extra = this->turn() == Player::First ? " w" : " b";

    return fen.substr(0, fen.length() - 1) /* + extra*/;
}

bool ChessPosition::construct_from_fen(std::string fen)
{
    Square sq(0);
    for (char c : fen)
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
            piece = other(piece);

        if (c == 'K') King1 = sq;
        if (c == 'k') King2 = sq;

        table[sq] = piece;
        ++sq;
    }
    this->flip_rows();
    return true;
}
