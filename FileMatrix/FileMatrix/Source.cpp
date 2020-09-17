//C++17

#include <iostream>

#include "Matrix.h"

using namespace fmtx;

//Вывод
void output(const Matrix& matrix);

int main() {
	Matrix matrix("input.txt");

	output(matrix);

	//matrix.add_coloum();
	//matrix.add_coloum();
	//matrix.add_coloum();
	//matrix.erase_coloum();
	//matrix.erase_coloum();
	//matrix.erase_coloum();
	for (size_t i = 0; i < matrix.strings(); ++i) {
		matrix[0][i] = std::to_string(matrix.strings() - i);
	}
	//matrix.clear();
}

void output(const Matrix& matrix) {
	for (size_t i = 0; i < matrix.strings(); ++i) {
		for (size_t j = 0; j < matrix.coloums() - 1; ++j) {
			std::cout << std::string(matrix[i][j]) << " ";
		}
		std::cout << std::string(matrix[i][matrix.coloums() - 1]) << std::endl;
	}
	std::cout << matrix.strings() << " " << matrix.coloums() << std::endl;
}