#ifndef VARIABLES_H
#define VARIABLES_H

#include "Settings.hpp"
#include "Log.hpp"
#include "Definitions.hpp"

#include "RooRealVar.h"
#include "RooArgList.h"
#include "RooFormulaVar.h"
#include "RooCategory.h"

#include "TString.h"

class Variables {

private:
    /**
    * Map of reweighting factors for each tag
    */
    std::map<TString, double> m_reweighting_factors = { {"KK", 0.408/0.394}, {"PiPi", 0.1454/0.140}, {"KSPi0", 1.24/1.22}, {"PiPiPi0", 1.49/2.15}, {"KPi", 3.947/3.89}, {"KSPiPi", 1} };

public:
    /**
     * Define empty RooRealVars for DATA
    */
    RooRealVar* m_kpi;
    RooRealVar* m_tag;
    RooRealVar* e_miss;
    RooRealVar* m_rec_d;
    RooRealVar* m_miss_dd;
    RooRealVar* slow_pi_p;
    RooRealVar* m_miss_ddg;
    RooRealVar* m_rec_dst;
    RooRealVar* delta_m_dst;

    // KS-specific
    RooRealVar* ks_flight_sig;
    RooRealVar* m_ks_pipi;

    // KSpipi specific
    RooRealVar* dalitz_bin;
    RooRealVar* kaon_charge;
    RooRealVar* m_ks_piplus;
    RooRealVar* m_ks_piminus;
    RooFormulaVar* flavour_bin;

    /**
     * Define empty RooRealVars for SIGNAL MC
    */
    RooRealVar* has_isr;
    RooRealVar* Dst_to_gamma;
    RooRealVar* Dst_to_piz;
    RooRealVar* DstDst_to_gamma_gamma;
    RooRealVar* DstDst_to_piz_gamma;
    RooRealVar* DstDst_to_piz_piz;
    RooRealVar* true_dalitz_bin;

    /**
     * Define empty RooRealVars for BKG MC
    */
    RooRealVar* kpi_is_true;
    RooRealVar* kpi_is_pik;
    RooRealVar* tag_is_true;
    RooRealVar* tag_is_kpipiz;
    RooRealVar* pipipiz_is_kpi;
    RooRealVar* pipipiz_is_kspiz;
    RooRealVar* kspiz_is_pipipiz;
    RooRealVar* kspipi_is_four_pi;
    RooRealVar* kspipi_is_kspipi_gamma;
    RooRealVar* kspipi_is_four_pi_gamma;

    /**
     * Define empty reweighting variables
    */
    RooRealVar* signal_mc_weight;
    RooRealVar* bkg_mc_weight;

    /**
     * KSpipi categories
    */
    RooCategory* cats;

    /**
     * Define lists of variables
    */
    RooArgList data_vars;
    RooArgList signal_mc_vars;
    RooArgList bkg_mc_vars;
    RooArgList weight_vars;

    /**
    * Empty constructor function
    */
    Variables(){}

