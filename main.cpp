#include <iostream>
#include <random>
#include <vector>
#include <array>
#include <map>
#include <algorithm>
#include <cassert>
#include <queue>
#include <ctime>

using Coordinate = unsigned;
using Coordinates = std::pair<Coordinate, Coordinate>;
using Symbol = char;

unsigned random(unsigned max) {
	std::random_device device;
	std::mt19937 generator(device());
	std::uniform_int_distribution<> distribution(0, max);
	return (unsigned int) distribution(generator );
}

class TicTacToe {

	Coordinate boardSize;
	std::vector<char> board;
	Symbol ourSymbol;
	Symbol enemySymbol;

public:
	void run(std::istream &in, std::ostream &out, std::ostream &err) {
		in >> boardSize;
		initPlayground();

		in >> ourSymbol;
		enemySymbol = ourSymbol == 'X' ? 'O' : 'X';

		Coordinates ourPlacement;
		Coordinate x, y;
		while (true) {
			in >> x;
			in >> y;
			if (!enemyPlace(x, y, err)) {
				continue;
			}
			ourPlacement = computerPlace();
			out << ourPlacement.first << " " << ourPlacement.second << std::endl;
			debugPlayground(out);
		}
	}



	bool enemyPlace(Coordinate x, Coordinate y, std::ostream &err) {
		if (x > boardSize || y > boardSize) {
			err << "out of bounds: fuck off" << std::endl;
			return 0;
		}

		if (board[positionFromCoordinates(x, y)] != '-') {
			err << "uz tam neco je vole: "<< board[positionFromCoordinates(x, y)] << std::endl;
			return 0;
		}

		board[positionFromCoordinates(x, y)] = enemySymbol;
		return 1;
	}

	Coordinates computerPlace() {
		Coordinates placement(random(boardSize - 1), random(boardSize - 1));
		board[positionFromCoordinates(placement.first, placement.second)] = ourSymbol;

		return placement;
	}

	Coordinate positionFromCoordinates(Coordinate x, Coordinate y) {
		return x + (y * boardSize);
	}

	void debugPlayground(std::ostream &out) {
		for (Coordinate y = 0; y < boardSize ;y++) {
			for (Coordinate x = 0; x < boardSize ;x++) {
				out << board[positionFromCoordinates(x, y)] << " ";
			}
			out << std::endl;
		}
	}

	void initPlayground() {
		board = std::vector<char>(boardSize * boardSize);
		for (Coordinate y = 0; y < boardSize ;y++) {
			for (Coordinate x = 0; x < boardSize ;x++) {
				board[positionFromCoordinates(x, y)] = '-';
			}
		}
	}
};

int main() {
	TicTacToe ttt;
	ttt.run(std::cin, std::cout);
	return 0;
}
