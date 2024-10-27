#include <iostream>
#include <vector>
#include "Games\chess.h"

void check(std::string_view sv)
{
    if (sv.length() == 0)
        return;

    char curr = sv[0];
    for (char c : sv)
    {
        //DCHECK(char_to_piece(c) >= char_to_piece(curr))
    }
}

std::string create(const std::vector<chess::Piece>& first, const std::vector<chess::Piece>& second)
{
    std::string ret = "K";
    ret.reserve(first.size() + second.size() + 2);
    for (chess::Piece p : first)
    {
        ret += chess::Piece_to_char(p);
    }
    ret += "K";
    for (chess::Piece p : second)
    {
        ret += chess::Piece_to_char(p);
    }
    return ret;
}

