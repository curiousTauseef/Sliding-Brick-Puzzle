#include "Matrix.h"


Matrix::Matrix()
	: width(0), height(0), array(new int[0]) {
}


Matrix::Matrix(const Matrix& other) {
	swap(*this, other);
}


Matrix::Matrix(const std::string path) {
	/* Step 0: Error check. Check if file exists */
	std::ifstream inStream(path);
	if (inStream.fail()) {
		std::cout << "Opening file '" << path << "' failed."
			<< " Either it does not exist or is not accessible." << std::endl;;
		std::exit(2);
	}
	/* Step 1: load from disk if exists. put puzzle in vec<vec> */
	std::string str;
	std::vector<std::vector<int>> vec;
	for (int i = 0; std::getline(inStream, str); i++) {
		std::vector<int> innerVec;
		for (unsigned int j = 0; j < str.length(); j++) {
			std::string num = str.substr(j, str.substr(j, str.length()).find(','));
			j += num.length();
			innerVec.push_back(std::stoi(num));
		}
		if (i > 0) {
			vec.push_back(innerVec);
		}
	}
	/* Step 2: set class members */
	width = vec[0].size();
	height = vec.size();
	array = new int[width * height];
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			at(i, j) = vec[i][j];
		}
	}
}


Matrix::~Matrix() {
	delete[] array;
}


int& Matrix::at(const int row, const int col) const {
	return array[row * width + col];
}


const bool Matrix::isSolved() const {
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (at(i, j) == -1)
				return false;
		}
	}
	return true;
}


std::vector<Moves> Matrix::getMoves(const int piece) const {
	std::vector<Moves> moves;
	if (piece < 2) return moves; // invalid piece!
	moves.reserve(4); // max entries is 4 (UP, DOWN, LEFT, RIGHT)

	std::vector<std::pair<int, int>> indices = getPieceIndices(piece);
	if (indices.size() == 0) return moves; // piece not in matrix!

	// find out width and height of piece
	std::pair<int, int> pieceDim = getPieceDim(indices);
	int pieceW = pieceDim.first;
	int pieceH = pieceDim.second;

	// check possible movements for piece
	// movement is only possible, if the cells where the piece
	// would "move to" are all 0
	// the piece 2 (master block) can also move when -1!
	int pieceX = indices[0].first;
	int pieceY = indices[0].second;
	int val = 0;
	// UP 
	for (int i = 0; i < pieceW; i++) {
		val = at(pieceY - 1, pieceX + i); // idx_y, idx_x
		if (piece == 2) {
			if (val > 0) break;
		}
		else {
			if (val != 0) break;
		}
		if (i == pieceW - 1) {
			moves.push_back(Moves::UP);
		}
	}
	// DOWN 
	for (int i = 0; i < pieceW; i++) {
		val = at(pieceY + pieceH, pieceX + i); // idx_y, idx_x
		if (piece == 2) {
			if (val > 0) break;
		}
		else {
			if (val != 0) break;
		}
		if (i == pieceW - 1) {
			moves.push_back(Moves::DOWN);
		}
	}
	// LEFT 
	for (int i = 0; i < pieceH; i++) {
		val = at(pieceY + i, pieceX - 1); // idx_y, idx_x
		if (piece == 2) {
			if (val > 0) break;
		}
		else {
			if (val != 0) break;
		}
		if (i == pieceH - 1) {
			moves.push_back(Moves::LEFT);
		}
	}
	// RIGHT
	for (int i = 0; i < pieceH; i++) {
		val = at(pieceY + i, pieceX + pieceW); // idx_y, idx_x
		if (piece == 2) {
			if (val > 0) break;
		}
		else {
			if (val != 0) break;
		}
		if (i == pieceH - 1) {
			moves.push_back(Moves::RIGHT);
		}
	}
	return moves;
}


std::vector<std::pair<int, int>> Matrix::getPieceIndices(const int piece) const {
	std::vector<std::pair<int, int>> indices;
	indices.reserve((width - 1) * (height - 1));
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (at(i, j) == piece) { // j=x, i=y coord.
				/* the first index in the list will always
				 be the top left cell of the piece*/					 
				indices.push_back(std::pair<int, int>(j, i));
			}
		}
	}
	return indices;
}


std::pair<int, int> Matrix::getPieceDim(std::vector<std::pair<int, int>> indices) const {
	std::pair<int, int> dim(0, 0);
	int tmpX = -1, tmpY = -1;
	for (unsigned int i = 0; i < indices.size(); i++) {
		if (indices[i].first > tmpX) {
			tmpX = indices[i].first;
			dim.first++;
		}
		if (indices[i].second > tmpY) {
			tmpY = indices[i].second;
			dim.second++;
		}
	}
	return dim;
}


std::unordered_map<unsigned int, std::vector<Moves>> Matrix::getAllMoves() const {
	std::unordered_map<unsigned int, std::vector<Moves>> map;
	for (int i = 1; i < height - 1; i++) { // 1 & -1 leaves out edges
		for (int j = 1; j < width - 1; j++) {
			unsigned int piece = at(i, j);
			if (piece > 1) {
				std::vector<Moves> pieceMoves = getMoves(piece);
				if (!pieceMoves.empty()) {
					/* std::map allows only unique keys (pieces)
					 no duplicate-check has to be performed here!*/
					map.insert(std::make_pair(piece, pieceMoves));
				}
			}
		}
	}
	return map;
}


void Matrix::applyMove(const int piece, const Moves move) {
	std::vector<std::pair<int, int>> indices;
	// collect indices to change in order for piece to move
	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			if (at(i, j) == piece) {
				switch (move) {
				case Moves::UP:
					indices.push_back(std::make_pair(i - 1, j));
					break;
				case Moves::DOWN:
					indices.push_back(std::make_pair(i + 1, j));
					break;
				case Moves::LEFT:
					indices.push_back(std::make_pair(i, j - 1));
					break;
				case Moves::RIGHT:
					indices.push_back(std::make_pair(i, j + 1));
					break;
				}
				at(i, j) = 0;
			}
		}
	}
	// change matrix
	for (unsigned int i = 0; i < indices.size(); i++) {
		at(indices[i].first, indices[i].second) = piece;
	}
}


Matrix Matrix::applyMoveCloning(const int piece, const Moves move) {
	Matrix m_clone(*this);
	m_clone.applyMove(piece, move);
	return m_clone;
}


void Matrix::normalize() {
	int nextIdx = 3;
	for (int i = 1; i < height - 1; i++) { // 1 & -1 leaves out edges
		for (int j = 1; j < width - 1; j++) {
			int *p = &at(i, j); // j,i to go through rows!
			if (*p == nextIdx) {
				nextIdx++;
			}
			else if (*p > nextIdx) {
				swapIdx(nextIdx, *p);
				nextIdx++;
			}
		}
	}
}


void Matrix::swapIdx(const int idx1, const int idx2) {
	for (int i = 1; i < height - 1; i++) {
		for (int j = 1; j < width - 1; j++) {
			int *p = &at(i, j);
			if (*p == idx1) {
				*p = idx2;
			}
			else if (*p == idx2) {
				*p = idx1;
			}
		}
	}
}

void swap(Matrix& lhs, const Matrix& rhs) {
	lhs.width = rhs.width;
	lhs.height = rhs.height;
	lhs.array = new int[lhs.width * lhs.height];
	for (int i = 0; i < lhs.height; i++) {
		for (int j = 0; j < lhs.width; j++) {
			lhs.at(i, j) = rhs.at(i, j);
		}
	}
}