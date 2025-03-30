#include "Settings.hpp"
#include "Log.hpp"
#include "Variables.hpp"
#include "Data.hpp"
#include "BinnedFitModel.hpp"
#include "BinnedFitter.hpp"
#include "Plotter.hpp"

/**
 * Function to print a welcome statement
 * @param Log logging class
 * @param tag TString of the tag being fitted
 * @param prod TString of the production mechanism
*/
void PrintWelcome(Log log, TString tag, TString prod){
    log.empty();
    log.success("Welcome to the KSpipi tag fit");
    log.success("Tag: " + tag);
    log.success("Production mechanism: " + prod);
    log.empty();
    return;
}


int main(int argc , char* argv[]){

    // ===================================
    // Read in the fit settings
    // ===================================
    std::string settings_file = argv[1];
    Settings* set = new Settings(settings_file);
    set->read();
    bool m_debug = set->getB("debug");
    TString tag = set->getT("tag");
    TString prod = set->getT("prod");
    TString prename = set->getT("prename");

    // ===================================
    // Print welcome
    // ===================================
    Log log("cptags");
    PrintWelcome(log, tag, prod);

    // ===================================
    // Load variables
    // ===================================
    Variables* vars = new Variables(*set);

    // ===================================
    // Load datasets
    // ===================================
    Data* dt = new Data(*set, vars, true, m_debug);

    // ===================================
    // Load the fit model
    // ===================================
    BinnedFitModel* fm = new BinnedFitModel(*set, vars, dt, m_debug);
    fm->MakePDF();

    // ===================================
    // Run the fit
    // ===================================
    BinnedFitter* ft = new BinnedFitter(*set, vars, fm, dt, m_debug);
    ft->RunFit();
    ft->SaveOutput();

    // ===================================
    // Plot the fit and scatter
    // ===================================
    for (auto category: fm->category_models){
        set->update_value("prename", category.first);
        Plotter* pt = new Plotter(*set, vars, dt, category.second, m_debug);
        pt->Plot(false, false, set->getB("pulls"), category.first);
        pt->Plot(false, true, set->getB("pulls"), category.first);
        //RooDataSet* toy_data = fm->pdf->generate(RooArgList(*vars->m_kpi, *vars->m_tag), 5 * dt->data->numEntries());
        //pt->ScatterPlot(*dt->data, false);
        //pt->ScatterPlot(*toy_data, true);
    }
}