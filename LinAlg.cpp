#include "LinAlg.h"
#include <getopt.h>

// Process command line arguments
//REQUIRES: Nothing
//MODIFIES: Nothing
//EFFECTS: Prints helpful information regarding the program to cout
void LinearAlgebra::printHelp() {
    cout << "This program will read in a matrix and perform specified operations\n";
    cout << "It will then output requested information about the new matrix\n";
    cout << "The --operations flag will perform specified operations on the input matrices\n";
    cout << "The --information flag will give information about the one input matrix\n";
    cout << "The --help flag will give information about program functionality and command-line flags\n";
}

//REQUIRES: argc, argv are valid
//MODIFIES: 
//EFFECTS:
void LinearAlgebra::getMode(int argc, char * argv[]) {
    // These are used with getopt_long()
    opterr = false; // Let us handle all error output for command line options
    int choice;
    int option_index = 0;
    option long_options[] = {
        {"operations",   no_argument,       nullptr, 'o'  },
        {"information",  no_argument,       nullptr, 'i'  },
        {"help",         no_argument,       nullptr, 'h'  },
        {nullptr,        0,                 nullptr, '\0' }
    };

    string str;
    bool modeSelected = false;
    while ((choice = getopt_long(argc, argv, "oih", long_options, &option_index)) != -1) {
        switch (choice) {
            case 'o':
                infoMode = false;
                modeSelected = true;
                break;
            case 'i':
                infoMode = true;
                modeSelected = true;
                break;
            case 'h':
                printHelp();
                exit(0);
                break;
            default:
                cerr << "Unknown command line option";
                exit(1);
                break;
            } // switch
    } // while

    if(!modeSelected) {
        cerr << "No operations specified";
        exit(1);
    }
}

/* InfoMode */
// numMatrices
// row col
// [matrix 1]

// row col
// [matrix 2]
// .
// .
// -------------------
/* Operation Mode */
// numMatrices
// row col
// [matrix 1]
// (Operator)
// row col
// [matrix 2]
// (Operator)
// .
// .


void LinearAlgebra::getInput() {
    uint32_t num = 8; //TODO: Fix after I know how many matrices I need for the info mode
    uint32_t row;
    uint32_t col;
    cin >> numMatrices;
    if(infoMode) { //info mode
        uint32_t count = 0;
        matrices.resize(numMatrices * num);
        while(cin >> row >> col) {
            matrices[count] = Matrix<double>(row,col,cin);
            for(uint32_t c = count + 1; c < count + 8; c++) {
                matrices[c] = matrices[c - 1];
            }
            count += 8;
        }
    }
    else { //operation mode
        matrices.reserve(numMatrices);
        operators.reserve(numMatrices - 1);
        char op;
        while(cin >> row >> col) {
            matrices.emplace_back(row,col,cin);
            cin >> op;
            if(op == '+' || op == '-' || op == '*') {
                operators.push_back(op);
            }
            else {
                cerr << "Invalid Operator";
                exit(1);
            }
        }
    }
}

Matrix<double> LinearAlgebra::performOperations() {
    for(size_t m = 0; m < matrices.size() - 1; m++) {
        char op = operators[m];
        switch(op) {
            case '+':
                matrices[m + 1] = matrices[m] + matrices[m + 1];
                break;
            case '-':
                matrices[m + 1] = matrices[m] - matrices[m + 1];
                break;
            case '*':
                matrices[m + 1] = matrices[m] * matrices[m + 1];
                break;
        }
    }
    return matrices.back();
}

