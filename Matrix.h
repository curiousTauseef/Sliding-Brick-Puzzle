#pragma once
#include "Moves.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <unordered_map>

class Matrix {

public:
	// default constructor (dummy)
	Matrix();
	// copy constructor
	Matrix(const Matrix&);
	// "from file" constructor. loads file and turns it into Matrix
	Matrix(const std::string);
	// destructor
	~Matrix();


	// member variables
	int width, height;


	// returns a reference to the piece at x,y coordinates
	int& at(const int x, const int y) const;
	// check if puzzle is completed
	const bool isSolved() const;
	// get a list of all possible moves for a piece
	std::vector<Moves> getMoves(const int = 2) const;
	// collects indices of a piece. First one is always upper left corner
	std::vector<std::pair<int, int>> getPieceIndices(const int) const;
	// calculates the piece dimensions. first: width, second: height
	std::pair<int, int> getPieceDim(std::vector<std::pair<int, int>>) const;
	// maps all possible moves for all pieces. key: piece, value: vec<moves>
	std::unordered_map<unsigned int, std::vector<Moves>> getAllMoves() const;
	// applies a move to a piece. Does NOT check wheather the move is valid!
	void applyMove(const int, const Moves);
	// Makes a deep copy of the current Matrix, applies a move and returns the new Matrix
	Matrix applyMoveCloning(const int, const Moves);
	// normalizes the Matrix row by row, top to bottom. 
	void normalize();
	/* copy & swap idiom
	* copy constructor and assign operator */
	friend void swap(Matrix&, const Matrix&);


	/***  overloaded operators   ***/

	// assign operator
	Matrix& operator= (const Matrix& other) {
		if (this != nullptr && this != &other) {
			delete[] array;
			swap(*this, other);
		}
		return *this;
	}

	// x = index X, y = index Y
	// x in range [0,..,width-1]
	// y in range [0,..,height-1]
	int& operator ()(const int x, const int y) {
		return array[x + y * width];
	}

	// outputs the Matrix to
	friend inline std::ostream& operator<<(
		std::ostream& os, Matrix const& m) {
		os << m.width << "," << m.height << "," << std::endl;
		for (int i = 0; i < m.height; i++) {
			for (int j = 0; j < m.width; j++) {
				os << m.at(i, j) << ",";
			}
			os << std::endl;
		}
		return os;
	}

	// compares two Matrix objects for equality
	inline bool operator==(Matrix const& other) {
		// sanity check
		if (width != other.width || height != other.height) {
			return false;
		}
		// real check for equality
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				if (at(i, j) != other.at(i, j)) {
					return false;
				}
			}
		}
		return true;
	}


private:
	// contens of the Matrix. Aligned in memory
	int *array;
	// used for normalization. swaps two indices in the Matrix
	void swapIdx(const int, const int);

};