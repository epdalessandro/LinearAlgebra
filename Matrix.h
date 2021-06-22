#include <string>
#include <cassert>
#include <cstring>
#include <iostream>

using namespace std;

template<typename T>

class Matrix {
public:
    //Default Constructor
    //REQUIRES: Nothing
    //MODIFIES: this
    //EFFECTS: Creates an empty matrix
    Matrix() : Matrix(0,0) {}

    //Base Constructor, assigns all elements to zero
    //REQUIRES: numRows >= 0, numCols >= 0, matrixInit is valid
    //MODIFIES: this
    //EFFECTS: Creates a matrix of size numRows x numCols with all values set to zero
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
    //REQUIRES: numRows >=0, numCols >=0, matrixInit is valid
    //MODIFIES: this
    //EFFECTS: Creates a matrix of size numRows x numCols with values read in from matrixInit
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
    //REQUIRES: rhs is a valid matrix
    //MODIFIES: this
    //EFFECTS: Default constructs this with atomic type variables of rhs, then deep copies the rhs matrix
    Matrix(Matrix &rhs) : determinant(rhs.determinant), rows(rhs.rows), columns(rhs.columns), matrix(new T*[rows]) {
        assert((rows >= 0) && (columns >= 0));
        for(uint32_t i = 0; i < rows; i++) {
            matrix[i] = new T[rhs.columns];
        }
        copyVals(rhs);
    }
    
    //Assignment Operator
    //REQUIRES: rhs is a valid matrix
    //MODIFIES: this
    //EFFECTS: Deep copy rhs member variables to this
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
    //REQUIRES: rhs is a valid rvalue Matrix
    //MODIFIES: this
    //EFFECTS: Creates a default matrix to swap with (allowing for safe deconstruction of rhs after swapping)
    //         Steals the data from rhs by swapping it with the default matrix, functionally a shallow copy 
    Matrix(Matrix &&rhs) : Matrix() { //Create a default matrix to swap with
        swap(*this, rhs);
    }

    //Move Assignment Operator
    //REQUIRES: rhs is a valid rvalue Matrix
    //MODIFIES: this
    //EFFECTS: Steals the data from rhs by swapping it with this (allowing for safe deconstruction of rhs after swapping)
    //         Functionally a shallow copy
    Matrix &operator=(Matrix &&rhs) {
        swap(*this, rhs);
        return *this;
    }
    
    //Destructor
    //REQUIRES: Nothing
    //MODIFIES: matrix
    //EFFECTS: Deletes the matrix member variable
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
    //REQUIRES: row and col are within the bounds of the matrix (>=0 and < numRows/numCols)
    //MODIFIES: Nothing
    //EFFECTS: Returns the value of the matrix in the [row,col] position by non-const reference
    T &operator()(uint32_t row, uint32_t col) {
        assert(row >= 0 && row < rows);
        assert(col >= 0 && col < columns);
        return matrix[row][col];
    }
    //REQUIRES: row and col are within the bounds of the matrix (>=0 and < numRows/numCols)
    //MODIFIES: Nothing
    //EFFECTS: Returns the value of the matrix in the [row,col] position by const reference
    const T &operator()(uint32_t row, uint32_t col) const {
        assert(row >= 0 && row < rows);
        assert(col >= 0 && col < columns);
        return matrix[row][col];
    }

    //REQUIRES: matrix and rhs are valid matrices
    //MODIFIES: Nothing
    //EFFECTS: Returns whether the matrices are of the same shape and every element in the RHS matrix
    //         is equal to its corresponding element in the LHS matrix
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

    //REQUIRES: matrix and rhs are valid matrices
    //MODIFIES: Nothing
    //EFFECTS: Returns whether the matrices are either not of the same shape or not every element 
    //         in the RHS matrix is equal to its corresponding element in the LHS matrix
    bool operator!=(const Matrix &rhs) const {
        return !(*this == rhs);
    }

    //REQUIRES: matrix and rhs are valid and of the same shape and type
    //          and the '+' operator is defined for the type
    //MODIFIES: matrix
    //EFFECTS: Adds the RHS matrix to the LHS matrix
    Matrix &operator+(const Matrix &rhs) {
        assert((rows == rhs.rows) && (columns == rhs.columns));
        for(uint32_t row = 0; row < rows; row++) {
            for(uint32_t col = 0; col < columns; col++) {
                matrix[row][col] += rhs(row,col);
            }
        }
        return *this;
    }
    
    //REQUIRES: matrix and rhs are valid and of the same shape and type 
    //          and the '-' operator is defined for the type
    //MODIFIES: matrix
    //EFFECTS: Subtracts the RHS matrix from the LHS matrix
    Matrix &operator-(const Matrix &rhs) {
        assert((rows == rhs.rows) && (columns == rhs.columns));
        for(uint32_t row = 0; row < rows; row++) {
            for(uint32_t col = 0; col < columns; col++) {
                matrix[row][col] -= rhs(row,col);
            }
        }
        return *this;
    }

    //REQUIRES: matrix and rhs are valid matrices, matrix.columns == rhs.rows
    //          rhs and matrix are of the same type and the '*' operator is defined for the type
    //MODIFIES: matrix
    //EFFECTS: Multiplies matrix and RHS
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

    //REQUIRES: matrix is valid, the '*' operator is defined for the type T
    //MODIFIES: matrix
    //EFFECTS: Multiplies each value in matrix by the input coefficient
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
    //REQUIRES: matrix is valid
    //MODIFIES: matrix
    //EFFECTS: deletes matrix
    void deleteMatrix() {
        for(uint32_t row = 0; row < rows; row++) {
            delete[] matrix[row];
        }
        delete[] matrix;
    }

    //REQUIRES: rhs is a valid matrix
    //MODIFIES: matrix
    //EFFECTS: Copies the RHS matrix into the lhs matrix
    void copyVals(const Matrix &rhs) {
        for(uint32_t row = 0; row < rows; row++) {
            for(uint32_t col = 0; col < columns; col++) {
                matrix[row][col] = rhs.matrix[row][col];
            }
        }
    }

    //REQUIRES: first and second are valid matrices
    //MODIFIES: first and second
    //EFFECTS: Swaps the rows, columns determinants and matrices of first and second
    void swap(Matrix<T> &first, Matrix<T> &second) { //custom swap for matrices
        std::swap(first.rows, second.rows);
        std::swap(first.columns, second.columns);
        std::swap(first.determinant, second.determinant);
        T** temp = first.matrix;
        first.matrix = second.matrix;
        second.matrix = temp; //need to let it destruct safely w/o destructing the swapped matrix
    }

};

//REQUIRES: mat is a valid matrix
//MODIFIES: Nothing
//EFFECTS: Prints out the matrix
template<typename T>
std::ostream &operator<<(std::ostream &os, const Matrix<T> &mat) {
    if(mat.getRows() == 0 || mat.getCols() == 0) { //Not sure how to get this w/ current for loop
        os << "[  ]\n";
    }
    else {
        for(uint32_t row = 0; row < mat.getRows(); row++) {
            os << "[  ";
            for(uint32_t col = 0; col < mat.getCols(); col++) {
                os << mat(row,col) << " ";
            }
            os << " ]\n"; //already an extra space after the last element so only one extra
        }
    }

    return os;
}