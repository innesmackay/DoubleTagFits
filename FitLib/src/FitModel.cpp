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

    // Create KDE
    RooKeysPdf* kpi_kde = new RooKeysPdf(m_prename + "kpi_kde", "", *m_vars->m_kpi, *m_data->signal_mc, RooKeysPdf::MirrorBoth, 2);
    RooKeysPdf* tag_kde = new RooKeysPdf(m_prename + "tag_kde", "", *m_vars->m_tag, *m_data->signal_mc, RooKeysPdf::MirrorBoth, 2);

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
    RooProdPdf* signal = new RooProdPdf(m_prename + "signal", "", *kpi_signal_shape, *tag_signal_shape);
    component_pdfs.add(*signal);

    // Yield
    RooRealVar* n_signal = new RooRealVar(m_prename + "n_signal", "", 204, 10, 2800);
    component_yields.add(*n_signal);

    // Make struct
    components["signal"].shape = signal;
    components["signal"].yield = n_signal;

    return;
}


void FitModel::AddKpiVsComb(){

    if (m_debug) m_log.info("Adding the KPi vs Comb. component");

    // Combinatorial shape
    RooAbsPdf* comb_shape;
    if (m_settings.getB("kde_bkgs")){
        comb_shape = new RooKeysPdf(m_prename + "kpi_vs_comb_comb_shape", "", *m_vars->m_tag, *m_data->shapes["kpi_vs_comb"], RooKeysPdf::MirrorBoth, 2 );
    }
    else if (m_settings.getB("expo_bkgs")){
        RooRealVar* exponent = new RooRealVar(m_prename + "kpi_vs_comb_exponent", "", -4, -50, 4);
        comb_shape = new RooExponential(m_prename + "kpi_vs_comb_comb_shape", "", *m_vars->m_tag, *exponent);
    }
    else{
        RooRealVar* exponent = new RooRealVar(m_prename + "kpi_vs_comb_exponent", "", -0.4, -1.0, 0.2);
        comb_shape = new RooChebychev(m_prename + "kpi_vs_comb_comb_shape", "", *m_vars->m_tag, *exponent);
    }

    // Product
    RooProdPdf* kpi_vs_comb = new RooProdPdf(m_prename + "kpi_vs_comb", "", *kpi_signal_shape, *comb_shape);
    component_pdfs.add(*kpi_vs_comb);

    // Yield
    RooRealVar* n_kpi_vs_comb = new RooRealVar(m_prename + "n_kpi_vs_comb", "", 50, 0, 500);
    component_yields.add(*n_kpi_vs_comb);

    // Make struct
    components["kpi_vs_comb"].shape = kpi_vs_comb;
    components["kpi_vs_comb"].yield = n_kpi_vs_comb;

    return;
}


void FitModel::AddCombVsTag(){

    if (m_debug) m_log.info("Adding the Comb. vs tag component");

    // Combinatorial shape
    RooAbsPdf* comb_shape;
    if (m_settings.getB("kde_bkgs")){
        comb_shape = new RooKeysPdf(m_prename + "comb_vs_tag_comb_shape", "", *m_vars->m_kpi, *m_data->shapes["comb_vs_tag"], RooKeysPdf::MirrorBoth, 2 );
    }
    else if (m_settings.getB("expo_bkgs")){
        RooRealVar* exponent = new RooRealVar(m_prename + "comb_vs_tag_exponent", "", -4, -50, 4);
        comb_shape = new RooExponential(m_prename + "comb_vs_tag_comb_shape", "", *m_vars->m_kpi, *exponent);
    }
    else{
        RooRealVar* exponent = new RooRealVar(m_prename + "comb_vs_tag_exponent", "", -0.4, -1.0, 0.2);
        comb_shape = new RooChebychev(m_prename + "comb_vs_tag_comb_shape", "", *m_vars->m_kpi, *exponent);
    }

    // Product
    RooProdPdf* comb_vs_tag = new RooProdPdf(m_prename + "comb_vs_tag", "", *comb_shape, *tag_signal_shape);
    component_pdfs.add(*comb_vs_tag);

    // Yield
    RooRealVar* n_comb_vs_tag = new RooRealVar(m_prename + "n_comb_vs_tag", "", 50, 0, 500);
    component_yields.add(*n_comb_vs_tag);

    // Make struct
    components["comb_vs_tag"].shape = comb_vs_tag;
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
    RooRealVar* n_qqbar = new RooRealVar(m_prename + "n_flat_qqbar", "", 300, 0., 5000);
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
        n_kpi_vs_kpipiz = new RooRealVar(m_prename + "n_kpi_vs_kpipi0", "", 75, 0, 500);
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
    double starting_mean = 3.92;
    if (m_settings.key_exists("starting_qqbar_mean")) starting_mean = m_settings.getD("starting_qqbar_mean");
    RooRealVar* qqbar_sigma = new RooRealVar(m_prename + "qqbar_sigma","", 0.015, 0., 0.180);
    RooRealVar* qqbar_mean = new RooRealVar(m_prename + "qqbar_mean","", starting_mean, 3.30, 4.20);
    RooAbsPdf* qqbar = new RooGenericPdf(m_prename + "correlated_qqbar", "exp(-0.5*pow(((@0+@1)-@3)/@2, 2))", RooArgSet(*m_vars->m_kpi, *m_vars->m_tag, *qqbar_sigma, *qqbar_mean));
    component_pdfs.add(*qqbar);

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
    components["correlated_qqbar"].shape = qqbar;
    components["correlated_qqbar"].yield = n_qqbar;

    return;
}


