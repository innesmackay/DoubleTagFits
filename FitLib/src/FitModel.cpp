#include "FitModel.hpp"

#include "RooKeysPdf.h"
#include "RooGaussian.h"
#include "RooRealVar.h"
#include "RooNDKeysPdf.h"
#include "RooFormulaVar.h"
#include "RooChebychev.h"
#include "RooExponential.h"
#include "RooGenericPdf.h"
#include "RooFFTConvPdf.h"


void FitModel::AddSignal(){

    if (m_debug) m_log.info("Adding the signal component");

    // Check if default shape exists
    if (!signal_shape){

        // Create KDE
        RooKeysPdf* kpi_kde;
        RooKeysPdf* tag_kde;
        if (m_cat_name != ""){
            RooDataSet* cat_ds = (RooDataSet*)m_data->signal_mc->reduce(("kspipi_catz==kspipi_catz::" + m_cat_name).c_str());
            kpi_kde = new RooKeysPdf(m_prename + "kpi_kde", "", *m_vars->m_kpi, *cat_ds, RooKeysPdf::MirrorBoth, 2);
            tag_kde = new RooKeysPdf(m_prename + "tag_kde", "", *m_vars->m_tag, *cat_ds, RooKeysPdf::MirrorBoth, 2);
        }
        else{
            kpi_kde = new RooKeysPdf(m_prename + "kpi_kde", "", *m_vars->m_kpi, *m_data->signal_mc, RooKeysPdf::MirrorBoth, 2);
            tag_kde = new RooKeysPdf(m_prename + "tag_kde", "", *m_vars->m_tag, *m_data->signal_mc, RooKeysPdf::MirrorBoth, 2);
        }

        // Smear
        if (m_settings.getB("smear_signal")){
            if (m_debug){ m_log.debug("Convolving the KDE with Gaussian"); }
            RooRealVar* kpi_smear_width = new RooRealVar(m_prename + "kpi_smear_width", "", 0.003, 1e-4, 0.01);
            RooRealVar* kpi_smear_mean = new RooRealVar(m_prename + "kpi_smear_mean", "", -0.001, -0.003, 0.003);
            RooGaussian* kpi_smear = new RooGaussian(m_prename + "kpi_smear", "", *m_vars->m_kpi, *kpi_smear_mean, *kpi_smear_width);
            kpi_signal_shape = new RooFFTConvPdf(m_prename + "kpi_signal_shape", "", *m_vars->m_kpi, *kpi_kde, *kpi_smear);
            
            RooRealVar* tag_smear_width = new RooRealVar(m_prename + "tag_smear_width", "", 0.003, 1e-4, 0.01);
            RooRealVar* tag_smear_mean = new RooRealVar(m_prename + "tag_smear_mean", "", -0.001, -0.003, 0.003);
            RooGaussian* tag_smear = new RooGaussian(m_prename + "tag_smear", "", *m_vars->m_tag, *tag_smear_mean, *tag_smear_width);            
            tag_signal_shape = new RooFFTConvPdf(m_prename + "tag_signal_shape", "", *m_vars->m_tag, *tag_kde, *tag_smear);
        }
        else{
            kpi_signal_shape = kpi_kde;
            tag_signal_shape = tag_kde;
        }

        // Product
        signal_shape = new RooProdPdf(m_prename + "signal", "", *kpi_signal_shape, *tag_signal_shape);

    }
    component_pdfs.add(*signal_shape);

    // Yield
    RooAbsReal* n_signal;
    if (m_settings.key_exists("Yi_strategy")) n_signal = m_vars->Ni[m_settings.get("prod")][m_settings.getI("bin_number")];
    else{ n_signal = new RooRealVar(m_prename + "n_signal", "", 100, 0, 2800); }
    component_yields.add(*n_signal);

    // Make struct
    components["signal"].shape = signal_shape;
    components["signal"].yield = n_signal;

    return;
}


