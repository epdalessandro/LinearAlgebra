#include "xcode_redirect.hpp"
#include <iostream>
#include "LinAlg.h"
using namespace std;

int main(int argc, char* argv[]) {
    ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv); //file redirection

    LinearAlgebra linal;
    linal.getMode(argc, argv);
    linal.getInput();
    linal.getInformation();
    linal.printInformation();
}