#include "MatrixMaths.hpp"
#include "Definitions.hpp"



namespace MatrixMaths{
    std::vector<RooFormulaVar*> ConvertMapToVector(std::map<int, RooFormulaVar*> m){
        std::vector<RooFormulaVar*> newvec;
        for (auto v : m) newvec.push_back( v.second );
        return newvec;
    }


    std::map<int, RooFormulaVar*> ConvertVectorToMap(std::vector<RooFormulaVar*> v){
        std::map<int, RooFormulaVar*> m;
        unsigned int i=0;
        for (auto bin: Definitions::DP_BINS){ m[bin] = v.at(i); i++; }
        return m;
    }


    std::map<std::string, RooFormulaVar*> ConvertProdVectorToMap(std::vector<RooFormulaVar*> v){
        std::map<std::string, RooFormulaVar*> m;
        unsigned int i=0;
        for (auto prod: Definitions::PRODS){ m[prod] = v.at(i); i++; }
        return m;
    }


    std::vector<std::vector<RooRealVar*>> ConvertTMatrix(TMatrixD mat, std::string mat_name){
        int nrows = mat.GetNrows();
        int ncols = mat.GetNcols();
        std::vector<std::vector<RooRealVar*>> cpp_matrix;
        cpp_matrix.resize(nrows, std::vector<RooRealVar*>(ncols, 0));
        for (auto i=0; i<mat.GetNrows(); i++ ){
            for (auto j=0; j<mat.GetNcols(); j++){
                TString tmp_name = (mat_name + "_" + std::to_string(i) + "_" + std::to_string(j)).c_str();
                RooRealVar* tmp_element = new RooRealVar(tmp_name, tmp_name, mat(i,j));
                tmp_element->setConstant(true);
                cpp_matrix[i][j] = tmp_element;
            }
        }
        return cpp_matrix;
    }


    std::map<int, RooFormulaVar*> MatrixMultiplication(std::map<int, RooFormulaVar*> map_row, std::vector<std::vector<RooRealVar*>> matrix, std::string new_name){
        std::vector<RooFormulaVar*> new_row;
        std::vector<RooFormulaVar*> row = ConvertMapToVector(map_row);
        for (int k=0; k<row.size(); k++){
            int npars = 0;
            TString rowFormula = "";
            RooArgSet varsToUse;
            for (int l=0; l<matrix[0].size(); l++){
                std::string tmp_formula;
                if (l!=0){ tmp_formula = " + "; }
                tmp_formula += "@" + std::to_string(npars) + "*@" + std::to_string(npars+1);
                rowFormula += tmp_formula.c_str();
                npars += 2;
                varsToUse.add(*row.at(l));
                varsToUse.add(*matrix[k][l]);
            }
            RooFormulaVar* tmp_migrated_yield = new RooFormulaVar((new_name + "_" + std::to_string(Definitions::DP_INDEX[k])).c_str(), rowFormula, varsToUse);
            new_row.push_back( tmp_migrated_yield );
        }
        return ConvertVectorToMap(new_row);
    }


    std::map<std::string, RooFormulaVar*> MatrixMultiplication(std::vector<RooFormulaVar*> row, std::vector<std::vector<RooRealVar*>> matrix, std::string new_name){
        std::vector<RooFormulaVar*> new_row;
        for (int k=0; k<row.size(); k++){
            int npars = 0;
            TString rowFormula = "";
            RooArgSet varsToUse;
            for (int l=0; l<matrix[0].size(); l++){
                std::string tmp_formula;
                if (l!=0){ tmp_formula = " + "; }
                tmp_formula += "@" + std::to_string(npars) + "*@" + std::to_string(npars+1);
                rowFormula += tmp_formula.c_str();
                npars += 2;
                varsToUse.add(*row.at(l));
                varsToUse.add(*matrix[k][l]);
            }
            RooFormulaVar* tmp_migrated_yield = new RooFormulaVar((new_name + "_" + Definitions::PROD_INDEX[k]).c_str(), rowFormula, varsToUse);
            new_row.push_back( tmp_migrated_yield );
        }
        return ConvertProdVectorToMap(new_row);
    }
}