void FitModel::AddKpiVsComb(){

    if (m_debug) m_log.info("Adding the KPi vs Comb. component");

    // Combinatorial shape
    if (!kpi_vs_comb_shape){
        RooAbsPdf* kpi_vs_comb_comb_shape;
        if (m_settings.getB("kde_bkgs")){
            kpi_vs_comb_comb_shape = new RooKeysPdf(m_prename + "kpi_vs_comb_comb_shape", "", *m_vars->m_tag, *m_data->shapes["kpi_vs_comb"], RooKeysPdf::MirrorBoth, 2 );
        }
        else if (m_settings.getB("expo_bkgs")){
            RooRealVar* exponent = new RooRealVar(m_prename + "kpi_vs_comb_exponent", "", -4, -50, 4);
            kpi_vs_comb_comb_shape = new RooExponential(m_prename + "kpi_vs_comb_comb_shape", "", *m_vars->m_tag, *exponent);
        }
        else{
            RooRealVar* exponent = new RooRealVar(m_prename + "kpi_vs_comb_exponent", "", -0.4, -1.0, 0.2);
            kpi_vs_comb_comb_shape = new RooChebychev(m_prename + "kpi_vs_comb_comb_shape", "", *m_vars->m_tag, *exponent);
        }

        // Product
        kpi_vs_comb_shape = new RooProdPdf(m_prename + "kpi_vs_comb", "", *kpi_signal_shape, *kpi_vs_comb_comb_shape);
    }    
    component_pdfs.add(*kpi_vs_comb_shape);

    // Yield
    RooRealVar* n_kpi_vs_comb = new RooRealVar(m_prename + "n_kpi_vs_comb", "", 5, 0, 300);
    component_yields.add(*n_kpi_vs_comb);

    // Make struct
    components["kpi_vs_comb"].shape = kpi_vs_comb_shape;
    components["kpi_vs_comb"].yield = n_kpi_vs_comb;

    return;
}


void FitModel::AddCombVsTag(){

    if (m_debug) m_log.info("Adding the Comb. vs tag component");

    // Combinatorial shape
    if (!comb_vs_tag_shape){
        RooAbsPdf* comb_vs_tag_comb_shape;
        if (m_settings.getB("kde_bkgs")){
            comb_vs_tag_comb_shape = new RooKeysPdf(m_prename + "comb_vs_tag_comb_shape", "", *m_vars->m_kpi, *m_data->shapes["comb_vs_tag"], RooKeysPdf::MirrorBoth, 2 );
        }
        else if (m_settings.getB("expo_bkgs")){
            RooRealVar* exponent = new RooRealVar(m_prename + "comb_vs_tag_exponent", "", -4, -50, 4);
            comb_vs_tag_comb_shape = new RooExponential(m_prename + "comb_vs_tag_comb_shape", "", *m_vars->m_kpi, *exponent);
        }
        else{
            RooRealVar* exponent = new RooRealVar(m_prename + "comb_vs_tag_exponent", "", -0.4, -1.0, 0.2);
            comb_vs_tag_comb_shape = new RooChebychev(m_prename + "comb_vs_tag_comb_shape", "", *m_vars->m_kpi, *exponent);
        }
        // Product
        comb_vs_tag_shape = new RooProdPdf(m_prename + "comb_vs_tag", "", *comb_vs_tag_comb_shape, *tag_signal_shape);
    }

    component_pdfs.add(*comb_vs_tag_shape);

    // Yield
    RooRealVar* n_comb_vs_tag = new RooRealVar(m_prename + "n_comb_vs_tag", "", 5, 0, 300);
    component_yields.add(*n_comb_vs_tag);

    // Make struct
    components["comb_vs_tag"].shape = comb_vs_tag_shape;
    components["comb_vs_tag"].yield = n_comb_vs_tag;

    return;
}


