
#include "DataUtils.hpp"

namespace DataUtils {

    std::unique_ptr<RooDataSet> RandomlySampleDataset(double sample_frac, std::unique_ptr<RooDataSet>& ds){
        int n = ds->sumEntries();
        int step = (1/sample_frac);
        TString dsname = ds->GetName();
        std::unique_ptr<RooDataSet> sampled = std::make_unique<RooDataSet>(dsname + "_sampled", "", *ds->get());
        for (int i=0; i<n; i+=step) { sampled->add(*ds->get(i)); }
        return sampled;
    }


    void SaveDatasetToFile(RooDataSet dataset, TString filename, TString treename){
        TFile file(filename, "RECREATE");
        auto tree = dataset.GetClonedTree();
        tree->Write(treename);
        file.Close();
        delete tree;
        return;
    }

}
