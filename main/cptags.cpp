#include "Settings.hpp"
#include "Log.hpp"
#include "Variables.hpp"
#include "Data.hpp"
#include "FitModel.hpp"
#include "Fitter.hpp"
#include "Plotter.hpp"

/**
 * Function to print a welcome statement
 * @param Log logging class
 * @param tag TString of the tag being fitted
 * @param prod TString of the production mechanism
*/
void PrintWelcome(Log log, TString tag, TString prod){
    log.empty();
    log.success("Welcome to the CP tag fit");
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
    FitModel* fm = new FitModel(*set, vars, dt, m_debug);
    fm->ReadComponents();
    fm->MakePDF();

    // ===================================
    // Run the fit
    // ===================================
    Fitter* ft = new Fitter(*set, vars, fm, dt, m_debug);
    ft->RunFit();
    ft->SaveOutput();

    // ===================================
    // Plot the fit and scatter
    // ===================================
    Plotter* pt = new Plotter(*set, vars, dt, fm, m_debug);
    pt->Plot(false, false, set->getB("pulls"));
    pt->Plot(false, true, set->getB("pulls"));
    RooDataSet* toy_data = fm->pdf->generate(RooArgList(*vars->m_kpi, *vars->m_tag), 5 * dt->data->numEntries());
    pt->ScatterPlot(*dt->data, false);
    pt->ScatterPlot(*toy_data, true);

}

