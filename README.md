# LinearAlgebra

This project will do simple linear algebra on input matrices.

The input file should be formatted as follows: \
[Number of Matrices in File]

[Rows] [Columns] \
[Matrix 1] \
[Command] 

[Rows] [Columns] \
[Matrix 2] \
[Command] \
etc. 

Example: \
1 \
2 4 \
1 2 3 4 \
5 6 7 7 \
All 

Command is one of: All, REF, RREF, Inverse, Transpose, RowSpace, ColumnSpace, NullSpace, Solve or an Operand (+,-,*) \
All --- Outputs all available information for the matrix (REF, RREF, Inverse if applicable, Transpose, RowSpace, ColumnSpace, NullSpace) \
REF, RREF, Inverse, Transpose, RowSpace, ColumnSpace, and NullSpace --- Outputs the specified form of the matrix \
Solve --- Treats the matrix as a system of equations to be solved, and output the final values for each of the variables in the system \
Operand --- Performs the operation on the input matrix and the next matrix in the input file (e.x [Matrix1]+ will add Matrix1 to Matrix2) 

Note: As of now no errors are thrown if input files are incorrect so be careful when adding/subtracting/multiplying matrices that matrix dimensions are correct and that the matrix is square if the inverse is asked for.

Command Line Options: -p/-precision [num] allows the user to set the number of output decimal places, default 2
