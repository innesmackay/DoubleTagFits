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
    inline std::map<int, int> DP_INDEX = {{0, -8}, {1, -7}, {2, -6}, {3, -5}, {4, -4}, {5, -3}, {6, -2}, {7, -1}, {8, 1}, {9, 2}, {10, 3}, {11, 4}, {12, 5}, {13, 6}, {14, 7}, {15, 8}};
    
    /** Production mechanisms and C values */    
    const std::vector<std::string> PRODS = {"D0D0", "DST0D0_g", "DST0D0_pi", "DST0DST0_even", "DST0DST0_odd"};
    inline std::map<std::string, int> C_VALS = {{"D0D0", -1}, {"DST0D0_g", 1}, {"DST0D0_pi", -1}, {"DST0DST0_even", 1}, {"DST0DST0_odd", -1}};
    inline std::map<int, std::string> PROD_INDEX = {{0, "D0D0"}, {1, "DST0D0_g"}, {2, "DST0D0_pi"}, {3, "DST0DST0_even"}, {4, "DST0DST0_odd"}};

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
