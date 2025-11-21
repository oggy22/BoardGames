// BoardGamesCommandLine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "..\BoardGamesEngine\Games\chess.h"
#include "..\BoardGamesEngine\Algorithms.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: BoardGamesCommandLine [game] [FirstPlayer] [SecondPlayer]" << std::endl;
        std::cout << "Example when both players are human:" << std::endl;
        std::cout << "BoardGamesCommandLine chess player player" << std::endl;
        std::cout << "Example when both white plays against computer:" << std::endl;
        std::cout << "BoardGamesCommandLine chess player 2:noeval" << std::endl;
        return 1;
	}

    if (argc > 4) {
        std::cout << "Too many arguments." << std::endl;
		return 1;
    }

    if (argv[1] != std::string("chess"))
    {
        std::cout << "Unknown game: " << argv[1] << std::endl;
        std::cout << "Only chess is supported so far." << std::endl;
        return 1;
	}

	std::function<chess::Move(const chess::ChessPosition& pos)> findMove = [](const chess::ChessPosition& pos)
        {
            std::string st;
            while (true)
            {
                std::cin >> st;
                auto move = chess::Move::from_string(pos, st);
                if (move.has_value())
                    return move.value();
            } 
        };

    std::array<std::function<chess::Move(const chess::ChessPosition& pos)>, 2> players = { findMove, findMove };

    for (int i = 2; i < argc; i++)
    {
		std::string arg = argv[i];
        if (arg == "player") continue;
        if (arg.size() < 2 || arg[1] != ':')
        {
            std::cout << "Invalid argument: " << arg << std::endl;
            return 1;
        }

		// get option before ':'
        size_t colonPos = arg.find(':');
		int depth = std::stoi(arg.substr(0, colonPos));
		std::cout << "depth: " << depth << std::endl;
        players[i - 2] = [depth](const chess::ChessPosition& pos)
            {
                return MinMax<chess::ChessPosition, KillerOptions::SingleUpdating, true>::FindBestMove(
                    pos, depth,
                    [&](chess::ChessPosition& pos) -> EvalValue::payload_t
                    {
                        return pos.evaluate<1>();
                    });
            };
    }

    for (int i = 0; i < argc; i++)
        std::cout << i << " : " << argv[i] << std::endl;

    chess::ChessPosition pos;
    while (!pos.is_check_mate() && !pos.is_lost())
    {
		std::cout << pos << std::endl;
        
        auto move = players[pos.ply() % 2](pos);
        if (!pos.is_legal(move))
        {
            std::cout << "Illegal move: " << move.chess_notation() << std::endl;
            continue;
        }
        pos += move;
	}
}