//REQUIRES: mat is valid, row is the topmost row to be subtracted down, startcol is the first column to start subtracting,
//          endcol is one past the last column to subtract
//MODIFIES: mat
//EFFECTS: Subtracts from the input row down to the bottom of the matrix so that the matrix entries below each pivot are zero
//          i.e. puts the matrix into row echelon form
//          Also only considers pivots between [startCol,endCol) so that the whole row is subtracted, but if the method has not
//          reached the bottom of the matrix, and there is no pivot remaining between [startCol,endCol) it will stop
void LinearAlgebra::subtractDown(Matrix<double> &mat, uint32_t row, uint32_t startCol, uint32_t endCol) {
    pair<int,int> pos = findPivotInMatrix(mat, row, startCol, endCol);
    uint32_t nextRow = 0;
    while(pos.first != -1 && pos.second != -1 && (uint32_t)pos.second < endCol) {
        if(nextRow != (uint32_t)pos.first) {
            interchangeRow(mat, (uint32_t)pos.first, nextRow);
        }
        for(uint32_t r = nextRow + 1; r < mat.rows; r++) {
            subtractRow(mat, nextRow, r);
        }
        pos = findPivotInMatrix(mat, (uint32_t)pos.first + 1, (uint32_t)pos.second + 1, endCol);
        nextRow++;
    }
}

//REQUIRES: mat is a valid matrix, mat is in Row Echelon Form, 
//          startcol is the first column to start subtracting, endcol is one past the last column to subtract
//MODIFIES: mat
//EFFECTS: Turns a matrix in Row Echelon Form (REF) to Reduced Row Echelon Form (RREF)
void LinearAlgebra::subtractUp(Matrix<double> &mat, uint32_t startCol, uint32_t endCol) {
    int pos;
    for(uint32_t r = mat.rows - 1; r < mat.rows; r--) { //rolls over after hits zero
        pos = findPivotInRow(mat, r, startCol, endCol);
        if(pos != -1) { //nonzero row
            divideRow(mat, r);
            for(uint32_t rowsAbove = r - 1; rowsAbove < mat.rows; rowsAbove--) {
                subtractRow(mat, r, rowsAbove);
            }
        }
    }
}

//REQUIRES: mat is a valid matrix, mat is square
//MODIFIES: mat
//EFFECTS: Finds the inverse of mat and replaces mat with its inverse
void LinearAlgebra::inverse(Matrix<double> &mat) {
    Matrix<double> newMat(mat.rows, mat.columns * 2);
    for(uint32_t r = 0; r < mat.rows; r++) {
        for(uint32_t c = 0; c < mat.rows; c++) {
            newMat(r,c) = mat(r,c);
        }
    }
    for(uint32_t diag = 0; diag < mat.rows; diag++) {
        newMat(diag,mat.columns + diag) = 1;
    }
    subtractDown(newMat,0,0,mat.columns);
    subtractUp(newMat,0,mat.columns);

    for(uint32_t r = 0; r < mat.rows; r++) {
        for(uint32_t c = 0; c < mat.rows; c++) {
            mat(r,c) = newMat(r, c + mat.columns);
        }
    }
}

//REQUIRES: mat is a valid matrix, row is a valid row within mat
//MODIFIES: mat, determinant
//EFFECTS: Divides the corresponding row by its pivot so that its pivot is 1
//          Determinant is multiplied by the coefficient dividing the row
void LinearAlgebra::divideRow(Matrix<double> &mat, uint32_t row) {
    int piv = findPivotInRow(mat, row, 0, mat.columns);
    if(piv != -1) { //nonzero row
        double coef = mat(row, (uint32_t)piv);
        for(uint32_t e = (uint32_t)piv; e < mat.columns; e++) {
            mat(row, e) /= coef;
        }
        mat.determinant *= coef; //dividing multiplies the determinant by the coefficient
    }
}

//REQUIRES: mat is a valid matrix, row1 and row2 are valid rows within mat
//MODIFIES: mat
//EFFECTS: Switches the positions of row1 and row2 in mat
//          Determinant is multiplied by -1
void LinearAlgebra::interchangeRow(Matrix<double> &mat, uint32_t row1, uint32_t row2) {
    swap(mat.matrix[row1], mat.matrix[row2]); //swap the rows
    mat.determinant *= -1; //interchanging multiplies the determinant by -1
}

