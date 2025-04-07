#include "Variables.hpp"
#include "EfficiencyUtils.hpp"
#include "MatrixMaths.hpp"


void Variables::FloatingYi(){
    for (auto prod: Definitions::PRODS){            
        unsigned int bin_counter=0;
        for (auto bin: Definitions::DP_BINS){
            Yi[prod][bin] = new RooRealVar(("Y_" + prod + "_" + std::to_string(bin)).c_str(), "", Inputs::Ki[-1*bin], 0, 2*Inputs::Ki[-1*bin]+0.05);
        }
    }
    return;
}


void Variables::DefaultYi(){
    
    // Create unnormalised Yi
    std::map<std::string, std::map<int, RooAbsArg*>> unnormalised_Yi;
    std::map<std::string, TString> sum_Yi_formula;
    std::map<std::string, RooArgSet> Yi_args;
    for (auto C: {-1, 1}){
        int bin_counter=0;
        for (auto bin: Definitions::DP_BINS){
            if (C == -1) unnormalised_Yi[std::to_string(C)][bin] = new RooFormulaVar(("Y_" + std::to_string(bin) + "_C_" + std::to_string(C)).c_str(), "", "(@0 + @1*(@2*@2 + @3*@3) + 2*@8*sqrt(@0*@1)*(@4*@2 + @5*@3)) - (1+@8)*@6*((1+(@2*@2 + @3*@3))*@4*sqrt(@0*@1) + @2*(@0 + @1)) - (1+@8)*@7*((1-(@2*@2 + @3*@3))*@5*sqrt(@0*@1) + @3*(@1 - @0))", RooArgSet( *Ki_vars[-1*bin], *Ki_vars[bin], *rCosDelta, *rSinDelta, *ci_vars[bin], *si_vars[bin], *yD, *xD, *C_vars["ODD"] ));
            else unnormalised_Yi[std::to_string(C)][bin] = new RooFormulaVar(("Y_" + std::to_string(bin) + "_C_" + std::to_string(C)).c_str(), "", "(@0 + @1*(@2*@2 + @3*@3) + 2*@8*sqrt(@0*@1)*(@4*@2 + @5*@3)) - (1+@8)*@6*((1+(@2*@2 + @3*@3))*@4*sqrt(@0*@1) + @2*(@0 + @1)) - (1+@8)*@7*((1-(@2*@2 + @3*@3))*@5*sqrt(@0*@1) + @3*(@1 - @0))", RooArgSet( *Ki_vars[-1*bin], *Ki_vars[bin], *rCosDelta, *rSinDelta, *ci_vars[bin], *si_vars[bin], *yD, *xD, *C_vars["EVEN"] ));
            sum_Yi_formula[std::to_string(C)] += Form("+@%i", bin_counter);
            Yi_args[std::to_string(C)].add(*unnormalised_Yi[std::to_string(C)][bin]);
            bin_counter += 1;
        }
    }

    // Normalise
    std::map<std::string, RooFormulaVar*> sum_Yi;
    for (auto key : unnormalised_Yi){        
        sum_Yi[key.first] = new RooFormulaVar(("sum_Yi_" + key.first).c_str(), "", sum_Yi_formula[key.first], Yi_args[key.first]);
        for (auto bin : Definitions::DP_BINS){
            TString tmp_name = unnormalised_Yi[key.first][bin]->GetName();
            Yi[key.first][bin] = new RooFormulaVar("norm_" + tmp_name, "", "@0/@1", RooArgSet(*unnormalised_Yi[key.first][bin], *sum_Yi[key.first]));
        }
    }

    return;

}


void Variables::FloatingByC(){
    for (auto C: {-1, 1}){
        for (auto bin: Definitions::DP_BINS){
            Yi[std::to_string(C)][bin] = new RooRealVar(("Y_" + std::to_string(bin) + "_C_" + std::to_string(C)).c_str(), "", Inputs::Ki[-1*bin], 0, 2*Inputs::Ki[-1*bin]+0.05);
        }
    }
    return;
}


std::map<std::string, std::map<int, RooFormulaVar*>> Variables::TotalNi(){
    std::map<std::string, std::map<int, RooFormulaVar*>> total_Ni;
    for (auto prod: Definitions::C_VALS){
        for (auto bin: Definitions::DP_BINS){
            TString tmp_name = ("N_" + prod.first + "_bin_" + std::to_string(bin)).c_str();
            if (m_settings.get("Yi_strategy") == "float"){
                total_Ni[prod.first][bin] = new RooFormulaVar(tmp_name, "", "@0*@1", RooArgSet(*Yi[prod.first][bin], *integrated_N[prod.first]));
            }
            else{
                total_Ni[prod.first][bin] = new RooFormulaVar(tmp_name, "", "@0*@1", RooArgSet(*Yi[std::to_string(prod.second)][bin], *integrated_N[prod.first]));
            }
        }
    }
    return total_Ni;
}


void Variables::InitialiseNi(){

    // Efficiency corrected Ni
    auto total_Ni = TotalNi();

    // Migrate the Ni
    auto migration = EfficiencyUtils::GetMigrationMatrix(); // TMatrix
    auto cpp_migration = MatrixMaths::ConvertTMatrix(migration, "migration"); // vector of vectors
    std::map<TString, std::map<int, RooFormulaVar*>> migrated_Ni;
    for (auto prod: Definitions::PRODS){
        // Migrate the Ni
        // Returns map of RooFormulaVar
        migrated_Ni[prod] = MatrixMaths::MatrixMultiplication( total_Ni[prod], cpp_migration, "Migrated_" + prod );
    }

    // Perform the efficiency correction
    for (auto bin: Definitions::DP_BINS){
        std::vector<RooFormulaVar*> tmp_yield_by_prod;
        for (auto prod: Definitions::PRODS){
            tmp_yield_by_prod.push_back( migrated_Ni[prod][bin] ); // Convert to vectors
        }
        auto tmp_unfolding = EfficiencyUtils::GetUnfoldingMatrix(abs(bin), m_settings); // Get unfolding matrix for bin
        auto cpp_tmp_unfolding = MatrixMaths::ConvertTMatrix(tmp_unfolding, "unfolding_bin" + std::to_string(bin)); // Vector of vectors
        auto tmp_unfolded_yield_in_bin = MatrixMaths::MatrixMultiplication( tmp_yield_by_prod, cpp_tmp_unfolding, "unfolded_bin_" + std::to_string(bin) ); // Do unfolding
        for (auto prod: Definitions::PRODS){
            Ni[prod][bin] = tmp_unfolded_yield_in_bin[prod]; // Get efficiency corrected Ni
        }
    }
    return;
}
