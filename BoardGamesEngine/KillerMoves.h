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
    std::array<Move, array_size> killers;

public:
	KillerMoveManager() {}
	bool is_killer(Move move)
	{
		for (auto killer : killers)
			if (killer == move)
				return true;
		return false;
	}

	int size() { return array_size; }
	void update(Move move)
	{
		for (int pos = array_size - 1; pos > 0; pos--)
			killers[pos] = killers[pos - 1];

		killers[0] = move;
	}

	std::experimental::generator<Move> all_killers()
	{
		for (auto move : killers)
			co_yield move;
	}
};

template <typename Move>
class KillerMoveManager<KillerOptions::None, typename Move>
{
public:
	void is_killer(Move move) { return false; }
};

template <typename Move>
class KillerMoveManager<KillerOptions::SingleStatic, typename Move>
{
	Move killer;
public:
	KillerMoveManager() : killer(Move()) {}
	void is_killer(Move move) { return killer == move; }
	void update(Move move) { /* do nothing */ }
	std::experimental::generator<Move> all_killers()
	{
		if (killer.is_valid())
			killer;
	}
};

template <typename Move>
class KillerMoveManager<KillerOptions::SingleUpdating, typename Move>
{
	Move killer;
public:
	KillerMoveManager() : killer(Move()) {}
	bool is_killer(Move move) { return killer == move; }
	void update(Move move) { killer = move; }
	std::experimental::generator<Move> all_killers()
	{
		if (killer.is_valid())
			co_yield killer;
	}
};

template <typename Move>
class KillerMoveManager<KillerOptions::Multiple, typename Move>
{
    std::unordered_set<Move, std::hash<Move>> killers;

public:
	KillerMoveManager() {}
	bool is_killer(Move move) { return killers.contains(move); }
	std::experimental::generator<Move> all_killers()
	{
		for (auto move : killers)
			co_yield move;
	}
	void update(Move move) { killers.insert(move); }
	int size() { return int(killers.size()); }
};
