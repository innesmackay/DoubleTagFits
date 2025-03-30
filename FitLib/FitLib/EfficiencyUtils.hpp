#ifndef EFFICIENCYUTILS_H
#define EFFICIENCYUTILS_H

#include "Settings.hpp"

#include "TMatrixD.h"
#include "TVectorD.h"
#include <map>

namespace EfficiencyUtils {

    /** Directory with efficiency matrices */
    const std::string input_dir = "/home/mackay/bes3_deltaKPi/inputs/efficiencies/";
    const int nbins = 16;
    const int nprods = 5;

    /** Function to load the KSpipi migration matrix */
    TMatrixD GetMigrationMatrix();
    inline TMatrixD GetInverseMigrationMatrix(){ return GetMigrationMatrix().Invert(); }

    /**
     * Function to perform the KSpipi bin migration
     * @param Ni yields in DP bins
    */
    std::map<int, double> ReverseMigration(std::map<int, double> Ni);
    std::map<int, double> Migration(std::map<int, double> Ni);

    /**
     * Function to load efficiency matrices
     * @param bin index of KSpipi DP region
     * @param tag name of CP tag
    */
    TMatrixD GetUnfoldingMatrix(int bin, Settings s);
    TMatrixD GetUnfoldingMatrix(std::string tag, Settings s);
    inline TMatrixD GetInverseUnfoldingMatrix(int bin, Settings s){ return GetUnfoldingMatrix(abs(bin), s).Invert(); }
    inline TMatrixD GetInverseUnfoldingMatrix(std::string tag, Settings s){ return GetUnfoldingMatrix(tag, s).Invert(); }

    /** 
     * Return row of efficiency matrix
     * @param tag name of CP tag
     * @param row_index index of row to return
    */
    inline TVectorD GetRowOfUnfoldingMatrix(std::string tag, int row_index, Settings s){
        TVectorD row(EfficiencyUtils::nprods);
        TMatrixD m = GetUnfoldingMatrix(tag, s);
        row = TMatrixDRow(m, row_index);
        return row;
    }

    /**
     * Perform efficiency calculation
     * @param yields_by_prod vector of yields
     * @param bin index of KSpipi DP region
     * @param tag name of CP tag
    */
    TVectorD FoldYields(TVectorD yields_by_prod, int bin, Settings s);
    TVectorD UnfoldYields(TVectorD yields_by_prod, int bin, Settings s);
    TVectorD FoldYields(TVectorD yields_by_prod, std::string tag, Settings s);
    TVectorD UnfoldYields(TVectorD yields_by_prod, std::string tag, Settings s);

}

#endif //  Efficiencies_H
