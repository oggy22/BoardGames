#pragma once
#include <unordered_set>
#include <set>
#include <vector>
#include <array>

enum class KillerOptions
{
	None = 0,
	SingleStatic = 1,
	SingleUpdating = 2,
	Multiple = 3,
	
	Fixed2 = 8 + 2,
	Fixed3 = 8 + 3,
	Fixed4 = 8 + 4,
	Fixed5 = 8 + 5,
	Fixed6 = 8 + 6,
	Fixed7 = 8 + 7,
};

template <KillerOptions killerOptions, typename Move>
class KillerMoveManager
{
	static_assert(killerOptions >= KillerOptions::Fixed2, "This implementation is only for fixed killer options with depth");
	const constexpr static int array_size = static_cast<int>(killerOptions) - 8;
    std::vector<std::array<Move, array_size>> killers;

public:
	KillerMoveManager(int depth) : killers(depth + 1) {}
	bool is_killer(Move move, int depth)
	{
		for (auto killer : killers[depth])
			if (killer == move)
				return true;
		return false;
	}

	int size(int depth) { return array_size; }
	void update(Move move, int depth)
	{
		for (int pos = array_size - 1; pos > 0; pos--)
			killers[depth][pos] = killers[depth][pos - 1];

		killers[depth][0] = move;
	}

	std::experimental::generator<Move> all_killers(int depth)
	{
		for (auto move : killers[depth])
			co_yield move;
	}
};

template <typename Move>
class KillerMoveManager<KillerOptions::None, typename Move>
{
public:
	void is_killer(Move move, int depth) { return false; }
};

template <typename Move>
class KillerMoveManager<KillerOptions::SingleStatic, typename Move>
{
	std::vector<Move> killers;
public:
	KillerMoveManager(int depth) : killers(depth, Move()) {}
	void is_killer(Move move, int depth) { return killers[depth] == move; }
	void update(Move move, int depth) { /* do nothing */ }
	std::experimental::generator<Move> all_killers(int depth)
	{
		if (killers[depth].is_valid())
			co_yield killers[depth];
	}
};

template <typename Move>
class KillerMoveManager<KillerOptions::SingleUpdating, typename Move>
{
	std::vector<Move> killers;
public:
	KillerMoveManager(int depth) : killers(depth, Move()) {}
	bool is_killer(Move move, int depth) { return killers[depth] == move; }
	void update(Move move, int depth) { killers[depth] = move; }
	std::experimental::generator<Move> all_killers(int depth)
	{
		if (killers[depth].is_valid())
			co_yield killers[depth];
	}
};

template <typename Move>
class KillerMoveManager<KillerOptions::Multiple, typename Move>
{
    std::vector<std::unordered_set<Move, std::hash<Move>>> killers;

public:
	KillerMoveManager(int depth) : killers(depth+1) {}
	bool is_killer(Move move, int depth) { return killers[depth].contains(move); }
	std::experimental::generator<Move> all_killers(int depth)
	{
		for (auto move : killers[depth])
			co_yield move;
	}
	void update(Move move, int depth) { killers[depth].insert(move); }
	int size(int depth) { return int(killers[depth].size()); }
};
