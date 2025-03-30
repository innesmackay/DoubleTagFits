#include "Data.hpp"
#include "TextFileUtils.hpp"

std::unique_ptr<TChain> Data::LoadMCChain(TString sample, bool bkg_prod, bool bkg_decay){

    if (m_debug) m_log.info("Loading the " + sample + " into a TChain");

    // Get name of tree
    TString chain_name = m_tag + "_vs_KPi_truth_tree";
    if (bkg_decay){ chain_name = m_tag + "_vs_KPi_bkg_tree"; }

    // Get file path
    TString base_file_path = m_base_path + "signal/";
    if (bkg_prod){ base_file_path = m_base_path + "bkg/"; }

    // Load TChain
    auto mc_chain = std::make_unique<TChain>(chain_name);
    mc_chain->Add(base_file_path + sample + ".root");

    return mc_chain;
}


std::unique_ptr<RooDataSet> Data::LoadData(){

    if (m_debug) m_log.info("Loading in the data sample");

    // Create empty TChain
    TString file_path = "/data/lhcb/users/gilman/StrongPhaseWork/mytuples/Reprocess_Jan2024/data/";
    TChain data_chain(m_tag + "_vs_KPi_bkg_tree");

    // Check if data file provided in the config
    if (m_settings.key_exists("data_file")){
        if (m_debug) m_log.debug("Loading in data from " + m_settings.getT("data_file"));
        data_chain.Add(m_settings.getT("data_file"));        
    }
    else{ data_chain.Add(file_path + "data_combined.root"); }

    // Apply selection criteria
    std::unique_ptr<TTree> data_cut;
    if (m_apply_cuts){
        TString full_cut = Selection::ProdCut(m_prod, m_tag); // Production mechanism
        if (Selection::TagCut(m_tag) != "") full_cut += " & " + Selection::TagCut(m_tag); // Tag
        data_cut.reset(data_chain.CopyTree(full_cut));
        if (m_debug){
            m_log.debug("Applying cut to the data: " + full_cut);
            m_log.success( (std::to_string(data_cut->GetEntries()) + "/" + std::to_string(data_chain.GetEntries()) + " candidates passed the cut").c_str() );
        }
    }
    else{
        if (m_debug) m_log.warning("No cuts applied to the data");
        data_cut.reset(&data_chain);
    }

    // Load into a RooDataSet
    std::unique_ptr<RooDataSet> ds = std::make_unique<RooDataSet>("data", "", m_vars->data_vars, RooFit::Import(*data_cut));
    if (m_debug) ds->Print("v");

    return ds;
}


std::unique_ptr<RooDataSet> Data::LoadMCSample(TString sample, bool bkg_prod, bool bkg_decay, double weight_val){

    // Load in the MC sample as TChain
    auto mc_chain = LoadMCChain(sample, bkg_prod, bkg_decay);
    float nBefore = mc_chain->GetEntries();

    // Apply selection criteria
    std::unique_ptr<TTree> mc_cut;
    if (m_apply_cuts){
        TString full_cut = Selection::ProdCut(m_prod, m_tag); // production mechanism
        std::cout << full_cut << std::endl;
        if (Selection::TagCut(m_tag) != "") full_cut += " & " + Selection::TagCut(m_tag); // tag
        mc_cut.reset(mc_chain->CopyTree(full_cut));
        if (m_debug){
            m_log.debug("Applying cut to the MC sample: " + full_cut);
            m_log.success( (std::to_string(mc_cut->GetEntries()) + "/" + std::to_string(mc_chain->GetEntries()) + " candidates passed the cut").c_str() );
        }
    }
    else{
        if (m_debug) m_log.warning("No cuts applied to the sample");
        mc_cut.reset(mc_chain.release());
    }

    // Convert to RooDataSet
    RooArgList reqd_vars;
    reqd_vars.add(m_vars->data_vars);
    if (!bkg_decay){ reqd_vars.add(m_vars->signal_mc_vars); }
    else{ reqd_vars.add(m_vars->bkg_mc_vars); }

    std::unique_ptr<RooDataSet> mc;
    if (m_settings.key_exists("weight_mc") & m_settings.get("weight_mc") == "true"){
        RooRealVar weight(sample + "_weight", "", weight_val);
        mc = std::make_unique<RooDataSet>(sample + "_mc", "", RooArgSet(reqd_vars, weight), RooFit::WeightVar(weight), RooFit::Import(*mc_cut));
    }
    else{
        mc =  std::make_unique<RooDataSet>(sample + "_mc", "", reqd_vars, RooFit::Import(*mc_cut));
    }

    return mc;
}


void Data::TruthMatchComponents(){

    // Read components
    auto component_list = TextFileUtils::ReadList(m_settings.get("components"));

    // Get truth matching strings and apply
    if (m_tag == "KK"){
        TString kpi_vs_comb_cut = "((KPi_isPiK==1 || KPi_isTrue==1) && KK_vs_KPi_isTrue==0)";
        TString comb_vs_tag_cut = "(KPi_isPiK==0 & KPi_isTrue==0 & KK_vs_KPi_isTrue==1)";
        TString kpi_vs_kpipiz_cut = "";
        if (std::find(component_list.begin(), component_list.end(), "kpi_vs_kpipi0") != component_list.end()) {
            kpi_vs_kpipiz_cut = "( (KPi_isPiK==1 || KPi_isTrue==1) && KK_vs_KPi_isKPiPi0==1 )";
            kpi_vs_comb_cut = "((KPi_isPiK==1 || KPi_isTrue==1) && KK_vs_KPi_isTrue==0 && KK_vs_KPi_isKPiPi0==0 )";
        }
        shapes["kpi_vs_comb"] = std::unique_ptr<RooDataSet>(static_cast<RooDataSet*>(bkg_mc->reduce(kpi_vs_comb_cut)));
        shapes["comb_vs_tag"] = std::unique_ptr<RooDataSet>(static_cast<RooDataSet*>(bkg_mc->reduce(comb_vs_tag_cut)));
        shapes["kpi_vs_kpipi0"] = std::unique_ptr<RooDataSet>(static_cast<RooDataSet*>(bkg_mc->reduce(kpi_vs_kpipiz_cut)));
    }

}
