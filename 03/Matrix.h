class Matrix
{
	size_t _rows, _cols;
	int** _m;
	class Row
	{
		size_t _col;
		int* data;
	public:
		Row(size_t col, int *d) :
			_col(col), data(d) {}
		int& operator[](size_t col)
		{
			if (col >= _col)
				throw std::out_of_range("");
			return data[col];
		}
		const int& operator[](size_t col) const
		{
			if (col >= _col)
				throw std::out_of_range("");
			return data[col];
		}
	};
public:
	Matrix(size_t rows, size_t cols) :
		_rows(rows), _cols(cols), _m(new int*[rows]) {
		for (size_t i = 0; i < rows; i++) {
			_m[i] = new int[cols];
		}
	}
	~Matrix() {
		for (size_t i = 0; i < _rows; i++) {
			delete[] _m[i];
		}
		delete[] _m;
	}
	int getRows() { return _rows; }
	int getColumns() { return _cols; }
	Row operator[](size_t row)
	{
		if (row < 0 || row >= _rows)
			throw std::out_of_range("");
		return Row(_cols, _m[row]);
	}
	const Row operator[](size_t row) const
	{
		if (row < 0 || row >= _rows)
			throw std::out_of_range("");
		return Row(_cols, _m[row]);
	}

	bool operator==(const Matrix &R) const
	{
		if (this->_rows != R._rows || this->_cols != R._cols)
			return false;

		for (size_t i = 0; i < _rows; i++)
			for (size_t j = 0; j < _cols; j++)
				if (_m[i][j] != R[i][j]) return false;
		return true;
	}

	bool operator!=(Matrix& R) const { return !(*this == R); }

	void operator*=(int factor)
	{
		for (int i = 0; i < _rows; ++i)
			for (int j = 0; j < _cols; ++j)
				_m[i][j] *= factor;
	}
};