//REQUIRES: mat is a valid matrix
//MODIFIES: mat
//EFFECTS: Finds the transpose of mat and replaces mat with it
void LinearAlgebra::transpose(Matrix<double> &mat) {
    Matrix<double> newMat(mat.columns, mat.rows);
    for(uint32_t r = 0; r < mat.rows; r++) {
        for(uint32_t c = 0; c < mat.columns; c++) {
            newMat(c,r) = mat(r,c);
        }
    }
    mat = newMat; //Can use the steal constructor here, newMat is an rValue
}

//REQUIRES: mat is a valid matrix, toSubtract and subtractFrom are valid rows within mat
//MODIFIES: mat
//EFFECTS: Subtracts the row at the toSubtract position from the row at the subtractFrom position
//         The coefficient the toSubtract row is multiplied by is the value in the subtractFrom row in the 
//         toSubtract pivot position column divided by the value of the toSubtract pivot position
//         i.e. it subtracts the toSubtract row from the subtractFrom row so that the    E.x [1,1,1,1] -> [1,1,1,1]
//         value in the subtractFrom row below the toSubtract pivot is zero                  [2,3,4,5]    [0,1,2,3]     
void LinearAlgebra::subtractRow(Matrix<double> &mat, uint32_t toSubtract, uint32_t subtractFrom) {
    int piv = findPivotInRow(mat, toSubtract, 0, mat.columns);
    if(piv != -1) { //nonzero row
        double coef = mat(subtractFrom, (uint32_t)piv) / mat(toSubtract, (uint32_t)piv);
        for(uint32_t e = (uint32_t)piv; e < mat.columns; e++) {
            mat(subtractFrom, e) -= coef * mat(toSubtract, e);
        }
    }
}

//REQUIRES: mat is a valid matrix
//MODIFIES: mat
//EFFECTS: Finds a basis for the Column Space of mat, and replaces mat with that basis
void LinearAlgebra::findColSpace(Matrix<double> &mat) {
    vector<bool> independentCols = getIndepCols(mat);

    uint32_t numIndepCols = 0;
    for(uint32_t c = 0; c < independentCols.size(); c++) {
        if(independentCols[c]) {
            numIndepCols++;
        }
    }

    Matrix<double> newMat(mat.rows, numIndepCols);
    uint32_t newMatCol = 0;
    for(uint32_t c = 0; c < mat.columns; c++) {
        if(independentCols[c]) {
            for(uint32_t r = 0; r < mat.rows; r++) {
                newMat(r,newMatCol) = mat(r,newMatCol);
            }
            newMatCol++;
        }
    }

    mat = newMat;
}

//REQUIRES: mat is a valid matrix
//MODIFIES: mat
//EFFECTS: Finds a basis for the Null Space of mat, and replaces mat with that basis
void LinearAlgebra::findNullSpace(Matrix<double> &mat) {
    vector<bool> independentCols = getIndepCols(mat);

    uint32_t numNullCols = 0;
    for(uint32_t c = 0; c < independentCols.size(); c++) {
        if(!independentCols[c]) {
            numNullCols++;
        }
    }

    Matrix<double> newMat(mat.rows, numNullCols);
    uint32_t newMatCol = 0;
    for(uint32_t c = 0; c < mat.columns; c++) {
        if(!independentCols[c]) {
            for(uint32_t r = 0; r < mat.rows; r++) {
                newMat(r,newMatCol) = mat(r,newMatCol);
            }
            newMatCol++;
        }
    }

    mat = newMat;
}

//REQUIRES: mat is a valid matrix
//MODIFIES: mat
//EFFECTS: Finds a basis for the Row Space of mat, and replaces mat with that basis
void LinearAlgebra::findRowSpace(Matrix<double> &mat) {
    transpose(mat);
    findColSpace(mat);
    transpose(mat);
}

//REQUIRES: mat is a valid matrix in Row Echelon Form or Reduced Row Echelon Form
//MODIFIES: determinant
//EFFECTS: Calculates the determinant of the mat
void LinearAlgebra::calcDeterminant(Matrix<double> &mat) { //must be a square matrix
    for(uint32_t r = 0; r < mat.rows; r++) {
        mat.determinant *= mat(r,r);
    }
}

