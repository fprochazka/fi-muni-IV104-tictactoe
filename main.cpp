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
		int x, y;
		while (true) {
			if (ourSymbol == 'X') {
				ourPlacement = computerPlace();
				out << ourPlacement.first << " " << ourPlacement.second << std::endl;
				debugPlayground(out);
			}

			do {
				in >> x;
				in >> y;
				if (x == -1 && y == -1) {
					return; // end
				}
			} while (!enemyPlace((Coordinate) x, (Coordinate) y, err));

			if (ourSymbol == 'O') {
				ourPlacement = computerPlace();
				out << ourPlacement.first << " " << ourPlacement.second << std::endl;
				debugPlayground(out);
			}
		}
	}

	bool enemyPlace(Coordinate x, Coordinate y, std::ostream &err) {
		if (x > boardSize - 1 || y > boardSize - 1) {
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
		Coordinates finalPlacement;

		int checkDirections[8][2] = {
			{-1, -1}, // vlevo nahoru
			{0, -1}, // nad
			{1, -1}, // vpravo nahoru
			{1, 0}, // vpravo
			{1, 1}, // vpravo dolu
			{0, 1}, // dolu
			{-1, 1}, // vlevo dolu
			{-1, 0} // vlevo
		};

		int maxDanger = 0;
		Coordinates maxDangerPosition;
		std::vector<unsigned> dangerLevels(boardSize * boardSize);

		int maxAttack = 0;
		Coordinates maxAttackPosition;
		std::vector<unsigned> attackLevels(boardSize * boardSize);

		for (Coordinate y = 0; y < boardSize ;y++) {
			for (Coordinate x = 0; x < boardSize ;x++) {
				if (board[positionFromCoordinates(x, y)] == enemySymbol) {
					for (size_t d = 0;d < 8; d++) {
						Coordinate dx = x;
						Coordinate dy = y;
						for (size_t change = 1; change <= 4; change++) {
							if ((int) (dx + checkDirections[d][0]) < 0 || (int) (dx + checkDirections[d][0]) > ((int) boardSize - 1)) {
								break; // bound
							}
							if ((int) (dy + checkDirections[d][1]) < 0 || (int) (dy + checkDirections[d][1]) > ((int) boardSize - 1)) {
								break; // bound
							}

							dx += checkDirections[d][0];
							dy += checkDirections[d][1];

							Coordinate position = positionFromCoordinates(dx, dy);
							if (board[position] == ourSymbol) {
								break;
							}
							if (board[position] == '-') {
								dangerLevels[position] += change;
								if ((int) dangerLevels[position] > maxDanger) {
									maxDanger = dangerLevels[position];
									maxDangerPosition = {dx, dy};
								}
								break;
							}
						}
					}

				} else if (board[positionFromCoordinates(x, y)] == ourSymbol) {
					for (size_t d = 0;d < 8; d++) {
						Coordinate dx = x;
						Coordinate dy = y;
						for (size_t change = 1; change <= 4; change++) {
							if ((int) (dx + checkDirections[d][0]) < 0 || (int) (dx + checkDirections[d][0]) > ((int) boardSize - 1)) {
								break; // bound
							}
							if ((int) (dy + checkDirections[d][1]) < 0 || (int) (dy + checkDirections[d][1]) > ((int) boardSize - 1)) {
								break; // bound
							}

							dx += checkDirections[d][0];
							dy += checkDirections[d][1];

							Coordinate position = positionFromCoordinates(dx, dy);
							if (board[position] == enemySymbol) {
								break;
							}
							if (board[position] == '-') {
								attackLevels[position] += change > 3 ? 1337 : change;
								if ((int) attackLevels[position] > maxAttack) {
									maxAttack = attackLevels[position];
									maxAttackPosition = {dx, dy};
								}
								break;
							}
						}
					}
				}
			}
		}

//		if (maxAttack > 0 && (maxAttack - maxDanger > 2 || maxDanger < 2)) {
//			board[positionFromCoordinates(maxAttackPosition.first, maxAttackPosition.second)] = ourSymbol;
//			return maxAttackPosition;
//		}

		if (maxDanger > 1) {
			board[positionFromCoordinates(maxDangerPosition.first, maxDangerPosition.second)] = ourSymbol;
			return maxDangerPosition;
		}

//		finalPlacement = {(boardSize - 1) / 2, (boardSize - 1) / 2};
//		board[positionFromCoordinates(finalPlacement.first, finalPlacement.second)] = ourSymbol;

		do {
			finalPlacement = {random(boardSize - 1), random(boardSize - 1)};
			Coordinate position = positionFromCoordinates(finalPlacement.first, finalPlacement.second);
			if (board[position] == '-') {
				board[position] = ourSymbol;
				break;
			}

		} while(true);

		return finalPlacement;
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
	ttt.run(std::cin, std::cout, std::cerr);
	return 0;
}
