#include "core.h"

template <typename Con, typename Pos, typename Move, typename Key>
concept Converter = requires(Con con, Pos& pos, Move move, Key key, SIZE index)
{
	{ Con::KeyToSize(key) } -> std::convertible_to<SIZE>;
	{ Con::PositionToIndex(pos) } -> std::convertible_to<SIZE>;
	{ Con::PositionToKey(pos) } -> std::convertible_to<Key>;
	{ Con::KeyIndexToPosition(key, index, pos) } -> std::convertible_to<bool>;
	{ Con::get_dependent_tables(key) } -> std::convertible_to<std::experimental::generator<Key>>;
	{ Con::get_opponent_table(key) } -> std::convertible_to<Key>;
	{ Con::flip_if_needed(pos) } -> std::convertible_to<void>;
};

template <typename Conv>
	requires Converter<Conv, typename Conv::Position, typename Conv::Move, typename Conv::Key>
class EndTable
{
	using Position = typename Conv::Position;
	using Key = typename Conv::Key;
	using Move = typename Conv::Move;

private:
	//Key key;
	std::vector<TableEntry> evals;
	inline static std::unordered_map<Key, EndTable> tables; // all tables

public:
	static Move FindBestMove(const Position& pos)
	{
   		Key key = Conv::PositionToKey(pos);
		solve(key);

		auto nonConstPos = const_cast<Position&>(pos);

		int value = 0; // todo: FindValue(pos);
		int expected_value = value--;     
		for (auto move : pos.all_legal_moves())
		{
			return move;

			nonConstPos += move;

			if (FindValue(nonConstPos) == expected_value)
			{
				nonConstPos -= move;
				return move;
			}

			nonConstPos -= move;
		}
		DCHECK_FAIL;
	}


	/// <summary>
	/// Bottom-up solves the endtables starting from the simplest and going up to the requested table.
	/// For example, when solving chess table "KQKP", tables are solved in the following order:
	/// "KK"
	/// "KPK"/"KKP"
	/// "KQK"/"KKQ"
	/// "KQKP"/"KPKQ"
	/// </summary>
	/// <param name="key"></param>
	static void solve(Key key)
	{
		if (tables.contains(key))
			return;

		bool any = false;
		for (Key dep : Conv::get_dependent_tables(key))
		{
			solve(dep);
			any = true;
		}

		tables[key] = EndTable(key);
	}

	static int FindValue(const Position& pos)
	{
		auto index = Conv::PositionToIndex(pos);
		return 0;
	}
	
	EndTable()
	{
	}

	EndTable(Key key )
	{
		if (Conv::get_opponent_table(key) == key)
		{
			SIZE size = Conv::KeyToSize(key);
			Position pos;
			for (int i = 0; i < size; i++)
			{
				continue;

				if (!Conv::KeyIndexToPosition(key, i, pos))
					continue;
				if (pos.is_lost())
					evals[i] = TableEntry::Lose(0);
				else
					evals[i] = TableEntry::Open;
			}

			bool chage = false;
			do
			{
				for (int i = 0; i < size; i++)
				{
					if (!Conv::KeyIndexToPosition(key, i, pos))
						continue;
					
					//TODO: remove when implemented
					continue;

					for (auto move : pos.all_legal_moves_played())
					{

						pos -= move;
					}
				}
			} while (chage);
		}
		else
		{
			//TODO:
		}
	}
};
