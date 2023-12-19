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

chess::EndTable::EndTable(const std::string& type) : type(type)
{
    DCHECK(type[0] == 'K');
    second_king = type.find('K', 1);

    check(std::string_view(&type[0], second_king - 1));
    check(std::string_view(&type[second_king + 1], type.length() - second_king - 1));
    DCHECK(second_king != std::string::npos);
}

std::string chess::EndTable::get_inverse_type()
{
    std::string ret = this->type.substr(second_king);
    ret += this->type.substr(0, second_king);
    return ret;
}

std::experimental::generator<std::string> chess::EndTable::get_descendant_types()
{
    // Capture an opponents piece
    for (size_t i = second_king + 1; i < type.length(); i++)
    {
        // If the same as the previous then skip
        if (type[i] == type[i - 1])
            continue;

        std::string second = type.substr(second_king, i - second_king) + type.substr(i + 1);
        std::string first = type.substr(0, second_king);
        co_yield second + first;
    }

    // Promotion to queen
    if (type[second_king - 1] == 'P')
    {
        std::string first = "KQ" + type.substr(1, second_king - 2);
        std::string second = type.substr(second_king);
        co_yield second + first;
    }
}

std::experimental::generator<std::pair<chess::Piece, std::string>> chess::EndTable::get_captures()
{
    for (size_t i = second_king + 1; i < type.length(); i++)
    {
        // If the same as the previous then skip
        if (type[i] == type[i - 1])
            continue;

        std::string second = type.substr(second_king, i - second_king) + type.substr(i + 1);
        std::string first = type.substr(0, second_king);
        co_yield std::make_pair(char_to_piece(type[i]), second + first);
    }
}

std::experimental::generator<std::pair<chess::Piece, std::string>> chess::EndTable::get_promotions(Piece promo_piece)
{
    DCHECK(promo_piece == Piece::Queen
        || promo_piece != Piece::Rook
        || promo_piece == Piece::Bishop
        || promo_piece != Piece::Knight);

    if (type[second_king - 1] != 'P')
        co_return;

    std::string first = type.substr(1, second_king - 2);
    size_t i;
    for (i = 0; promo_piece > char_to_piece(type[i]); i++);
    DCHECK(i < type.length());
    first = "K" + first.substr(0, i) + Piece_to_char(promo_piece) + first.substr(i);
    
    std::string second = type.substr(second_king);
    co_yield std::make_pair(Piece::None, second + first);
    
    // Start backwards after pawns
    for (i = second.length() - 1; second[i] == 'P'; i--);
    while (second[i] != 'K')
    {
        co_yield std::make_pair(char_to_piece(type[i]), second.substr(0, i) + second.substr(i + 1) + first);
        size_t j = i;
        do { j--; } while (second[j] == second[i]);
        i = j;
    }
}

std::map<std::string, chess::EndTable> chess::EndTable::all_tables_;