void FitModel::AddComb(){

    if (m_debug) m_log.info("Adding pure combinatorial component");

    RooAbsPdf* qqbar_kpi_shape;
    if (m_settings.getB("kpi_expo_qqbar")){
        RooRealVar* qqbar_kpi_exponent = new RooRealVar(m_prename + "qqbar_kpi_exponent", "", -4, -50, 4);
        qqbar_kpi_shape = new RooExponential(m_prename + "qqbar_kpi_shape", "", *m_vars->m_kpi, *qqbar_kpi_exponent);
    }
    else if (m_settings.getB("kpi_cheb2_qqbar")){
        RooRealVar* qqbar_kpi_c0 = new RooRealVar(m_prename + "qqbar_kpi_c0", "", -0.4, -1.0, 1.0);
        RooRealVar* qqbar_kpi_c1 = new RooRealVar(m_prename + "qqbar_kpi_c1", "", 0.1, -1.0, 1.0);
        qqbar_kpi_shape = new RooChebychev(m_prename + "qqbar_kpi_shape", "", *m_vars->m_kpi, RooArgList(*qqbar_kpi_c0, *qqbar_kpi_c1));
    }
    else{
        RooRealVar* qqbar_kpi_exponent = new RooRealVar(m_prename + "qqbar_kpi_exponent", "", -0.4, -1.0, 0.2);
        qqbar_kpi_shape = new RooChebychev(m_prename + "qqbar_kpi_shape", "", *m_vars->m_kpi, *qqbar_kpi_exponent);
    }

    RooAbsPdf* qqbar_tag_shape;
    if (m_settings.getB("tag_expo_qqbar")){
        RooRealVar* qqbar_tag_exponent = new RooRealVar(m_prename + "qqbar_tag_exponent", "", -4, -50, 4);
        qqbar_tag_shape = new RooExponential(m_prename + "qqbar_tag_shape", "", *m_vars->m_tag, *qqbar_tag_exponent);
    }
    else if (m_settings.getB("tag_cheb2_qqbar")){
        RooRealVar* qqbar_tag_c0 = new RooRealVar(m_prename + "qqbar_tag_c0", "", -0.4, -1.0, 0.5);
        RooRealVar* qqbar_tag_c1 = new RooRealVar(m_prename + "qqbar_tag_c1", "", 0., -1.0, 1.0);
        qqbar_tag_shape = new RooChebychev(m_prename + "qqbar_tag_shape", "", *m_vars->m_tag, RooArgList(*qqbar_tag_c0, *qqbar_tag_c1));
    }
    else{
        RooRealVar* qqbar_tag_exponent = new RooRealVar(m_prename + "qqbar_tag_exponent", "", -0.1, -1.0, 0.2);
        qqbar_tag_shape = new RooChebychev(m_prename + "qqbar_tag_shape", "", *m_vars->m_tag, *qqbar_tag_exponent);
    }

    // Product
    RooProdPdf* qqbar = new RooProdPdf(m_prename + "flat_qqbar", "", *qqbar_kpi_shape, *qqbar_tag_shape);
    component_pdfs.add(*qqbar);

    // Yield
    RooRealVar* n_qqbar = new RooRealVar(m_prename + "n_flat_qqbar", "", 25, 0., 5000);
    component_yields.add(*n_qqbar);
    
    // Make struct
    components["flat_qqbar"].shape = qqbar;
    components["flat_qqbar"].yield = n_qqbar;

    return;
}


void FitModel::AddKPiVsKPiPi0(){

    m_log.info("Adding the KPi vs KPiPi0 component");

    // Shape
    RooKeysPdf* kpipiz_shape = new RooKeysPdf(m_prename + "kpipi0_kde", "", *m_vars->m_tag, *m_data->shapes["kpi_vs_kpipi0"], RooKeysPdf::MirrorBoth, 2 );
    RooProdPdf* kpi_vs_kpipiz = new RooProdPdf(m_prename + "kpi_vs_kpipi0", "", *kpi_signal_shape, *kpipiz_shape);
    component_pdfs.add(*kpi_vs_kpipiz);

    // Yield
    RooAbsReal* n_kpi_vs_kpipiz;
    if (m_settings.key_exists("kpi_vs_kpipi0_rate")){
        if (m_debug){ m_log.debug(("Fixing the Kpi vs Kpipi0 yield to be " + std::to_string(m_settings.getD("kpi_vs_kpipi0_rate")) + " of the signal yield").c_str()); }
        RooRealVar* kpi_vs_kpipiz_rate = new RooRealVar(m_prename + "kpi_vs_kpipi0_rate", "", m_settings.getD("kpi_vs_kpipi0_rate"));
        kpi_vs_kpipiz_rate->setConstant();
        n_kpi_vs_kpipiz = new RooFormulaVar(m_prename + "n_kpi_vs_kpipi0", "@0 * @1", RooArgList(*kpi_vs_kpipiz_rate, *components["signal"].yield));
    }
    else{
        n_kpi_vs_kpipiz = new RooRealVar(m_prename + "n_kpi_vs_kpipi0", "", 120, 0, 500);
    }
    component_yields.add(*n_kpi_vs_kpipiz);

    // Make struct
    components["kpi_vs_kpipi0"].shape = kpi_vs_kpipiz;
    components["kpi_vs_kpipi0"].yield = n_kpi_vs_kpipiz;

    return;
}


