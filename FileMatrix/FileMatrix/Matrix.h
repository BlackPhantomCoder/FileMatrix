#pragma once
#include <string>
#include <fstream>
#include <functional>
#include <vector>
#include <sstream>
#include <cstdio>
#include <exception>
#include <memory>



namespace fmtx {

	//Элемент матрицы
	class MatrixElement {
	public:
		MatrixElement(size_t i, size_t j, std::function<std::string(size_t, size_t)> read_fnc,
			std::function<void(size_t, size_t, const std::string&)> write_fnc);
		operator std::string() const;
		void operator= (const std::string& t);
	private:
		std::function <void(size_t, size_t, const std::string&)> t_write_fnc;
		std::function <std::string(size_t, size_t)> t_read_fnc;
		size_t t_i, t_j;
	};



	class Matrix
	{
	protected:
		//Вспомогательный класс для реализации синтаксиса [][]
		class HalfMatrix {
		public:
			HalfMatrix();
			HalfMatrix(size_t i, std::function<size_t(void)> max_j, std::function<std::string(size_t, size_t)> read_fnc,
				std::function<void(size_t, size_t, const std::string&)> write_fnc);
			const MatrixElement operator[](size_t j)const;
			MatrixElement operator[](size_t j);
		private:
			friend Matrix;
			std::function<void(size_t, size_t, const std::string&)> t_write_fnc;
			std::function<std::string(size_t, size_t)> t_read_fnc;
			size_t t_i;
			std::function<size_t(void)> t_max_j;
			void operator=(const HalfMatrix&);
		};
		//Получить путь к буферному файлу
		inline std::string t_get_file_name(size_t i, size_t j);
	public:
		Matrix(const std::string& str, char symbol = '|', const std::string& tmp_dir = ".tmp", const std::string& file_type = ".el");
		~Matrix();

		//Число строк
		size_t strings()const;
		//Число столбцов
		size_t coloums()const;
		//Добавить строку в конец
		void add_string();
		//Добавить столбец в конец
		void add_coloum();
		//Удалить строку с конца
		void erase_string();
		//Удалить столбец с конца
		void erase_coloum();
		//Удалить всё
		void clear();
		//проверка на наличие элементов
		bool is_empty()const;

		//Оператор доступа к элементам
		const Matrix::HalfMatrix operator[](size_t i)const;
		Matrix::HalfMatrix& operator[](size_t i);
	private:
		std::function< std::string(size_t, size_t)> t_read_fnc;
		std::function<void(size_t, size_t, const std::string&)> t_write_fnc;
		size_t t_strings;
		size_t t_coloums;
		std::string t_path;
		HalfMatrix t_half;

		const char t_symbol;
		const std::string t_tmp_dir;
		const std::string t_file_type;

		void operator=(const Matrix&) {}
	};
}