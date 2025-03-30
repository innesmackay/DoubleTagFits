#ifndef DATA_H
#define DATA_H

#include "Settings.hpp"
#include "Variables.hpp"
#include "Selection.hpp"
#include "Log.hpp"
#include "DataUtils.hpp"

#include "RooDataSet.h"

#include "TChain.h"

#include <map>
#include <vector>
#include <string>
#include <memory>

/**
 * Class to handle the data and MC samples.
*/
class Data {

private:
    /** Base data path */
    const TString m_base_path = "/data/lhcb/users/gilman/StrongPhaseWork/mytuples/Reprocess_Jan2024/";

    /** Fit configuration class */
    Settings m_settings;

    /** Name of tag */
    TString m_tag;

    /** Apply cuts to samples? */
    bool m_apply_cuts;

    /** Debug flag */
    bool m_debug;

    /** Logging class */
    Log m_log;

    /** Variables class */
    Variables* m_vars;

    /**
    * Function to load in an MC TChain
    * @param sample name of the sample
    * @param bkg_prod boolean - not from DD
    * @param bkg_decay boolean - not correct final state
    */
    std::unique_ptr<TChain> LoadMCChain(TString sample, bool bkg_prod, bool bkg_decay);


public:
    /**
     * Create empty datasets
    */
    std::unique_ptr<RooDataSet> data;
    std::unique_ptr<RooDataSet> signal_mc;
    std::unique_ptr<RooDataSet> bkg_mc;
    std::unique_ptr<RooDataSet> other_prod_mc;
    std::map<std::string, std::unique_ptr<RooDataSet>> shapes;

    /** Name of prod */
    TString m_prod;

    /**
    Constructor function, sets all of the private member objects
    * @param settings fit config
    * @param vars Class storing all of the variables
    * @param cutSamples boolean apply cuts to the samples
    */
    Data(Settings settings, Variables* vars, bool cut_samples = true, bool debug = false){

        m_settings  = settings;
        m_vars = vars;
        m_apply_cuts = cut_samples;
        m_debug = debug;
        Initialise(); 

        if (m_settings.get("tag") == "KSPiPi"){
            std::map<std::string, RooDataSet*> data_map, mc_map;
            for (auto prod: Definitions::PRODS){
                m_prod = prod;
                std::unique_ptr<RooDataSet> ds = std::move(LoadData());
                std::unique_ptr<RooDataSet> mc_ds = std::move(LoadSignalMC());
                for (int bin: Definitions::DP_BINS){
                    auto cat_label = Definitions::ProdBinLabel(prod, bin);
                    data_map[cat_label] = static_cast<RooDataSet*>(ds->reduce(Selection::BinCut(bin)));
                    mc_map[cat_label] = static_cast<RooDataSet*>(mc_ds->reduce(Selection::BinCut(bin)));
                }
            }
            data = std::make_unique<RooDataSet>("comb_data", "", RooArgSet(*m_vars->m_kpi, *m_vars->m_tag), RooFit::Index(*m_vars->cats), RooFit::Import(data_map));
            signal_mc = std::make_unique<RooDataSet>("comb_mc", "", RooArgSet(*m_vars->m_kpi, *m_vars->m_tag), RooFit::Index(*m_vars->cats), RooFit::Import(mc_map));
            DataUtils::ClearMap(data_map);
            DataUtils::ClearMap(mc_map);
        }
        else{
            data = std::move(LoadData());
            signal_mc = std::move(LoadSignalMC());
            LoadBkgMC();
            LoadOtherProdMC();
            TruthMatchComponents();
        }

    }

    /** Initialise class variables */
    void Initialise(){
        m_log = Log("Data");
        m_tag = m_settings.getT("tag");
        m_prod = m_settings.getT("prod");       
        return;
    }

    /**
    * Load the MC samples
    */
    std::unique_ptr<RooDataSet> LoadSignalMC(){
        std::unique_ptr<RooDataSet> all_mc;
        std::unique_ptr<RooDataSet> dd_mc = LoadMCSample("D0D0_40x_combined", false, false, 1./40);
        std::unique_ptr<RooDataSet> dstd_mc = LoadMCSample("DST0D0_40x_combined", false, false, 1./40);
        std::unique_ptr<RooDataSet> dstdst_mc = LoadMCSample("DST0DST0_40x_combined", false, false, 1./40);
        dstd_mc->append(*dstdst_mc);
        if (dd_mc->sumEntries() != 0){ dstd_mc->append(*dd_mc); }
        if (m_settings.getB("sample_signal_mc")){ all_mc = DataUtils::RandomlySampleDataset(m_settings.getD("sampling_frac"), dstd_mc); }
        else{ all_mc = std::move(dstd_mc); }
        return all_mc;
    }
    void LoadBkgMC(){
        std::unique_ptr<RooDataSet> dd_bkg_mc = LoadMCSample("D0D0_40x_combined", false, true, 1./40);
        bkg_mc = LoadMCSample("DST0D0_40x_combined", false, true, 1./40);
        std::unique_ptr<RooDataSet> dstdst_bkg_mc = LoadMCSample("DST0DST0_40x_combined", false, true, 1./40);        
        bkg_mc->append(*dstdst_bkg_mc);
        bkg_mc->append(*dd_bkg_mc);
        return;
    }
    std::unique_ptr<RooDataSet> GetQQBarMC(){ return LoadMCSample("qq_2x_combined", true, true, 1./2); }
    void LoadOtherProdMC(){ other_prod_mc = LoadMCSample("bkg_5x_combined", true, true, 1./5); return; }
    std::unique_ptr<RooDataSet> LoadData();

    /**
     * Apply truth cuts to bkg samples
    */
    void TruthMatchComponents();

    /**
    * Function to load in an MC RooDataSet
    * @param sample name of the sample
    * @param bkg_prod boolean - not from DD
    * @param bkg_decay boolean - not correct final state
    * @param weight_val per-event weights
    */
    std::unique_ptr<RooDataSet> LoadMCSample(TString sample, bool bkg_prod, bool bkg_decay, double weight_val);

};

#endif //  Data_H