//REQUIRES: For each matrix in the input, it has been copied 7 additional times consecutively in the matrices vector
//          E.x. [m1, m1, m1, m1, m1, m1, m1, m1, m2, m2, m2, m2, m2, m2, m2, m2, ...]
//MODIFIES: Nothing
//EFFECTS: Calculates the REF, RREF, Transpose, Inverse, and Row/Column/Null Space in the order
//          [OriginalMatrix]   [REF]   [RREF]   [Transpose]  [Inverse]   [RowSpace]  [ColumnSpace]  [NullSpace]
void LinearAlgebra::getInformation() {
    for(uint32_t m = 0; m < numMatrices; m++) {
        subtractDown(matrices[1 + m * 8], 0, 0, matrices[1 + m * 8].columns); //REF

        matrices[2 + m * 8] = matrices[1 + m * 8]; //Copy REF to RREF matrix 
        subtractUp(matrices[2 + m * 8], 0, matrices[2 + m * 8].columns); //RREF

        transpose(matrices[3 + m * 8]); //Transpose

        if(matrices[4 + m * 8].rows == matrices[4 + m * 8].columns) { //square matrix;
            inverse(matrices[4 + m * 8]); //Inverse
        }

        findColSpace(matrices[6 + m * 8]); //Column Space
        findNullSpace(matrices[7 + m * 8]); //Null Space
        findRowSpace(matrices[5 + m * 8]); //Row Space
    }
}

//REQUIRES: The REF, RREF, Transpose, Inverse, and Row/Column/Null Spaces have all been calculated for each input matrix
//MODIFIES: Nothing
//EFFECTS: Prints out the REF, RREF, Transpose, Inverse, and Row/Column/Null Spaces for each input matrix
void LinearAlgebra::printInformation() {
    for(uint32_t m = 0; m < numMatrices; m++) {
        cout << "Original Matrix:\n" << matrices[m * 8] << "\n\n";
        cout << "Reduced Echelon Form:\n" << matrices[1 + m * 8] << "\n\n";
        cout << "Reduced Row Echelon Form:\n" << matrices[2 + m * 8] << "\n\n";
        cout << "Transpose:\n" << matrices[3 + m * 8] << "\n\n";
        if(matrices[4 + m * 8].rows == matrices[4 + m * 8].columns) { //square matrix;
            cout << "Inverse:\n" << matrices[4 + m * 8] << "\n\n";
        }

        cout << "Column Space:\n";
        printColumns(matrices[6 + m * 8]);
        cout << "Null Space:\n";
        printColumns(matrices[7 + m * 8]);
        cout << "Row Space:\n";
        printRows(matrices[5 + m * 8]);
    }
}

/* ---------------------- ACCESSORS ---------------------- */
Matrix<double>& LinearAlgebra::getREF(uint32_t numInputMat) {
    return matrices[numInputMat * 8 + 1];
}

Matrix<double>& LinearAlgebra::getRREF(uint32_t numInputMat) {
    return matrices[numInputMat * 8 + 2];
}

Matrix<double>& LinearAlgebra::getTranspose(uint32_t numInputMat) {
    return matrices[numInputMat * 8 + 3];
}

Matrix<double>& LinearAlgebra::getInverse(uint32_t numInputMat) {
    return matrices[numInputMat * 8 + 4];
}

Matrix<double>& LinearAlgebra::getRowSpace(uint32_t numInputMat) {
    return matrices[numInputMat * 8 + 5];
}

Matrix<double>& LinearAlgebra::getColSpace(uint32_t numInputMat) {
    return matrices[numInputMat * 8 + 6];
}

Matrix<double>& LinearAlgebra::getNullSpace(uint32_t numInputMat) {
    return matrices[numInputMat * 8 + 7];
}

double LinearAlgebra::getDeterminant(Matrix<double> &mat) {
    return mat.determinant;
}

