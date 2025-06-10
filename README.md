# BoardGames
An extensible framework for programming search algorithms (minimax with cutting, killer moves, transposition tables, etc) and end-tables for board games such as two-player, full info board games such as chess, MNK (generalized TicTacToe), Connect4, checkers etc. Heavy use of teamplates and newer C++ features such as variadic templates, `constexpr`, `consteval`, `concepts`, `std::experimental::generator` and heavy use of C macros around chess moves.

To get local enlistment:
1. `git clone https://github.com/oggy22/BoardGames`
2. Open `BoardGames.sln`

# Minmax algorithm
The main algorithm for finding best moves, regardless of the game played, is the minmax algorithm. It is implemented in a highly templatized manner where you can provide any game as long as it satisfies the requirements specified by the C++ concept [BoardPosition](https://github.com/oggy22/BoardGames/blob/eac33e4aaf0dc464c61ecb5028f71c25afd8cd1f/BoardGamesEngine/core.h#L710). The following heuristics are implemented:
1. Alhpa-beta prunning
1. Killer move
1. Transposition tables

