#include "chess.h"

namespace chess
{
    /// <summary>
    /// 64^n
    /// </summary>
    class ConverterSimple
    {
    public:
        using Position = chess::ChessPosition;
        using Move = chess::Move;
        using Key = std::string;
        static std::string name() { return "ConverterSimple"; }
        static SIZE KeyToSize(std::string key)
        {
            return (SIZE)(1) << (6 * key.length());
        }

        static std::experimental::generator<std::string> get_dependent_tables(std::string key)
        {
            if (key == "KK")
                co_return;

            for (int i = 0; i < key.size(); i++)
            {
                co_yield key.substr(0, i) + key.substr(i + 1);
            }
        }

        static std::string get_opponent_table(std::string key)
        {
            size_t second_king = key.find('K', 1);
            DCHECK(second_king != std::string::npos);
            std::string first = key.substr(second_king);
            std::string second = key.substr(0, second_king);
            return first + second;
        }

        static SIZE PositionToIndex(const ChessPosition& position)
        {
            SIZE factor = 1;
            SIZE ret = 0;
            for (auto pair : position.get_piece_squares())
            {
                ret += pair.second * factor;
                factor *= 64;
            }
            return ret;
        }

        static bool KeyIndexToPosition(std::string key, SIZE index, ChessPosition& pos)
        {
            //TODO: finish implementation
            SIZE factor = 1;
            for (int i = 0; i < key.size(); i++)
            {
                //pos.table[index % 64] = char_to_piece(key[i]);
                //index /= 64;
            }
            return false;
            //return pos.is_legal();
        }

        static std::string PositionToKey(const ChessPosition& position)
        {
            std::string key;
            for (auto pair : position.get_piece_squares())
            {
                key += pair.first;
            }
            return key;
        }

        static void flip_if_needed(ChessPosition& pos)
        {
            //todo: implement
        }

    };

    /// <summary>
    /// 64x63x62x...x(64-n+1)
    /// </summary>
    struct ConverterReducing
    {
        using Position = chess::ChessPosition;
        using Move = chess::Move;
        using Key = std::string;

        static std::string name() { return "ConverterReducing"; }

        static SIZE KeyToSize(std::string key)
        {
            SIZE ret = 1;
            for (int i = 64; i > 64 - key.length(); i--)
            {
                ret *= i;
            }
            return ret;
        }

        static std::string get_opponent_table(std::string key)
        {
            size_t second_king = key.find('K', 1);
            DCHECK(second_king != std::string::npos);
            std::string first = key.substr(second_king);
            std::string second = key.substr(0, second_king);
            return first + second;
        }

        static std::experimental::generator<std::string> get_dependent_tables(std::string key)
        {
            size_t second_king = key.find('K', 1);
            DCHECK(second_king != std::string::npos);

            for (size_t drop_index = second_king + 1; drop_index < key.size(); drop_index++)
            {
                std::string first = key.substr(second_king, drop_index - second_king) + key.substr(drop_index + 1);
                std::string second = key.substr(0, second_king);
                co_yield first + second;
            }
        }

        static std::string PositionToKey(const ChessPosition& position)
        {
            //TODO: imlement
            return "KK";
        }

        static bool KeyIndexToPosition(std::string key, SIZE index, ChessPosition& pos)
        {
            return false;
        }

        static SIZE PositionToIndex(const ChessPosition& position)
        {
            return 0;
        }

        static ChessPosition IndexToPosition(SIZE index)
        {
            return ChessPosition();
        }

        static void flip_if_needed(ChessPosition& pos)
        {

        }
    };

    struct ConverterReducingWithPawns
    {
        static std::string name() { return "ConverterReducingWithPawns"; }
        // Without pawns:
        // 10 x (64-1) x (64-2) x (64-3) x ... x (64-n+1)
        // The king is at first quadrant (first 4 rows and first 4 columns) and below diagonal. That is 1+2+3+4=10 fields.

        // With pawns:
        // 48/2 x 47 ... (48-p+1) x (64-p) x ... x (64-n+1)
        // Place pawns first. Each pawn cannot take 1st and 8th row so 6x8=48 places.
        // The first one is on the left side, if not flip rows so 48/2=24
        //static SIZE size(const EndTable& end_table)
        //{
        //    if (!end_table.has_pawns())
        //    {
        //        SIZE ret = 10;  // 10 = 1+2+3+4
        //        for (SIZE i = 63; i > 64 - SIZE(end_table.get_type().length()); i--)
        //        {
        //            ret *= i;
        //        }
        //        return ret;
        //    }
        //}

        static SIZE Enumerate(const ChessPosition&);
        static std::tuple<std::string, SIZE> PositionToTable(const ChessPosition&);
        static ChessPosition TableToPosition(const std::string& type, SIZE index);
    };
}