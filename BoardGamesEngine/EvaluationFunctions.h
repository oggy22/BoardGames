#pragma once
#include <cmath>
#include "endings.h"

class EvaluationFunctionDummy
{
public:
	float Evaluate(ChessPosition& position) { return 0; }
	float EvaluteIncrement(ChessPosition& position, Move move) { return 0; }
};

class EvaluationFunctionMaterial
{
	static const float value[7] = { 0, std::numeric_limits<float>::infinity(), 10, 5, 3, 3, 1 };
public:
	float Evaluate(ChessPosition& position)
	{
		float sum = 0.0;
		Square sq;
		do
		{
			Piece piece = position[sq];
			if (piece == Piece::None)
				continue;
			float sign = std::abs(float(piece));
			piece = abs(piece);
			if (piece == Piece::King)
				continue;
			float add;
			add = value[piece];
			sum += sign * add;
		} while (sq < 64);
	}
	float EvaluteIncrement(ChessPosition& position, Move move)
	{
		return value[move.captured()];
	}
};