void FitModel::AddCorrelatedQQBar(){

    if (m_debug) m_log.info("Adding correlated qqbar component");

    // Shape
    if (!qqbar_shape){
        double starting_mean = 3.92;
        if (m_settings.key_exists("starting_qqbar_mean")) starting_mean = m_settings.getD("starting_qqbar_mean");
        RooRealVar* qqbar_sigma = new RooRealVar(m_prename + "qqbar_sigma","", 0.015, 0., 0.180);
        RooRealVar* qqbar_mean = new RooRealVar(m_prename + "qqbar_mean","", starting_mean, 3.30, 4.20);
        qqbar_shape = new RooGenericPdf(m_prename + "correlated_qqbar", "exp(-0.5*pow(((@0+@1)-@3)/@2, 2))", RooArgSet(*m_vars->m_kpi, *m_vars->m_tag, *qqbar_sigma, *qqbar_mean));
    }
    component_pdfs.add(*qqbar_shape);

    // RooRealVar qqbar_sigma_r(m_prename + "qqbar_sigma_r","", 0.015, 0., 0.180);
    // RooRealVar qqbar_alpha("qqbar_alpha","", 0.04, 0, 5);
    // RooRealVar qqbar_n("qqbar_n","", 10);
    // RooRealVar qqbar_coeff("qqbar_coeff","", 0., -50, 50);
    // qqbar_n->setConstant(true);
    // gInterpreter->ProcessLine("#include \"FitLib/FitLib/TwoDShapes.hpp\"");
    //if (m_settings.key_exists("correlated_cb") & m_settings.getB("correlated_cb")){
    //    auto CB_Formula = "CB(x[0], x[1], x[2], x[3], x[4], x[5])";
    //    qqbar = new RooGenericPdf("corr_qqbar", CB_Formula,  RooArgSet(*m_sigm, *m_tagm, *qqbar_mean, *qqbar_sigma, *qqbar_alpha, *qqbar_n));
    //}
    //else if (m_settings.key_exists("correlated_gaussexp") & m_settings.getB("correlated_gaussexp")){
    //    auto GaussExp_Formula = "GaussExp(x[0], x[1], x[2], x[3], x[4])";
    //    qqbar = new RooGenericPdf("corr_qqbar", GaussExp_Formula,  RooArgSet(*m_sigm, *m_tagm, *qqbar_mean, *qqbar_sigma, *qqbar_coeff));
    //}
    //else if (m_settings.key_exists("correlated_asymgauss") & m_settings.getB("correlated_asymgauss")){
    //    auto AsymGauss_Formula = "AsymGauss(x[0], x[1], x[2], x[3], x[4])";
    //    qqbar_sigma->SetName("qqbar_sigma_l");
    //    qqbar = new RooGenericPdf("corr_qqbar", AsymGauss_Formula,  RooArgSet(*m_sigm, *m_tagm, *qqbar_mean, *qqbar_sigma, *qqbar_sigma_r));
    //}
    
    // Yield
    RooRealVar* n_qqbar = new RooRealVar(m_prename + "n_correlated_qqbar", "", 50, 0., 3000);
    component_yields.add(*n_qqbar);

    // Make struct
    components["correlated_qqbar"].shape = qqbar_shape;
    components["correlated_qqbar"].yield = n_qqbar;

    return;
}



