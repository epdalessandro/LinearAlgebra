#include <string>
#include <cassert>
#include <cstring>
#include <iostream>

using namespace std;

template<typename T>

class Matrix {
public:
    //Default Constructor
    Matrix() : Matrix(0,0) {}
    //Base Constructor, assigns all elements to zero
    Matrix(uint32_t numRows, uint32_t numCols) : rows(numRows), columns(numCols), matrix(new T*[rows]) {
        // assert((rows >= 0) && (columns >= 0));
        for(uint32_t i = 0; i < rows; i++) {
            matrix[i] = new T[columns];
        }
        for(uint32_t row = 0; row < rows; row++) {
            for(uint32_t col = 0; col < columns; col++) {
                matrix[row][col] = 0;
            }
        }
    }
    
    //Constructor + Initializer
    Matrix(uint32_t numRows, uint32_t numCols, std::istream& matrixInit) : 
        rows(numRows), columns(numCols), matrix(new T*[rows]) {
        // assert((rows >= 0) && (columns >= 0));
        for(uint32_t i = 0; i < rows; i++) {
            matrix[i] = new T[columns];
        }

        T input;
        uint32_t row = 0;
        uint32_t col = 0;
        while(matrixInit >> input) {
            matrix[row][col] = input;
            if((++col % columns) == 0) { //reached the end of a row
                col = 0;
                row++;
            }
        }
    }

    //Copy Constructor
    Matrix(Matrix &rhs) : determinant(rhs.determinant), rows(rhs.rows), columns(rhs.columns), matrix(new T*[rows]) {
        assert((rows >= 0) && (columns >= 0));
        for(uint32_t i = 0; i < rows; i++) {
            matrix[i] = new T[rhs.columns];
        }
        copyVals(rhs);
    }
    
    //Assignment Operator
    Matrix &operator=(const Matrix &rhs) {
        if(this == &rhs) { //if same matrix
            return *this;
        }
        deleteMatrix();
        determinant = rhs.determinant;
        rows = rhs.rows;
        columns = rhs.columns;
        matrix = new T*[rows];
        for(uint32_t i = 0; i < rows; i++) {
            matrix[i] = new T[rhs.columns];
        }
        copyVals(rhs);

        assert((rows >= 0) && (columns >= 0));
        return *this;
    }

    //Move Copy Constructor
    Matrix(Matrix &&rhs) : Matrix() { //Create a default matrix to swap with
        swap(*this, rhs);
    }

    //Move Assignment Operator
    Matrix &operator=(Matrix &&rhs) {
        swap(*this, rhs);
        return *this;
    }
    
    //Destructor
    ~Matrix() {
        deleteMatrix();
    }

    ///////////////////////////////////////////////// ACCESSORS ////////////////////////////////////////////////////
    uint32_t getRows() const {
        return rows;
    }
    uint32_t getCols() const {
        return columns;
    }
    ///////////////////////////////////////////////// OPERATORS ////////////////////////////////////////////////////
    
    //Row and Column >= 0 and less than the max rows/columns
    T &operator()(uint32_t row, uint32_t col) {
        assert(row >= 0 && row < rows);
        assert(col >= 0 && col < columns);
        return matrix[row][col];
    }
    //Row and Column >= 0 and less than the max rows/columns
    const T &operator()(uint32_t row, uint32_t col) const {
        assert(row >= 0 && row < rows);
        assert(col >= 0 && col < columns);
        return matrix[row][col];
    }

    bool operator==(const Matrix &rhs) const {
        if((rows != rhs.rows) || (columns != rhs.columns)) {
            return false;
        }
        for(uint32_t row = 0; row < rows; row++) {
            for(uint32_t col = 0; col < columns; col++) {
                if(matrix[row][col] != rhs(row,col)) {
                    return false;
                }
            }
        }
        return true;
    }
    bool operator!=(const Matrix &rhs) const {
        return !(*this == rhs);
    }

    //Requires matrices of the same shape and data type
    Matrix &operator+(const Matrix &rhs) {
        assert((rows == rhs.rows) && (columns == rhs.columns));
        for(uint32_t row = 0; row < rows; row++) {
            for(uint32_t col = 0; col < columns; col++) {
                matrix[row][col] += rhs(row,col);
            }
        }
        return *this;
    }
    
    //Requires matrices of the same shape
    Matrix &operator-(const Matrix &rhs) {
        assert((rows == rhs.rows) && (columns == rhs.columns));
        for(uint32_t row = 0; row < rows; row++) {
            for(uint32_t col = 0; col < columns; col++) {
                matrix[row][col] -= rhs(row,col);
            }
        }
        return *this;
    }

    //Requires mat1.columns == mat2.rows
    Matrix &operator*(const Matrix &rhs) {
        assert(columns == rhs.rows);
        Matrix toReturn(rows, rhs.columns);

        T sum = 0;
        for(uint32_t row = 0; row < rows; row++) {
            for(uint32_t col = 0; col < rhs.columns; col++) {
                for(uint32_t elements = 0; elements < columns; elements++) {
                    sum += matrix[row][elements] * rhs(elements,col);
                }
                toReturn(row,col) = sum;
                sum = 0;
            }
        }
        *this = toReturn;
        return *this;
    }

    Matrix scale(T coefficient) {
        for(uint32_t row = 0; row < rows; row++) {
            for(uint32_t col = 0; col < columns; col++) {
                matrix[row][col] *= coefficient;
            }
        }
        return *this;
    }
    
    double determinant = 1;
    uint32_t rows;
    uint32_t columns;
    T ** matrix;

private:
    void deleteMatrix() {
        for(uint32_t row = 0; row < rows; row++) {
            delete[] matrix[row];
        }
        delete[] matrix;
    }

    void copyVals(const Matrix &rhs) {
        for(uint32_t row = 0; row < rows; row++) {
            for(uint32_t col = 0; col < columns; col++) {
                matrix[row][col] = rhs.matrix[row][col];
            }
        }
    }

    void swap(Matrix<T> &first, Matrix<T> &second) { //custom swap for matrices
        std::swap(first.rows, second.rows);
        std::swap(first.columns, second.columns);
        std::swap(first.determinant, second.determinant);
        first.matrix = second.matrix;
        second.matrix = nullptr; //need to let it destruct safely w/o destructing the swapped matrix
    }

};

template<typename T>
std::ostream &operator<<(std::ostream &os, const Matrix<T> &mat) {
    for(uint32_t row = 0; row < mat.getRows(); row++) {
        os << "[  ";
        for(uint32_t col = 0; col < mat.getCols(); col++) {
            os << mat(row,col) << " ";
        }
        os << " ]\n"; //already an extra space after the last element so only one extra
    }
    return os;
}