/*
void FitModel::AddKPiVsKPiComponent(){

    m_log.info("Adding the KPi vs KPi component");

    // Shape
    RooKeysPdf* kpi_vs_kpi_kpi_shape = new RooKeysPdf(m_prename + "kpi_vs_kpi_kpi_kde", "", *m_tagm, *m_datasets["kpi_vs_kpi"], RooKeysPdf::MirrorBoth, 2 );
    //RooAbsPdf* kpi_vs_kpi_kpi_shape;
    //if (m_settings.getB("smear")){
    //    if (m_debug){ m_log.debug("Smearing the Kpi vs. Kpi component with the Kpi smearing function"); }
    //    kpi_vs_kpi_kpi_shape = new RooFFTConvPdf(m_prename + "kpi_vs_kpi_kpi_shape", "", *m_tagm, *kpi_vs_kpi_kpi_kde, *m_sig_smear);
    //}
    //else{
    //    kpi_vs_kpi_kpi_shape = kpi_vs_kpi_kpi_kde;
    //}
    RooProdPdf* kpi_vs_kpi = new RooProdPdf(m_prename + "kpi_vs_kpi", "", *m_signal_kpi, *kpi_vs_kpi_kpi_shape);
    m_component_pdfs.add(*kpi_vs_kpi);

    // Yield
    if (m_settings.key_exists("kpi_vs_kpi_rate")){
        if (m_debug){ m_log.debug(("Fixing the Kpi vs Kpi yield to be " + std::to_string(m_settings.getD("kpi_vs_kpi_rate")) + " of the signal yield").c_str()); }
        RooRealVar* kpi_vs_kpi_rate = new RooRealVar(m_prename + "kpi_vs_kpi_rate", "", m_settings.getD("kpi_vs_kpi_rate"));
        kpi_vs_kpi_rate->setConstant();
        RooRealVar* n_signal = (RooRealVar*) m_yields.find(m_prename + "n_signal");
        RooFormulaVar* n_kpi_vs_kpi = new RooFormulaVar(m_prename + "n_kpi_vs_kpi", "@0 * @1", RooArgList(*kpi_vs_kpi_rate, *n_signal));
        m_yields.add(*n_kpi_vs_kpi);
    }
    else{
        if (m_debug){ m_log.debug("Allowing the Kpi vs Kpi yield to be free"); }
        RooRealVar* n_kpi_vs_kpi = new RooRealVar(m_prename + "n_kpi_vs_kpi", "", 75, 0, 500);
        m_yields.add(*n_kpi_vs_kpi);
        n_kpi_vs_kpi->setError(0.5);
    }
    return;
}

void FitModel::SetFreePars(Settings fitResults){
    m_log.info(("Fixing the parameters using results from " + fitResults.getFilename()).c_str());
    RooArgSet* allpars = m_pdf->getParameters(RooArgSet(*m_sigm, *m_tagm));
    TIterator* iter(allpars->createIterator());
    for (TObject *a = iter->Next(); a != 0; a = iter->Next()) {
        RooRealVar *p = (RooRealVar*) a;
        TString name = p->GetName();
        std::string std_string_name = std::string(name);
        if (m_debug){
            m_log.debug(("Setting " + std_string_name + " to " + fitResults.get(std_string_name)).c_str() );
        }
        p->setVal(fitResults.getD(std_string_name));
    }
    return;
}


void FitModel::ResetPDF(){
    RooAddPdf* pdf;
    m_pdf = pdf;
    this->InitialiseFitModel();
}
*/