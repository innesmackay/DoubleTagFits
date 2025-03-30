#include "EfficiencyUtils.hpp"
#include "TFile.h"
#include "TH2F.h"

#include <fstream>
#include <string>
#include <sstream>

namespace EfficiencyUtils {
    
    TMatrixD GetMigrationMatrix(){
        TFile file("/home/mackay/bes3_deltaKPi/misc/migration/new_single_migration_matrices.root");
        TH2F* migration = (TH2F*) file.Get("migMatrix");
        TMatrixD m(EfficiencyUtils::nbins, EfficiencyUtils::nbins);
        for (int i=0; i<EfficiencyUtils::nbins; i++){
            for (int j=0; j<EfficiencyUtils::nbins; j++){
                if(i != EfficiencyUtils::nbins) m(j,i) = migration->GetBinContent(i+1, j+1);
            }
        }
    return m;
    }
    

    std::map<int, double> ReverseMigration(std::map<int, double> Ni){

        // Conver map to TVector
        TVectorD input_yields(EfficiencyUtils::nbins);
        unsigned int i = 0;
        for (auto key: Ni) input_yields(i) = key.second; i++;

        // Migrate
        input_yields *= GetMigrationMatrix();

        // Convert back to map
        i = 0;
        std::map<int, double> migrated_Ni;
        for (auto key: Ni) migrated_Ni.insert( {key.first, input_yields(i)} ); i++;

        return migrated_Ni;

    }
    

    std::map<int, double> Migration(std::map<int, double> Ni){

        // Convert map to TVector
        TVectorD input_yields(EfficiencyUtils::nbins);
        unsigned int i = 0;
        for (auto key: Ni) input_yields(i) = key.second; i++;

        // Migrate
        input_yields = GetMigrationMatrix().Invert() * input_yields;
        
        // Convert TVector to map
        i = 0;
        std::map<int, double> migrated_Ni;
        for (auto key: Ni) migrated_Ni.insert( {key.first, input_yields(i)} ); i++;
        
        return migrated_Ni;
    }
    

    TMatrixD GetUnfoldingMatrix(int bin, Settings s){

        // Get folder containing effs
        std::string folder;
        if (s.key_exists("efficiency_folder")) folder = s.get("efficiency_folder");
        else folder = EfficiencyUtils::input_dir + "default/";

        // Parse input filename
        std::string init_filename = folder + "KSPiPi_vs_KPi";
        if (s.getB("toy") & s.key_exists("seed")){
            init_filename += "_" + std::to_string(s.getI("seed")) + "FTbin" + std::to_string(abs(bin));
        }        
        std::string file_ender = "FTbin" + std::to_string(abs(bin));
        if (s.getB("unfolding_systematic")) file_ender += s.get("systematic_file_ender");        
        std::string filename = init_filename + file_ender + ".txt";

        // Load matrix        
        std::ifstream input_file(filename);
        std::string line, value;
        TMatrixD unfolding_matrix(EfficiencyUtils::nprods, EfficiencyUtils::nprods);
        int row_counter = 0;
        while(getline(input_file, line)){
            std::stringstream ss(line);
            int col_counter = 0;
            while(getline(ss, value, ' ')){
                unfolding_matrix(row_counter, col_counter) = std::stod(value) / 100;
                col_counter += 1;
            }
            row_counter += 1;
        }
        input_file.close();

        return unfolding_matrix;
    }
    
    
    TMatrixD GetUnfoldingMatrix(std::string tag, Settings s){
        
        // Get folder name
        std::string folder;
        if (s.key_exists("efficiency_folder")) folder = EfficiencyUtils::input_dir + s.get("efficiency_folder") + "/";
        else folder = EfficiencyUtils::input_dir + "default/";

        // Parse filename    
        std::string init_filename = folder + tag + "_vs_KPi";
        std::string file_ender = "";
        if (s.getB("unfolding_systematic")) file_ender += s.get("systematic_file_ender");
        std::string filename = init_filename + file_ender + ".txt";

        // Load matrix        
        std::ifstream input_file(filename);
        std::string line, value;
        TMatrixD unfolding_matrix(EfficiencyUtils::nprods, EfficiencyUtils::nprods);
        int row_counter = 0;
        while(getline(input_file, line)){
            std::stringstream ss(line);
            int col_counter = 0;
            while(getline(ss, value, ' ')){
                unfolding_matrix(row_counter, col_counter) = std::stod(value) / 100;
                col_counter += 1;
            }
            row_counter += 1;
        }
        input_file.close();

        return unfolding_matrix;
    }
    
    
    TVectorD FoldYields(TVectorD yields_by_prod, int bin, Settings s){
        TMatrixD unfolding_matrix = GetUnfoldingMatrix(abs(bin), s);
        yields_by_prod *= unfolding_matrix;
        return yields_by_prod;
    }
    
    TVectorD FoldYields(TVectorD yields_by_prod, std::string tag, Settings s){
        TMatrixD unfolding_matrix = GetUnfoldingMatrix(tag, s);
        yields_by_prod *= unfolding_matrix;
        return yields_by_prod;
    }
    
    TVectorD UnfoldYields(TVectorD yields_by_prod, int bin, Settings s){
        TMatrixD unfolding_matrix = GetUnfoldingMatrix(abs(bin), s);
        yields_by_prod *= unfolding_matrix.Invert();
        return yields_by_prod;
    }
    
    TVectorD UnfoldYields(TVectorD yields_by_prod, std::string tag, Settings s){
        TMatrixD unfolding_matrix = GetUnfoldingMatrix(tag, s);
        yields_by_prod *= unfolding_matrix.Invert();
        return yields_by_prod;
    }

}