#ifndef DATAUTILS_H
#define DATAUTILS_H

#include "RooDataSet.h"
#include "TTree.h"
#include "TFile.h"
#include "TString.h"

#include <memory>

/**
 * Namespace containing utility functions to perform on datasets
*/
namespace DataUtils {

    /**
     * Randomly sample a fraction of a dataset
     * @param sample_frac double fraction of dataset to be sampled
     * @param ds RooDataSet to be sampled
    */
    std::unique_ptr<RooDataSet> RandomlySampleDataset(double sample_frac, std::unique_ptr<RooDataSet>& ds);

    /**
     * Save RooDataSet as a TTree in a ROOT file
     * @param dataset RooDataSet to be saved
     * @param filename TString name of the output file
     * @param treename TString name of the tree
    */
    void SaveDatasetToFile(RooDataSet dataset, TString filename, TString treename);

    /**
     * Delete pointers in a map to remove memory leaks
     * @param m map of strings and RooDataSets
    */
    inline void ClearMap(std::map<std::string, RooDataSet*> m){
        for (auto pair: m){ delete pair.second; }
        m.clear();
        return;
    }

}

#endif //  DataUtils_H