    /**
    * Constructor function, initialises all variables
    * @param settings settings class with fit config
    */
    Variables(Settings s){

        // Name of tag
        TString tag = s.getT("tag");

        // General
        m_kpi = new RooRealVar("KPi_mInv", "", s.getD("m_sig_low"),  s.getD("m_sig_high"));
        m_tag = new RooRealVar(tag + "_mInv", "", s.getD("m_tag_low"),  s.getD("m_tag_high"));
        e_miss = new RooRealVar(tag + "_vs_KPi_EMiss", "", -20, 20);
        m_rec_d = new RooRealVar(tag + "_vs_KPi_BestMRec", "", -20, 20);
        m_miss_dd = new RooRealVar(tag + "_vs_KPi_MMiss2", "", -20, 20);
        slow_pi_p = new RooRealVar("slowestPion_p", "", -20., 20);
        m_miss_ddg = new RooRealVar(tag + "_vs_KPi_bestDSTDSTPhoton_MM2", "", -20, 20);
        m_rec_dst = new RooRealVar(tag + "_vs_KPi_bestDSTDSTPhoton_DSTMRec", "", -20, 20);
        delta_m_dst = new RooRealVar(tag + "_vs_KPi_bestDSTDSTPhoton_deltaM", "", -20, 20); 
        data_vars.add(RooArgList(*m_kpi, *m_tag, *e_miss, *m_rec_d, *m_miss_dd, *slow_pi_p, *m_miss_ddg, *m_rec_dst, *delta_m_dst));

        // KSPi0 / PiPiPi0 / KSPiPi specific
        ks_flight_sig = new RooRealVar(tag + "_ks_flightSig", "", -1e5, 1e5);
        m_ks_pipi = new RooRealVar(tag + "_ks_mInv", "", 0.25, 1.87);

        // KSPiPi specific
        dalitz_bin = new RooRealVar("KSPiPi_vs_KPi_bin", "", -9, 9);
        kaon_charge = new RooRealVar("KPi_KCharge", "", -2, 2);
        m_ks_piplus = new RooRealVar("KSPiPi_vs_KPi_mPlus", "", 0, 3); 
        m_ks_piminus = new RooRealVar("KSPiPi_vs_KPi_mMinus", "", 0, 3);
        flavour_bin = new RooFormulaVar("KSPiPi_vs_KPi_flavour_bin", "", "-1 * @0 * @1", RooArgSet(*kaon_charge, *dalitz_bin));

        // Signal MC
        Dst_to_gamma = new RooRealVar("DST_ToGamma", "", -2, 2);
        Dst_to_piz = new RooRealVar("DST_ToPi0", "", -2, 2);
        DstDst_to_gamma_gamma = new RooRealVar("DSTDST_ToGammaGamma", "", -2, 2);
        DstDst_to_piz_gamma = new RooRealVar("DSTDST_ToPi0Gamma", "", -2, 2);
        DstDst_to_piz_piz = new RooRealVar("DSTDST_ToPi0Pi0", "", -2, 2);
        true_dalitz_bin = new RooRealVar("KSPiPi_vs_KPi_true_bin", "", -9, 9);
        signal_mc_vars.add(RooArgList(*Dst_to_gamma, *Dst_to_piz, *DstDst_to_gamma_gamma, *DstDst_to_piz_gamma, *DstDst_to_piz_piz));

        // Bkg MC
        kpi_is_true = new RooRealVar("KPi_isTrue", "", -2., 2.);
        kpi_is_pik = new RooRealVar("KPi_isPiK", "", -2., 2.);
        tag_is_true = new RooRealVar(tag + "_vs_KPi_isTrue", "", -2., 2.);
        bkg_mc_vars.add(RooArgList(*kpi_is_true, *kpi_is_pik, *tag_is_true));

        tag_is_kpipiz = new RooRealVar(tag + "_vs_KPi_isKPiPi0", "", -2., 2.);

        pipipiz_is_kpi = new RooRealVar("PiPiPi0_vs_KPi_isKPi", "", -2., 2.);
        pipipiz_is_kspiz = new RooRealVar("PiPiPi0_vs_KPi_isKSPi0", "", -2., 2.);

        kspiz_is_pipipiz = new RooRealVar("KSPi0_vs_KPi_isPiPiPi0", "", -2., 2.);

        kspipi_is_four_pi = new RooRealVar("KSPiPi_is4Pi", "", -2., 2.);
        kspipi_is_kspipi_gamma = new RooRealVar("KSPiPi_isKSPiPiGamma", "", -2., 2.);
        kspipi_is_four_pi_gamma = new RooRealVar("KSPiPi_is4PiGamma", "", -2., 2.);

        // Add tag specific variables to lists
        if (tag == "KK"){
            bkg_mc_vars.add(*tag_is_kpipiz);
        }
        else if (tag == "PiPiPi0"){
            bkg_mc_vars.add(RooArgList(*tag_is_kpipiz, *pipipiz_is_kpi, *pipipiz_is_kspiz));
            data_vars.add(RooArgList(*ks_flight_sig, *m_ks_pipi));
        }
        else if (tag == "KSPi0"){
            bkg_mc_vars.add(RooArgList(*kspiz_is_pipipiz));
            data_vars.add(*ks_flight_sig);
        }
        else if (tag == "KSPiPi"){
            bkg_mc_vars.add(RooArgList(*kspipi_is_four_pi, *kspipi_is_kspipi_gamma, *kspipi_is_four_pi_gamma));
            data_vars.add(RooArgList(*ks_flight_sig, *m_ks_pipi, *dalitz_bin, *kaon_charge, *m_ks_piplus, *m_ks_piminus, *flavour_bin));
            signal_mc_vars.add(*true_dalitz_bin);
        }

        // Reweighting variables
        signal_mc_weight = new RooRealVar("sigMCweight", "", 1.0/40.0*m_reweighting_factors["KPi"]*m_reweighting_factors[tag]);
        bkg_mc_weight = new RooRealVar("bkgMCweight", "", 1.0/5.0*m_reweighting_factors["KPi"]*m_reweighting_factors[tag]);

        // KSpipi categories
        cats = new RooCategory("kspipi_catz", "KSpipi category");
        for (auto prod : Definitions::PRODS){
            for (auto bin : Definitions::DP_BINS){
                cats->defineType(Definitions::ProdBinLabel(prod, bin).c_str());
            }
        }
    }

};

#endif //  Variables_H


/*
    void InitialiseCategories();
    RooCategory* GetCategories(){ return m_categories; }
    void SetCategories(RooCategory* cat){ m_categories = cat; }
    void SetDebug(bool debug){ m_debug = debug; }
    void InitialiseCiSiKi();
    RooRealVar* GetCiVar(int bin){ return m_ci_vars[bin]; }
    RooRealVar* GetSiVar(int bin){ return m_si_vars[bin]; }
    RooRealVar* GetKiVar(int bin){ return m_Ki_vars[bin]; }
    void InitialiseMixingVariables();
    RooRealVar* GetXD(){ return m_xMix; }
    RooRealVar* GetYD(){ return m_yMix; }
    void InitialiseObservables(double rcos_start=-0.05, double rsin_start=0, TString prename="");
    RooRealVar* Get_rCosDelta(){ return m_rKpi_cos_deltaKpi; }
    RooRealVar* Get_rSinDelta(){ return m_rKpi_sin_deltaKpi; }
    void InitialiseYi();
    RooAbsArg* GetYiVar(std::string key, int bin){ return m_Yi_vars[key][bin]; }
    std::map<std::string, std::map<int, RooAbsArg*>> GetYi(){return m_Yi_vars; }
    void InitialiseTotalYields();
    RooRealVar* GetTotalYield(TString prod){ return m_total_yields[prod]; }
    void InitialiseNi(TString prename="");
    RooFormulaVar* GetFoldedYield(TString prod, int bin){ return m_folded_Ni[prod][bin]; }
    void ResetCiSi(std::string inputs_file){
        m_log.info("Changin the ci and si inputs ...");
        m_inputs.SetNewCi(inputs_file);
        m_inputs.SetNewSi(inputs_file);
        return;
    }
    void ResetKi(std::string inputs_file){
        m_log.info("Changing the Ki inputs ...");
        m_inputs.SetNewKi(inputs_file);
        return;
    }
*/