void FitModel::AddKPiVsKPi(){

    m_log.info("Adding the KPi vs KPi component");

    // Shape
    RooKeysPdf* kpi_shape = new RooKeysPdf(m_prename + "kpi_bkg_kde", "", *m_vars->m_tag, *m_data->shapes["kpi_vs_kpi"], RooKeysPdf::MirrorBoth, 2 );
    RooProdPdf* kpi_vs_kpi = new RooProdPdf(m_prename + "kpi_vs_kpi", "", *kpi_signal_shape, *kpi_shape);
    component_pdfs.add(*kpi_vs_kpi);

    // Yield
    RooAbsReal* n_kpi_vs_kpi;
    if (m_settings.key_exists("kpi_vs_kpi_rate")){
        if (m_debug){ m_log.debug(("Fixing the Kpi vs Kpi yield to be " + std::to_string(m_settings.getD("kpi_vs_kpi_rate")) + " of the signal yield").c_str()); }
        RooRealVar* kpi_vs_kpi_rate = new RooRealVar(m_prename + "kpi_vs_kpi_rate", "", m_settings.getD("kpi_vs_kpi_rate"));
        kpi_vs_kpi_rate->setConstant();
        n_kpi_vs_kpi = new RooFormulaVar(m_prename + "n_kpi_vs_kpi", "@0 * @1", RooArgList(*kpi_vs_kpi_rate, *components["signal"].yield));
    }
    else{
        n_kpi_vs_kpi = new RooRealVar(m_prename + "n_kpi_vs_kpi", "", 120, 0, 500);
    }
    component_yields.add(*n_kpi_vs_kpi);

    // Make struct
    components["kpi_vs_kpi"].shape = kpi_vs_kpi;
    components["kpi_vs_kpi"].yield = n_kpi_vs_kpi;

    return;
}


void FitModel::AddDSTpDSTm(){

    if (m_debug) m_log.info("Adding the D*+D*- component");
    Settings dstpsdstm_yields("inputs/DSTpDSTm.txt");
    dstpsdstm_yields.read();

    // Yield
    RooRealVar* n_dstpdstm = new RooRealVar(m_prename + "n_dstpdstm", "", dstpsdstm_yields.getD(m_settings.get("cat_label")));
    n_dstpdstm->setConstant(true);
    component_yields.add(*n_dstpdstm);
    component_pdfs.add(*signal_shape);

    // Make struct
    components["dstpdstm"].shape = signal_shape;
    components["dstpdstm"].yield = n_dstpdstm;

    return;
}



void FitModel::AddDSTpDm(){

    if (m_debug) m_log.info("Adding the D*+D- component");

    // Load shape
    if (!dstpdm_shape){
        auto dstpdm_data = m_data->LoadMCSample("DSTpDm_combined_5x", true, true, 1./5);
        RooDataSet* reduced_dstpdm_data = (RooDataSet*) dstpdm_data->reduce("((KSPiPi_vs_KPi_mPlus>1.55) || (KSPiPi_vs_KPi_mMinus>1.55)) & (KSPiPi_mInv<2)");
        RooKeysPdf* dstpdm_kde_shape = new RooKeysPdf("shared_dstpdm_shape", "", *m_vars->m_tag, *reduced_dstpdm_data, RooKeysPdf::MirrorBoth, 2);
        delete reduced_dstpdm_data;
        dstpdm_shape = new RooProdPdf("dstpdm_shape", "", *kpi_signal_shape, *dstpdm_kde_shape);
    }

    // Yield
    Settings dstpdm_yields("inputs/DSTpDm.txt");
    dstpdm_yields.read();
    RooRealVar* n_dstpdm = new RooRealVar(m_prename + "n_dstpdm", "", dstpdm_yields.getD(m_settings.get("cat_label")));
    n_dstpdm->setConstant(true);
    component_yields.add(*n_dstpdm);
    component_pdfs.add(*dstpdm_shape);

    // Make struct
    components["dstpdm"].shape = dstpdm_shape;
    components["dstpdm"].yield = n_dstpdm;

    return;
}
