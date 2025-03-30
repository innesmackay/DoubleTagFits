#ifndef BINNEDFITMODEL_H
#define BINNEDFITMODEL_H

#include "FitModel.hpp"
#include "Definitions.hpp"
#include "RooSimultaneous.h"
#include "RooKeysPdf.h"
#include "RooGaussian.h"
#include "RooGenericPdf.h"
#include "RooFFTConvPdf.h"
#include "RooExponential.h"
#include "RooChebychev.h"

class BinnedFitModel {

private:
    /* Settings class */
    Settings m_settings;

    /* Data class */
    Data* m_data;

    /* Variables class */
    Variables* m_vars;

    /* Debug boolean */
    bool m_debug;

    /* Logging class */
    Log m_log;

public:
    /** 
     * Initialise the class
     * @param settings fit configuration
     * @param vars Variables class
     * @param data Data class
     * @param debug boolean
    */
    BinnedFitModel(Settings settings, Variables* vars, Data* data, bool debug = false){
        m_settings = settings;
        m_vars = vars;
        m_data = data;
        m_debug = debug;
        m_log = Log("BinnedFitModel");
        pdf = std::make_unique<RooSimultaneous> ("sim_pdf", "", *m_vars->cats);
    }

    /** Empty PDF */
    std::unique_ptr<RooSimultaneous> pdf;

    /** Map of FitModels in each category */
    std::map<std::string, FitModel*> category_models;

    /** Make the signal shapes */
    RooAbsPdf* GetSignalShape(std::string prod, RooDataSet ds, std::string mode){
        RooAbsPdf* signal;
        RooRealVar* mass;
        if (mode == "kpi") mass = m_vars->m_kpi;
        else mass = m_vars->m_tag;

        RooKeysPdf* kde = new RooKeysPdf((prod + "_" + mode + "_kde").c_str(), "", *mass, ds, RooKeysPdf::MirrorBoth, 2);
        if (m_settings.getB("smear_signal")){
            if (m_debug){ m_log.debug("Convolving the KDE with Gaussian"); }
            RooRealVar* smear_width = new RooRealVar((prod + "_" + mode + "_smear_width").c_str(), "", 0.003, 1e-4, 0.01);
            RooRealVar* smear_mean = new RooRealVar((prod + "_" + mode + "_smear_mean").c_str(), "", 0, -0.003, 0.003);
            RooGaussian* smear_fnc = new RooGaussian((prod + "_" + mode + "_smear").c_str(), "", *mass, *smear_mean, *smear_width);
            signal = new RooFFTConvPdf((prod + "_" + mode + "_signal_shape").c_str(), "", *mass, *kde, *smear_fnc);
        }
        else signal = kde;
        return signal;
    }

    /** Make background slopes */
    RooAbsPdf* GetBkgShape(std::string prod, std::string mode, Settings s){
        RooAbsPdf* bkg;   
        RooRealVar* mass;
        if (mode == "kpi") mass = m_vars->m_tag;
        else mass = m_vars->m_kpi;
        if (s.getB("expo_bkgs")){
            RooRealVar* exponent = new RooRealVar(("shared_" + prod + "_" + mode + "_vs_comb_exponent").c_str(), "", -30, -40, 4);
            bkg = new RooExponential(("shared_" + prod + "_" + mode + "_vs_comb_comb_shape").c_str(), "", *mass, *exponent);
        }
        else{
            RooRealVar* exponent = new RooRealVar(("shared_" + prod + "_" + mode + "_vs_comb_exponent").c_str(), "", -0.4, -1.0, 0.2);
            bkg = new RooChebychev(("shared_" + prod + "_" + mode + "_vs_comb_comb_shape").c_str(), "", *mass, *exponent);
        }
        return bkg;
    }

