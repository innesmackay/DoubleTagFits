#ifndef FITMODEL_H
#define FITMODEL_H

#include "Settings.hpp"
#include "Log.hpp"
#include "Variables.hpp"
#include "Data.hpp"
#include "TextFileUtils.hpp"

#include "RooAddPdf.h"
#include "RooAbsPdf.h"
#include "RooProdPdf.h"
#include "RooAbsReal.h"

class FitModel {
    
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

    /* PDF prename */
    TString m_prename = "";

    /* Category name (for binned fits) */
    std::string m_cat_name;

public:
    /**
    * Constructor
    * @param settings fit config
    * @param vars Variables class
    * @param Data Data class
    * @param debug boolean
    */
    FitModel(Settings settings, Variables* vars, Data* data, bool debug = false, std::string cat_name = ""){
        m_settings = settings;
        m_vars = vars;
        m_data = data;
        m_debug = debug;
        if (m_settings.key_exists("prename")) m_prename = m_settings.getT("prename");
        m_log = Log("FitModel");
        m_cat_name = cat_name;
    }

    /**
     * Structure containing details of a component
    */
    struct Component {
        std::string name;
        RooAbsPdf* shape;
        RooAbsReal* yield;
    };

    /* List of components */
    std::map<std::string, Component> components;

    /**
     * Function to read list of components and create empty structures
    */
    void ReadComponents(){ 
        auto component_list = TextFileUtils::ReadList(m_settings.get("components"));
        for (auto c: component_list){
            if (m_debug) m_log.info("Adding component: " + c);
            components[c] = Component();
            components[c].name = c;
        }
        return;
    }

    /**
     * Create the PDF
    */
    std::unique_ptr<RooAddPdf> pdf;
    RooArgList component_pdfs;
    RooArgList component_yields;
    void MakePDF(){
        AddSignal();
        for (auto c: components){
            if (c.first == "signal") continue;
            else if (c.first == "kpi_vs_comb") AddKpiVsComb();
            else if (c.first == "comb_vs_tag") AddCombVsTag();
            else if (c.first == "flat_qqbar") AddComb();
            else if (c.first == "kpi_vs_kpipi0") AddKPiVsKPiPi0();
            else if (c.first == "correlated_qqbar") AddCorrelatedQQBar();
            else if (c.first == "dstpdstm") AddDSTpDSTm();
            else if (c.first == "dstpdm") AddDSTpDm();
            else if (c.first == "kpi_vs_kpi") AddKPiVsKPi();
        }
        pdf = std::make_unique<RooAddPdf>(m_prename + "pdf", "", component_pdfs, component_yields);
        if (m_debug) m_log.success("PDF made!");
        return;
    }

    /**
     * Add the components to the PDF
    */
    void AddSignal();
    void AddKpiVsComb();
    void AddCombVsTag();
    void AddComb();
    void AddKPiVsKPiPi0();
    void AddCorrelatedQQBar();
    void AddDSTpDSTm();
    void AddDSTpDm();
    void AddKPiVsKPi();

    /**
     * Parameters shared between DP bins
    */
    // Signal shape
    RooAbsPdf* signal_shape = nullptr;
    RooAbsPdf* kpi_signal_shape = nullptr;
    RooAbsPdf* tag_signal_shape = nullptr;

    // Comb. functions
    RooAbsPdf* kpi_vs_comb_shape = nullptr;
    RooAbsPdf* comb_vs_tag_shape = nullptr;

    // Correlated mean
    RooAbsPdf* qqbar_shape = nullptr;

    // DSTpDm shape
    RooAbsPdf* dstpdm_shape = nullptr;

};

#endif // FITMODEL_H
