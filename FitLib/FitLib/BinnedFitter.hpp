#ifndef BINNEDFITTER_H
#define BINNEDFITTER_H

#include "Settings.hpp"
#include "Variables.hpp"
#include "BinnedFitModel.hpp"
#include "Log.hpp"

#include "RooFitResult.h"

#include <map>
#include <vector>
#include <string>
#include <memory>

/**
 * Class to run the fits and produce output.
*/
class BinnedFitter {

private:
    /** Fit configuration class */
    Settings m_settings;

    /** Variables class */
    Variables* m_vars;

    /** Fit model class */
    BinnedFitModel* m_fm;

    /** Data class */
    Data* m_dt;

    /** Debug flag */
    bool m_debug;

    /** Logging class */
    Log m_log;

    /** RooFitResult */
    RooFitResult* m_result;

public:

    /**
    Constructor function, sets all of the private member objects
    * @param settings fit config
    * @param vars Class storing all of the variables
    * @param fm Class containing the fit model
    * @param dt Class containing the datasets
    */
    BinnedFitter(Settings settings, Variables* vars, BinnedFitModel* fm, Data* dt, bool debug = false){
        m_settings  = settings;
        m_vars = vars;
        m_fm = fm;
        m_dt = dt;
        m_debug = debug;
        m_log = Log("BinnedFitter");
    }

    /* Run the fit */
    void RunFit();

    /* Check for zero yields */
    bool CheckYields();

    /* Check the bkg slopes */
    bool CheckBkgSlopes();

    /* Write output to file */
    void SaveOutput();

    /* Fix all parameters except signal */
    void FixAllPars();

};

#endif //  BinnedFitter_H
