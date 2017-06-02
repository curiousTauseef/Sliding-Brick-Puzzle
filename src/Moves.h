#pragma once
#include <iostream>

// enumeration of possible moves for any piece
enum class Moves {
	UP, DOWN, LEFT, RIGHT
};

/* inline because pure header definition
* will increase code size, but avoids
* additional code generation at runtime */
inline std::ostream& operator<<(
	std::ostream& os, Moves const& m) {
	switch (m) {
	case Moves::UP:
		os << "up";
		break;
	case Moves::DOWN:
		os << "down";
		break;
	case Moves::LEFT:
		os << "left";
		break;
	case Moves::RIGHT:
		os << "right";
		break;
	default: os << "UNKNOWN";
	}
	return os;
}