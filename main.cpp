#include <iostream>
#include <random>
#include <vector>
#include <array>
#include <map>
#include <algorithm>
#include <cassert>
#include <queue>
#include <ctime>
#include <cmath>

#define DEBUG_HEAT 1
#define DEBUG_DIAGONAL 1
#define SYMBOL_X 'X'
#define SYMBOL_O 'O'
#define SYMBOL_BOUND 'B'
#define SYMBOL_EMPTY '-'

using Coordinate = unsigned;
using Symbol = char;
using Diagonal = std::array<Symbol, 9>;

class Coordinates {
public:
	Coordinate x;
	Coordinate y;
	Coordinates(Coordinate x, Coordinate y) : x(x), y(y) {}
	Coordinates() {}
};

unsigned random(unsigned from, unsigned to) {
	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_int_distribution<> distribution(from, to);
	return (unsigned int) distribution(generator );
}

class TicTacToe {

	Coordinate boardSize;
	std::vector<char> board;
	std::vector<bool> boardCheckPositions;
	Symbol ourSymbol;
	Symbol enemySymbol;

	int checkDirections[8][2] = {
		{-1, 0}, // vlevo
		{-1, -1}, // vlevo nahoru
		{0, -1}, // nad
		{1, -1}, // vpravo nahoru
		{1, 0}, // vpravo
		{1, 1}, // vpravo dolu
		{0, 1}, // dolu
		{-1, 1} // vlevo dolu
	};

public:
	void run(std::istream &in, std::ostream &out, std::ostream &err) {
		in >> boardSize;
		initPlayground();

		do {
			in >> ourSymbol;
			if (ourSymbol != SYMBOL_X && ourSymbol != SYMBOL_O) {
				err << "Spatny symbol pro Skynet AI, dovolene jsou pouze X a O ... zkus to znova a nezapomen na SHIFT!" << std::endl;
				continue;
			}

			enemySymbol = ourSymbol == SYMBOL_X ? SYMBOL_O : SYMBOL_X;
			break;

		} while (true);

		Coordinates ourPlacement;
		int playerX, playerY;
		while (true) {
			if (ourSymbol == SYMBOL_X) {
				ourPlacement = computerPlace(out, err);
				out << ourPlacement.x << " " << ourPlacement.y << std::endl;
			}

			do {
				in >> playerX;
				in >> playerY;
				if (playerX == -1 && playerY == -1) {
					return; // end
				}
			} while (!enemyPlace({(Coordinate) playerX, (Coordinate) playerY}, err));

			if (ourSymbol == SYMBOL_O) {
				ourPlacement = computerPlace(out, err);
				out << ourPlacement.x << " " << ourPlacement.y << std::endl;
			}
		}
	}

	bool enemyPlace(const Coordinates &point, std::ostream &err) {
		if (point.x > boardSize - 1 || point.y > boardSize - 1) {
			err << "out of bounds: fuck off" << std::endl;
			return false;
		}

		if (!placeSymbol(enemySymbol, point)) {
			err << "uz tam neco je vole: "<< board[positionFromCoordinates(point)] << std::endl;
			return false;
		}

		return true;
	}

	Coordinates computerPlace(std::ostream &out, std::ostream &err) {
		Coordinates finalPlacement;

		int maxHeat = 0;
		Coordinates maxHeatPosition;
		std::vector<unsigned> heatLevels(boardSize * boardSize);

		for (Coordinate y = 0; y < boardSize ;y++) {
			for (Coordinate x = 0; x < boardSize ;x++) {
				Coordinates originCursor(x, y);
				Coordinate originCursorPosition = positionFromCoordinates(originCursor);

				if (!boardCheckPositions[originCursorPosition]) {
					heatLevels[originCursorPosition] = 0;
					continue; // ignore
				}


				if (board[originCursorPosition] != SYMBOL_EMPTY) {
					if (DEBUG_HEAT) {
						err << "Queue na kontrolu obsahuje neprazdne policko: " << originCursor.x << "x" << originCursor.y << std::endl;
					}
					continue;
				}

				for (size_t dir = 0; dir < 4; dir++) { // intentionally only 4 !!!
//					if (DEBUG_DIAGONAL) {
//						std::vector<char> diagonalSelectionBoard = board; // copy
//					}

					Diagonal diagonal;
					Coordinates diffCursor(
						originCursor.x + checkDirections[dir][0] * 5,
						originCursor.y + checkDirections[dir][1] * 5
					);
					for (int i = 0; i < 9; i++) {
						diffCursor.x += checkDirections[dir][0] * -1;
						diffCursor.y += checkDirections[dir][1] * -1;
						if (!isValidCoordinates(diffCursor.x, diffCursor.y)) {
							diagonal[i] = SYMBOL_BOUND;
							continue;
						}

						Coordinate diagonalPosition = positionFromCoordinates(diffCursor);
						diagonal[i] = board[diagonalPosition];
//						if (DEBUG_DIAGONAL) {
//							diagonalSelectionBoard[diagonalPosition] = (i == 4) ? 'M' : ('0' + i);
//						}
					}

//					if (DEBUG_DIAGONAL) {
//						debugPlayground(out, diagonalSelectionBoard);
//					}

					size_t change = calculateDiagonalHeat(diagonal);

					heatLevels[originCursorPosition] += change;
					if ((int) heatLevels[originCursorPosition] > maxHeat) {
						maxHeat = heatLevels[originCursorPosition];
						maxHeatPosition = originCursor;
					}
				}
			}
		}

		if (maxHeat > 0) {
			finalPlacement = maxHeatPosition;
			placeSymbol(ourSymbol, finalPlacement);

		} else {
			do {
				int middle = (boardSize - 1) / 2;
				finalPlacement = {
					random(middle - (middle /2), middle + (middle /2)),
					random(middle - (middle /2), middle + (middle /2))
				};
				if (placeSymbol(ourSymbol, finalPlacement)) {
					break;
				}

			} while(true);
		}

		if (DEBUG_HEAT) {
			debugHeatLevels(out, finalPlacement, heatLevels);
		}

		return finalPlacement;
	}

