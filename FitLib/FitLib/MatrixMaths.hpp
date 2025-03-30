#ifndef MATRIXMATHS_H
#define MATRIXMATHS_H

#include "RooRealVar.h"
#include "RooFormulaVar.h"
#include "TMatrixD.h"

#include <map>
#include <vector>

namespace MatrixMaths {
    /**
     * Convert map of RooFormulaVar with key given by DP bin into a vector
     * @param m map of RooFormulaVar by DP region
    */
    std::vector<RooFormulaVar*> ConvertMapToVector(std::map<int, RooFormulaVar*> m);
    /**
     * Convert vector of RooFormulaVar into a map with key given by DP bin
     * @param v vector of RooFormulaVar
    */
    std::map<int, RooFormulaVar*> ConvertVectorToMap(std::vector<RooFormulaVar*> v);
    /**
     * Convert vector of RooFormulaVar's into a map with key given by production mechanism
     * @param v vector of RooFormulaVar
    */
    std::map<std::string, RooFormulaVar*> ConvertProdVectorToMap(std::vector<RooFormulaVar*> v);
    /**
     * Convert a TMatrix into a vector of vectors for calculations
     * @param mat TMatrix to convert
     * @param mat_name string name of output matrix
    */
    std::vector<std::vector<RooRealVar*>> ConvertTMatrix(TMatrixD mat, std::string mat_name);
    /**
     * Perform matrix multiplication on a row of RooFormulaVar's given by the DP bin
     * @param map_row map containing RooFormulaVar
     * @param matrix matrix as a vector of vectors
     * @param new_name string name of the new matrix
    */
    std::map<int, RooFormulaVar*> MatrixMultiplication(std::map<int, RooFormulaVar*> map_row, std::vector<std::vector<RooRealVar*>> matrix, std::string new_name);
    /**
     * Perform matrix multiplication on a row of RooFormulaVar's given by the production mechanism
     * @param map_row map containing RooFormulaVar
     * @param matrix matrix as a vector of vectors
     * @param new_name string name of the new matrix
    */
    std::map<std::string, RooFormulaVar*> MatrixMultiplication(std::vector<RooFormulaVar*> row, std::vector<std::vector<RooRealVar*>> matrix, std::string new_name);

}

#endif //  MatrixMaths_H