/* ---------------------- HELPERS ---------------------- */

//REQUIRES: mat is a valid matrix, row is a valid row in the matrix
//MODIFIES: Nothing
//EFFECTS: Finds the column of the first non-zero element in the specified row in the range of columns [startCol,endCol)
//         If no such pivot is found (the row is a zero row), -1 is returned
int LinearAlgebra::findPivotInRow(Matrix<double> &mat, uint32_t row, uint32_t startCol, uint32_t endCol) {
    for(uint32_t e = startCol; e < endCol; e++) {
        if(mat(row, e) != 0) {
            return (int)e;
        }
    }
    return -1;
}

//REQUIRES: mat is a valid matrix,
//MODIFIES: Nothing
//EFFECTS: Finds the first non-zero (pivot) element in the matrix starting from startRow and in the range of columns [startCol,endCol)
pair<int,int> LinearAlgebra::findPivotInMatrix(Matrix<double> &mat, uint32_t startRow, uint32_t startCol, uint32_t endCol) {
    for(uint32_t c = startCol; c < endCol; c++) {
        for(uint32_t r = startRow; r < mat.rows; r++) {
            if(mat(r,c) != 0) {
                return make_pair(r,c);
            }
        }
    }
    return make_pair(-1, -1);
}

//REQUIRES: mat is a valid matrix
//MODIFIES: Nothing
//EFFECTS: Turns a copy of the input matrix into its corresponding RREF
//         Then returns a vector of bools for whether or not a pivot appears in a specific column in the matrix
//         Note: Not having a reference is intentional, a copy is needed so that the original matrix is untouched
vector<bool> LinearAlgebra::getIndepCols(Matrix<double> mat) { //I should return this by reference but then its a reference to a destructed object
    subtractDown(mat, 0, 0, mat.columns);                       //maybe stealing means its not actually copied?
    subtractUp(mat, 0, mat.columns);
    vector<bool> independentCols(mat.columns, false);
    uint32_t nextRow = 0;
    for(uint32_t c = 0; c < mat.columns; c++) {
        for(uint32_t r = nextRow; r < mat.rows; r++) {
            if(mat(r, c) != 0) {
                independentCols[c] = true;
                nextRow = r + 1;
                break;
            }
        }
    }

    return independentCols;
}

//REQUIRES: mat is a valid matrix
//MODIFIES: Nothing
//EFFECTS: Prints out the columns of a matrix individually
void LinearAlgebra::printColumns(Matrix<double> const &mat) {
    if(mat.getRows() == 0 || mat.getCols() == 0) { //Empty Matrix
        cout << "[  ]\n\n";
    }
    else { //Non-Empty Matrix
        for(uint32_t r = 0; r < mat.getRows() - 1; r++) {
            for(uint32_t c = 0; c < mat.getCols(); c++) {
                cout << "[ " << mat(r,c) << " ]   ";
            }
            cout << "\n";
        }
        for(uint32_t c = 0; c < mat.getCols(); c++) {
            cout << "[ " << mat(mat.getRows() - 1,c) << " ],  ";
        }
        cout << "\n\n";
    }
}

//REQUIRES: mat is a valid matrix
//MODIFIES: Nothing
//EFFECTS: Prints out the rows of a matrix individually
void LinearAlgebra::printRows(Matrix<double> const &mat) {
    if(mat.getRows() == 0 || mat.getCols() == 0) { //Empty Matrix
        cout << "[ ";
    }
    else { //Non-Empty Matrix
        for(uint32_t r = 0; r < mat.getRows() - 1; r++) {
            cout << "[  ";
            for(uint32_t c = 0; c < mat.getCols(); c++) {
                cout << mat(r,c) << " ";
            }
            cout << " ],\n";
        }
        cout << "[  ";
        for(uint32_t c = 0; c < mat.getCols(); c++) {
            cout << mat(mat.getRows() - 1,c) << " ";
        }
    }
    cout << " ]\n\n";
}