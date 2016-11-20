/*
* by Jiahuan.Liu
* 04/18/2016
* @SCUT
*/
#ifndef UTIL_H_
#define UTIL_H_

#include <vector>
#include <math.h>
#include <iostream>

#include "Object.h"

#define Min(a, b) ((a) < (b) ? (a) : (b))
#define Max(a, b) ((a) > (b) ? (a) : (b))
#define Abs(a, b) ((a) > (b) ? (a) - (b) : (b) - (a))

#define PLUS_INFINITY 1.0E32

void assignmentoptimal(float *assignment, float *cost, float *distMatrixIn, int nOfRows, int nOfColumns, float infval);

template <typename Val>
class Matrix
{
public:
	Matrix()
		:_dim1(0), _dim2(0), _data(NULL) {}
	Matrix(size_t dim1, size_t dim2)
		:_dim1(dim1), _dim2(dim2)
	{
		_data = new Val[_dim1 * _dim2];
		for (size_t i = 0; i < _dim1 * _dim2; ++i)
			_data[i] = Val();
	}
	virtual ~Matrix()
	{
		delete []_data;
	}
	void operator=(const Matrix<Val>& mat)
	{
		_dim1 = mat._dim1;
		_dim2 = mat._dim2;
		delete[]_data;
		_data = new Val[_dim1 * _dim2];
		for (size_t i = 0; i < _dim1 * _dim2; ++i)
			_data[i] = mat._data[i];
	}
	bool get(size_t iDim1, size_t iDim2, Val& val)
	{
		if (iDim1 >= _dim1 || iDim2 >= _dim2)
			return false;
		val = _data[iDim1 + _dim1 * iDim2];
		return true;
	}
	Val get(size_t iDim1, size_t iDim2)
	{
		if (iDim1 >= _dim1 || iDim2 >= _dim2)
			return Val();
		return _data[iDim1 + _dim1 * iDim2];
	}
	bool set(size_t iDim1, size_t iDim2, const Val& val)
	{
		if (iDim1 >= _dim1 || iDim2 >= _dim2)
			return false;
		_data[iDim1 + _dim1 * iDim2] = val;
		return true;
	}
	Val get(Position pos)
	{
		if ((size_t)pos.x >= _dim1 || (size_t)pos.y >= _dim2)
			return Val();
		return _data[(size_t)pos.x + _dim1 * (size_t)pos.y];
	}
	bool set(Position pos, const Val& val)
	{
		if ((size_t)pos.x >= _dim1 || (size_t)pos.y >= _dim2)
			return false;
		_data[(size_t)pos.x + _dim1 * (size_t)pos.y] = val;
		return true;
	}
	Val min(size_t index, size_t dim, size_t beg, size_t end)
	{
		if (index >= (dim == 1 ? _dim1 : _dim2) ||
			beg < 0 || end > (dim == 1 ? _dim2 : _dim1))
			return false;
		Val curr;
		Val tempMin = Val();
		for (int i = beg; i < end; ++i)
		{
			curr = (dim == 1 ? this->get(index, i) : this->get(i, index));
			if (i == 0)
				tempMin = curr;
			if (curr < tempMin)
				tempMin = curr;
		}
		return tempMin;
	}
	size_t getSize(size_t dim = 1)
	{
		return  (dim == 1 ? _dim1 : _dim2);
	}
	Val *getDataPtr()
	{
		return _data;
	}
	void print()
	{
		for (size_t j = 0; j < _dim2; j++)
		{
			for (size_t i = 0; i < _dim1; i++)
			{
				std::cout << this->get(i, j) << "  ";
			}
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}
private:
	size_t _dim1;
	size_t _dim2;
	Val* _data;
};

template <typename Type>
class GridMat
{
public:
	GridMat()
		:_dim1(0), _dim2(0), _mat() {}
	GridMat(size_t matDim1, size_t matDim2, size_t gridDim1, size_t gridDim2)
		:_dim1(matDim1 / gridDim1), _dim2(matDim2 / gridDim2), _mat(matDim1, matDim2) {}
	//Type get(size_t matDim1, size_t matDim2);
	//bool set(size_t matDim1, size_t matDim2, Type& val);
private:
	size_t _dim1;
	size_t _dim2;
	Matrix<Type> _mat;
};

#endif