    /** Make qqbar function */
    RooAbsPdf* GetQQPDF(std::string prod){
        RooRealVar* qqbar_sigma = new RooRealVar(("shared_" + prod + "_qqbar_sigma").c_str(),"", 0.015, 0., 0.180);
        RooRealVar* qqbar_mean = new RooRealVar(("shared_" + prod + "_qqbar_mean").c_str(),"", 3.8, 3.30, 4.20);
        RooAbsPdf* qqbar = new RooGenericPdf(("shared_" + prod + "_correlated_qqbar").c_str(), "exp(-0.5*pow(((@0+@1)-@3)/@2, 2))", RooArgSet(*m_vars->m_kpi, *m_vars->m_tag, *qqbar_sigma, *qqbar_mean));
        return qqbar;
    }

    /** Make the PDF */
    void MakePDF(){
        m_log.info("Creating the binned PDF");
        for (auto prod: Definitions::PRODS){
            Settings fit_settings(m_settings.get(prod + "_settings"));
            fit_settings.read();
            fit_settings.update_value("Yi_strategy", m_settings.get("Yi_strategy"));
            fit_settings.update_value("prod", prod);

            // Setup shared parameters
            // Signal PDFs
            std::string prod_cut = "";
            for (auto bin: Definitions::DP_BINS){
                if (bin == -8) prod_cut += "(kspipi_catz==kspipi_catz::" + Definitions::ProdBinLabel(prod, bin) + ")";
                else prod_cut += " || (kspipi_catz==kspipi_catz::" + Definitions::ProdBinLabel(prod, bin) + ")";
            }

            std::unique_ptr<RooDataSet> prod_ds = std::unique_ptr<RooDataSet>(static_cast<RooDataSet*>(m_data->signal_mc->reduce(RooArgSet(*m_vars->m_kpi, *m_vars->m_tag), prod_cut.c_str())));
            RooAbsPdf* kpi_signal = GetSignalShape(prod, *prod_ds, "kpi");
            RooAbsPdf* tag_signal = GetSignalShape(prod, *prod_ds, "tag");
            RooProdPdf* signal = new RooProdPdf((prod + "_signal").c_str(), "", *kpi_signal, *tag_signal);

            // Bkg slopes
            RooAbsPdf* kpi_vs_comb_comb_shape;
            RooAbsPdf* comb_vs_tag_comb_shape;
            if (m_settings.getB("shared_slopes")){
                kpi_vs_comb_comb_shape = GetBkgShape(prod, "kpi", fit_settings);
                comb_vs_tag_comb_shape = GetBkgShape(prod, "tag", fit_settings);
            }
            RooProdPdf* kpi_vs_comb_shape = new RooProdPdf((prod + "_kpi_vs_comb").c_str(), "", *kpi_signal, *kpi_vs_comb_comb_shape);
            RooProdPdf* comb_vs_tag_shape = new RooProdPdf((prod + "_comb_vs_tag").c_str(), "", *comb_vs_tag_comb_shape, *tag_signal);

            // qqb shape
            RooAbsPdf* qqbar_pdf = GetQQPDF(prod);

            for (auto bin: Definitions::DP_BINS){
                std::string category = Definitions::ProdBinLabel(prod, bin);
                fit_settings.update_value("prename", category + "_");
                fit_settings.update_value("cat_label", category);
                fit_settings.update_value("bin_number", std::to_string(bin));
                FitModel* m = new FitModel(fit_settings, m_vars, m_data, m_debug, category);
                m->ReadComponents();
                m->signal_shape = signal;
                m->kpi_signal_shape = kpi_signal;
                m->tag_signal_shape = tag_signal;
                if (m_settings.getB("shared_slopes")){
                    m->kpi_vs_comb_shape = kpi_vs_comb_shape;
                    m->comb_vs_tag_shape = comb_vs_tag_shape;
                }
                m->qqbar_shape = qqbar_pdf;
                m->MakePDF();
                m->pdf->Print("V");
                category_models[category] = m;
                pdf->addPdf( *m->pdf, category.c_str());
            }
        }
        return;
    }
};

#endif // BINNEDFITMODEL_H
