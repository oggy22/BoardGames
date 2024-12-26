#include "chess.h"

using namespace chess;

void ChessPosition::initialize_transposition_tables()
{
    if (transposition_tables_initialized)
        return;
    std::mt19937 gen(0);
    std::uniform_int_distribution<uint64_t> dist(
        std::numeric_limits<uint64_t>::min(),
        std::numeric_limits<uint64_t>::max());

    for (int i = 0; i < 64; i++)
        for (int j = 0; j < 6; j++)
        {
            hash_white[i][j] = dist(gen);
            hash_black[i][j] = dist(gen);
        }
    hash_turn = dist(gen);
    transposition_tables_initialized = true;
}
