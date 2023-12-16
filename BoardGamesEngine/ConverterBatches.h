#pragma once
#include "Games\chess.h"
#include "combinations.h"
//#define SIZE long long

namespace chess {

    struct ConverterBatches
    {
        static std::string name() { return "ConverterBatches"; }
        // Without pawns:
        // Q1,R1,B1,N1,Q2,R2,B2,N2 - number of queens, rooks, bishops and knights for each player
        // 10                   x (63 / Q1)             x (63-Q1 / R1)             x (63-Q1-R1 / B1)             x (63-Q1-R1-B1 / N1) x
        // (63-Q1-R1-B1-N1 / 1) x (63-Q1-R1-B1-N1 / Q2) x (63-Q1-R1-B1-N1-Q2 / R2) x (63-Q1-R1-B1-N1-Q2-R2 / B2) x (63-Q1-R1-B1-N1-Q2-R2-B2 / N2)
        // (Note that A/B is number of combinations not division)
        // The king is at first quadrant (first 4 rows and first 4 columns) and below diagonal. That is 1+2+3+4=10 fields.
        // The rest of pieces come in batches by type and color. For example, if there are 2 queens they can take 63x62/2 positions.
        static SIZE size(const EndTable& end_table)
        {
            if (!end_table.has_pawns())
            {
                const std::string& type = end_table.get_type();
                SIZE ret = 10;  // 10 = 1+2+3+4
                for (int i = 1; i < end_table.get_type().length(); )
                {
                    int j = i;
                    while (type[j] == type[i]) { j++; }
                    SIZE mul = nk(64 - i, j - i);
                    ret *= mul;
                    i = j;
                }
                return ret;
            }
            //int q1, r1, b1, n1, q2, r2, b2, n2;
            DCHECK_FAIL;
        }
    };
}