	size_t calculateDiagonalHeat(const Diagonal &diagonal) const {
		return std::max<size_t>(
			calculateDiagonalHeatForSymbol(ourSymbol, diagonal)
			+ calculateDiagonalHeatForSymbol(enemySymbol, diagonal),
		    0
		);
	}

	size_t calculateDiagonalHeatForSymbol(Symbol symbol, const Diagonal &diagonal) const {
		size_t heat = 0;
		size_t playableFields = 1; // middle
		bool symbolsInRow = true; // middle
		Symbol opositeSymbol = symbol == SYMBOL_O ? SYMBOL_X :SYMBOL_O;

		for (int i = -1; i <= 1 ; i += 2) { // both directions from position 4
			for (int j = 4 + i; j > 0 && j < 9 ;j += i) {
				if (diagonal[j] == SYMBOL_BOUND || diagonal[j] == opositeSymbol) {
					break; // nowhere to play here
				}

				if (diagonal[j] == SYMBOL_EMPTY) {
					symbolsInRow = false;
				}

				playableFields++; // our or empty
				if (symbolsInRow) {
					heat += pow(2, i > 0 ? (j - 4) : (4 - j));
				}
			}
		}

		return playableFields >= 5 ? heat : 0;
	}

	bool placeSymbol(char symbol, const Coordinates &point) {
		Coordinate placementPosition = positionFromCoordinates(point);
		if (board[placementPosition] != SYMBOL_EMPTY) {
			return false;
		}

		// place symbol
		board[placementPosition] = symbol;

		// remove placement position from queue
		boardCheckPositions[placementPosition] = false;

		// queue neighbour fields
		for (size_t dir = 0; dir < 8; dir++) {
			int dx = point.x + checkDirections[dir][0];
			int dy = point.y + checkDirections[dir][1];
			if (!isValidCoordinates(dx, dx)) {
				continue;
			}

			Coordinate diffCursorPosition = positionFromCoordinates({(Coordinate)dx, (Coordinate)dy});
			if (board[diffCursorPosition] == SYMBOL_EMPTY) {
				boardCheckPositions[diffCursorPosition] = true;
			}
		}

		return true;
	}

	Coordinate positionFromCoordinates(Coordinates point) {
		return point.x + (point.y * boardSize);
	}

	bool isValidCoordinates(int x, int y) {
		return !(x < 0 || x > ((int) boardSize) - 1)
			&& !(y < 0 || y > ((int) boardSize) - 1);
	}

	void debugPlayground(std::ostream &out, const std::vector<char> boardData) {
		out << " ";
		for (Coordinate x = 0; x < boardSize ;x++) {
			out << " " << x % 10;
		}
		out << std::endl;
		for (Coordinate y = 0; y < boardSize ;y++) {
			out << y % 10 << " ";
			for (Coordinate x = 0; x < boardSize ;x++) {
				out << boardData[positionFromCoordinates({x, y})] << " ";
			}
			out << std::endl;
		}
		out << std::endl;
	}

	void debugHeatLevels(std::ostream &out, const Coordinates &maxHeatPosition, const std::vector<unsigned> &heatLevels) {
		Coordinate boardFields = boardSize * boardSize;
		std::vector<char> boardCopy(boardFields);

		for (Coordinate position = 0; position < (boardFields) ;position++) {
			if (board[position] != SYMBOL_EMPTY || heatLevels[position] == 0) {
				boardCopy[position] = board[position];
				continue;
			}

			boardCopy[position] = heatLevels[position] > 9 ? '9' : ('0' + heatLevels[position]);
		}

		boardCopy[positionFromCoordinates(maxHeatPosition)] = '#';
		debugPlayground(out, boardCopy);

		for (Coordinate position = 0; position < (boardFields) ;position++) {
			boardCopy[position] = boardCheckPositions[position] ? '?' : board[position];
		}
		debugPlayground(out, boardCopy);
	}

	void initPlayground() {
		board = std::vector<char>(boardSize * boardSize);
		for (Coordinate y = 0; y < boardSize ;y++) {
			for (Coordinate x = 0; x < boardSize ;x++) {
				board[positionFromCoordinates({x, y})] = SYMBOL_EMPTY;
			}
		}

		boardCheckPositions = std::vector<bool>(boardSize * boardSize);
		for (Coordinate y = 0; y < boardSize ;y++) {
			for (Coordinate x = 0; x < boardSize ;x++) {
				boardCheckPositions[positionFromCoordinates({x, y})] = false;
			}
		}
	}
};

int main() {
	TicTacToe ttt;
	ttt.run(std::cin, std::cout, std::cerr);
	return 0;
}
