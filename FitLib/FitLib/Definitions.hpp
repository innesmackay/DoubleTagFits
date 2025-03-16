#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <vector>
#include <map>
#include <string>

#include "TString.h"

namespace Definitions {

    /** Dalitz plot bin numbers */
    const std::vector<int> DP_BINS = {-8, -7, -6, -5, -4, -3, -2, -1, 1, 2, 3, 4, 5, 6, 7, 8};
    const std::vector<int> SYMM_DP_BINS = {-8, -7, -6, -5, -4, -3, -2, -1};
    
    /** Production mechanisms and C values */    
    const std::vector<std::string> PRODS = {"D0D0", "DST0D0_g", "DST0D0_pi", "DST0DST0_even", "DST0DST0_odd"};
    const std::map<std::string, int> C_VALS = {{"D0D0", -1}, {"DST0D0_g", 1}, {"DST0D0_pi", -1}, {"DST0DST0_even", 1}, {"DST0DST0_odd", -1}};

    /** Category labels */    
    inline std::string BinLabel(int bin_no){
        std::string abs_bin = std::to_string(abs(bin_no));
        if (bin_no<0) return "_m" + abs_bin;
        else return "_p" + abs_bin;
    }
    inline std::string ProdBinLabel(std::string prod, int bin_no){
        std::string abs_bin = std::to_string(abs(bin_no));
        if (bin_no<0) return prod + "_m" + abs_bin;
        else return prod + "_p" + abs_bin;
    }

}

#endif //  Definitions_H
