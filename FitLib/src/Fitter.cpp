#include "Fitter.hpp"

void Fitter::RunFit(){

    // Initial fit
    if (m_debug) m_log.info("Running the first fit ...");
    auto r = m_fm->pdf->fitTo(*m_dt->data, RooFit::Save(1), RooFit::Extended(1));
    r->Print("v");

    // Second fit
    if (CheckYields() | CheckBkgSlopes()){
        if (m_debug) m_log.info("Running the second fit ...");
        m_result = m_fm->pdf->fitTo(*m_dt->data, RooFit::Save(1), RooFit::Extended(1));
        m_result->Print("v");
    }
    else m_result = r;
    m_log.success("Fit complete!");
    return;
}


bool Fitter::CheckYields(){    
    if (m_debug) m_log.info("Checking for zero yields");
    bool second_fit = false;
    for (auto c: m_fm->components){
        RooRealVar* yield = (RooRealVar*) c.second.yield;
        if (yield->getVal() < 1){

            // Fix yield
            m_log.warning("Setting " + c.first + " yield to zero");
            second_fit = true;
            yield->setVal(0);
            yield->setConstant(true);

            // Fix associated parameters
            RooArgSet* component_pars = c.second.shape->getParameters(RooArgSet(*m_vars->m_kpi, *m_vars->m_tag));
            TIterator* iter(component_pars->createIterator());
            for (TObject *a = iter->Next(); a != 0; a = iter->Next()) {
                RooRealVar *p = (RooRealVar*) a;  
                TString par_name = p->GetName();
                if (par_name.Contains("smear")) continue;
                m_log.warning("Fixing " + par_name);
                p->setConstant(true);            
            }
        }
    }
    return second_fit;
}



bool Fitter::CheckBkgSlopes(){
    if (m_debug) m_log.info("Checking if bkg slopes are at limits");
    bool second_fit = false;
    RooArgSet* pdf_pars = m_fm->pdf->getParameters(RooArgSet(*m_vars->m_kpi, *m_vars->m_tag));
    TIterator* iter(pdf_pars->createIterator());
    for (TObject *a = iter->Next(); a != 0; a = iter->Next()) {
        RooRealVar *p = (RooRealVar*) a;
        TString name = p->GetName();
        if (name.Contains("exponent")){
            double maxRatio = p->getVal() / p->getMax() ;
            double minRatio = p->getVal() / p->getMin() ;
            if (maxRatio > 0.99 | minRatio > 0.99){
                if (m_debug){ m_log.debug(name + " was found to be at the limit. Fixing it."); }
                p->setConstant(true);
                second_fit = true;
            }
        }
    }
    return second_fit;
}


void Fitter::SaveOutput(){

    // Open the outfile
    std::string prename = "";
    if (m_settings.key_exists("prename")) prename = m_settings.get("prename");
    std::string outfile_name = "output/" + m_settings.get("tag") + "/" + m_settings.get("prod") + "/" + prename + "fit_results.txt";
    m_log.info("Writing output to " + outfile_name);
    std::ofstream m_outfile;
    m_outfile.open(outfile_name);

    // Write fit status
    m_outfile << "status " << m_result->status() << std::endl;
    m_outfile << "covQual " << m_result->covQual() << std::endl;
    m_outfile << "minNLL " << m_result->minNll() << std::endl;
    m_outfile << "edm " << m_result->edm() << std::endl;

    // Write floating pars
    m_outfile << "\n* Floated parameters:\n";
    RooArgList fp = m_result->floatParsFinal();
    for (int i=0; i < fp.getSize(); i++){
        m_outfile << fp.at(i)->GetName() << " "
        << ((RooRealVar*)fp.at(i))->getVal() << std::endl;
    }
    m_outfile << "\n* Floated parameter errors:\n";
    for (int i=0; i < fp.getSize(); i++){
        m_outfile << fp.at(i)->GetName() << "_errLo "
        << ((RooRealVar*)fp.at(i))->getErrorLo() << std::endl;
        m_outfile << fp.at(i)->GetName() << "_errHi "
        << ((RooRealVar*)fp.at(i))->getErrorHi() << std::endl;
    }

    // Write fixed pars
    m_outfile << "\n* Constant parameters\n";
    RooArgList cp = m_result->constPars();
    for (int i=0; i < cp.getSize(); i++){
        m_outfile << cp.at(i)->GetName() << " "
        << ((RooRealVar*)cp.at(i))->getVal() << std::endl;
    }

    m_outfile.close();

    return;
}