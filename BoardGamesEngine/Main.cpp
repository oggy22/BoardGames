// ChessEndings.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <sstream>

#include "Games\chess.h"
#include "ConverterBatches.h"

std::string size_to_memory(SIZE size)
{
    // Byte, kilo, Mega, Giga, Tera, Peta
    static const char suff[] = "BkMGTP";
    int ind = 0;
    while (size >= 1024)
    {
        size = size / 1024;
        ind++;
    }
    std::ostringstream sstr;
    sstr << size << suff[ind];
    return sstr.str();
}

template <typename T>
void Print() //requires Converter<T>
{
    std::cout << "All tables:" << T::name() << std::endl;
    SIZE total_size = 0;
    //for (chess::EndTable table : chess::EndTable::all_tables())
    //{
    //    SIZE size = table.get_size<chess::ConverterBatches>();
    //    std::cout << table.get_type() << " " << size << " " << size_to_memory(size) << std::endl;
    //    total_size += size;
    //}
    std::cout << "Total: " << total_size << " " << size_to_memory(total_size) << std::endl;
    std::cout << std::endl;
}

int main()
{
    std::map<chess::Piece, std::map <chess::Piece, int>> map;
    map[chess::Piece::Knight][chess::Piece::Queen] = 5;

    for (int n = 0; n < 6; n++)
        for (int k = 0; k <= n; k++)
            std::cout << "nk(" << n << "," << k << ")" << nk(n, k) << std::endl;

    //std::cout << "Total tables: " << chess::EndTable::total_tables() << std::endl;
    //chess::EndTable end_table = chess::EndTable::Create("KQK");
    //std::cout << "Total tables: " << chess::EndTable::total_tables() << std::endl;

    std::cout << std::endl;

    //Print<chess::ConverterSimple>();
    //Print<chess::ConverterReducing>();
    //Print<chess::ConverterReducingWithPawns>();
    //Print<chess::ConverterBatches>();
}
