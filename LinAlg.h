#include "Matrix.h"
#include <vector>
#include <utility>
using namespace std;


class LinearAlgebra {
public:
    void printHelp(); //DONE
    void getMode(int argc, char* argv[]); //DONE
    void getInput(); //DONE
    void subtractDown(Matrix<double> &mat, uint32_t row, uint32_t startCol, uint32_t endCol); //DONE
    void subtractUp(Matrix<double> &mat, uint32_t startCol, uint32_t endCol); //DONE
    void divideRow(Matrix<double> &mat, uint32_t row); //DONE
    void interchangeRow(Matrix<double> &mat, uint32_t row1, uint32_t row2); //DONE
    void transpose(Matrix<double> &mat); //DONE
    void inverse(Matrix<double> &mat); //DONE
    void subtractRow(Matrix<double> &mat, uint32_t toSubtract, uint32_t subtractFrom); //DONE
    int findPivotInRow(Matrix<double> &mat, uint32_t row, uint32_t startCol, uint32_t endCol); //DONE
    pair<int,int> findPivotInMatrix(Matrix<double> &mat, uint32_t startRow, uint32_t startCol, uint32_t endCol); //DONE
    vector<bool> getIndepCols(Matrix<double> &mat); 
    void calcDeterminant();
    void getInformation();
    void printInformation();

    double getDeterminant(Matrix<double> &mat); //DONE
    void findRowSpace(Matrix<double> &mat); //DONE
    void findColSpace(Matrix<double> &mat); //DONE
    void findNullSpace(Matrix<double> &mat); //DONE

    Matrix<double>& getREF(uint32_t numInputMat); //DONE
    Matrix<double>& getRREF(uint32_t numInputMat); //DONE
    Matrix<double>& getInverse(uint32_t numInputMat); //DONE
    Matrix<double>& getTranspose(uint32_t numInputMat); //DONE
    Matrix<double>& getRowSpace(uint32_t numInputMat); //DONE
    Matrix<double>& getColSpace(uint32_t numInputMat); //DONE
    Matrix<double>& getNullSpace(uint32_t numInputMat); //DONE


    Matrix<double> performOperations(); //DONE

private:
    vector<Matrix<double>> matrices;
    vector<char> operators;
    bool infoMode;
    uint32_t numMatrices;
};