#include "Matrix.h"
#include <filesystem>

namespace fmtx {
	MatrixElement::MatrixElement(size_t i, size_t j, std::function<std::string(size_t, size_t)> read_fnc,
		std::function<void(size_t, size_t, const std::string&)> write_fnc) 
			: t_i(i), t_j(j), t_read_fnc(read_fnc), t_write_fnc(write_fnc)
	{
	}

	MatrixElement::operator std::string() const
	{
		return t_read_fnc(t_i, t_j);
	}

	void MatrixElement::operator=(const std::string& t)
	{
		t_write_fnc(t_i, t_j, t);
	}





	inline std::string Matrix::t_get_file_name(size_t i, size_t j)
	{
		return t_tmp_dir + "//" + std::to_string(i) + "_" + std::to_string(j) + "." + t_file_type;
	}

	Matrix::Matrix(const std::string& str, char symbol, const std::string& tmp_dir,const std::string& file_type) 
		: t_path(str), t_strings(0), t_coloums(0), t_symbol(symbol), t_tmp_dir(tmp_dir), t_file_type(file_type)
	{
		bool finish = false;
		auto path = (std::filesystem::current_path());
		for (auto& p : std::filesystem::directory_iterator{ path })
		{
			if (p.is_directory() && p.path() == path / (t_tmp_dir))
			{
				finish = true;
				break;
			}
		}
		if (!finish) {
			std::filesystem::create_directory(path / (t_tmp_dir));
		}
		std::ifstream file(t_path);
		std::ofstream buf_file;
		file.seekg(0, file.end);
		size_t i = 0;
		if (file.tellg() > 0) {
			file.seekg(0, file.beg);
			file >> t_strings >> t_coloums;
			file.seekg(2, file.cur);
			for (size_t i = 0; i < t_strings; ++i) {
				std::string str;
				std::getline(file, str, '\n');
				std::stringstream stream(str);
				for (size_t j = 0; j < t_coloums; ++j) {
					std::getline(stream, str, t_symbol);
					buf_file.open(t_get_file_name(i, j));
					buf_file << str;
					buf_file.close();
				}
			}
		}
		file.close();
		t_read_fnc = [this](size_t i, size_t j) {
			std::ifstream file(t_get_file_name(i, j));
			std::string result;
			std::getline(file, result);
			file.close();
			return result;
		};

		t_write_fnc = [this](size_t i, size_t j, const std::string& t) {
			std::ofstream file(t_get_file_name(i, j));
			file << t;
			file.close();
		};
		t_half.operator=(HalfMatrix(0, [this]() {return coloums(); }, t_read_fnc, t_write_fnc));
	}


	Matrix::~Matrix()
	{
		std::ofstream file(t_path);
		file << t_strings << " " << t_coloums << std::endl;
		if (t_strings > 0) {

			for (size_t i = 0; i < t_strings; ++i) {
				for (size_t j = 0; j < t_coloums; ++j) {
					file << t_read_fnc(i, j);
					if (j != t_coloums - 1) file << t_symbol;
					remove(t_get_file_name(i, j).c_str());
				}
				if (i != t_strings - 1)file << std::endl;
			}


		}
		file.close();
		std::filesystem::remove(std::filesystem::current_path() / t_tmp_dir);
	}


	size_t Matrix::strings() const
	{
		return t_strings;
	}


	size_t Matrix::coloums() const
	{
		return t_coloums;
	}


	void Matrix::add_string()
	{
		if (t_coloums == 0) {
			++t_coloums;
		}

		std::ofstream file;
		for (size_t i = 0; i < t_coloums; ++i) {
			file.open(t_get_file_name(t_strings, i));
			file.close();
		}
		++t_strings;
	}


	void Matrix::add_coloum()
	{
		if (t_strings == 0) {
			++t_strings;
		}
		std::ofstream file;
		for (size_t i = 0; i < t_strings; ++i) {
			file.open(t_get_file_name(i, t_coloums));
			file.close();
		}
		++t_coloums;
	}


	void Matrix::erase_string()
	{
		if (t_strings > 0) {
			for (size_t i = 0; i < t_coloums; ++i) {
				if (remove(t_get_file_name(t_strings - 1, i).c_str()) != 0) {
					throw(std::exception(""));
				}
			}
			--t_strings;
		}
		else {
			throw(std::invalid_argument("ошибка удаления"));
		}
	}


	void Matrix::erase_coloum()
	{
		if (t_coloums > 0) {
			for (size_t i = 0; i < t_strings; ++i) {
				if (remove(t_get_file_name(i, t_coloums - 1).c_str()) != 0) {
					throw(std::exception(""));
				}
			}
			--t_coloums;
		}
		else {
			throw(std::invalid_argument("ошибка удаления"));
		}
	}


	void Matrix::clear()
	{
		size_t x = t_strings;
		for (size_t i = 0; i < x; ++i) {
			erase_string();
		}
		t_strings = 0;
		t_coloums = 0;

	}


	bool Matrix::is_empty() const
	{
		return t_strings * t_coloums == 0;
	}

	Matrix::HalfMatrix& Matrix::operator[](size_t i)
	{
		if (i >= t_strings)throw(std::invalid_argument("выход за границы матрицы"));
		t_half.t_i = i;
		return t_half;
	}

	const Matrix::HalfMatrix Matrix::operator[](size_t i)const
	{
		if (i >= t_strings)throw(std::invalid_argument("выход за границы матрицы"));
		return HalfMatrix(i, [this]() {return coloums(); }, t_read_fnc, t_write_fnc);
	}



	Matrix::HalfMatrix::HalfMatrix()
	{
	}

	Matrix::HalfMatrix::HalfMatrix(size_t i, std::function<size_t(void)> max_j, std::function<std::string(size_t, size_t)> read_fnc, std::function<void(size_t, size_t, const std::string&)> write_fnc)
		: t_i(i), t_max_j(max_j), t_read_fnc(read_fnc), t_write_fnc(write_fnc)
	{
	}

	const MatrixElement Matrix::HalfMatrix::operator[](size_t j)const
	{
		t_max_j();
		if (j >= t_max_j())throw(std::invalid_argument("выход за границы матрицы"));
		return MatrixElement(t_i, j, t_read_fnc, t_write_fnc);
	}

	MatrixElement Matrix::HalfMatrix::operator[](size_t j)
	{
		t_max_j();
		if (j >= t_max_j())throw(std::invalid_argument("выход за границы матрицы"));
		return MatrixElement(t_i, j, t_read_fnc, t_write_fnc);
	}

	void Matrix::HalfMatrix::operator=(const HalfMatrix& m)
	{
		t_i = m.t_i;
		t_max_j = m.t_max_j;
		t_read_fnc = m.t_read_fnc;
		t_write_fnc = m.t_write_fnc;
	}

	inline std::string t_get_file_name(size_t i, size_t j, std::string dir, std::string type)
	{
		return dir + "//" + std::to_string(i) + "_" + std::to_string(j) + "." + type;
	}
}