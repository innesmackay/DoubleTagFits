#ifndef PLOTTER_H
#define PLOTTER_H

#include "Settings.hpp"
#include "Variables.hpp"
#include "Data.hpp"
#include "FitModel.hpp"
#include "Log.hpp"

#include "RooPlot.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TROOT.h"

#include <vector>

class Plotter {

private:
    /** Fit configuration class */
    Settings m_settings;

    /** Variables class */
    Variables* m_vars;

    /** Fit model class */
    FitModel* m_fm;

    /** Data class */
    Data* m_dt;

    /** Debug flag */
    bool m_debug;

    /** Logging class */
    Log m_log;

    /** Plotting colours */
    std::vector<Color_t> m_colours = {kAzure-3, kTeal-7, kOrange+7, kMagenta-6, kYellow-6, kPink-8, kAzure+9, kSpring+4, kGray};
    std::map<TString, Color_t> m_component_colours =  { {"signal", kAzure+5},
                                                        {"kpi_vs_comb", kOrange-3},
                                                        {"comb_vs_tag", kGreen-6},
                                                        {"kpi_vs_kpi", kPink-8},
                                                        {"kpi_vs_kpipi0", kViolet+6},
                                                        {"flat_qqbar", kYellow-6},
                                                        {"correlated_qqbar", kGray},
                                                        {"dstpdm", kGreen},
                                                        {"dstpdstm", kGreen}
                                                      };

    /** Latex labels */
    std::map<TString, TString> m_particle_labels = { {"KK", "K^{+}K^{-}"},
                                                     {"PiPi", "#pi^{+}#pi^{-}"},
                                                     {"PiPiPi0", "#pi^{+}#pi^{-}#pi^{0}"},
                                                     {"KSPi0", "K_{S}^{0}#pi^{0}"},
                                                     {"KSPiPi", "K_{S}^{0}#pi^{+}#pi^{-}"},
                                                     {"KPi", "K^{-}#pi^{+}"}
                                                    };

public:
    /**
    * Constructor function
    * @param settings Fit config
    * @param vars Variable class
    * @param dt Data class
    * @param fm Fit model class
    * @param debug boolean
    */
    Plotter(Settings settings, Variables* vars, Data* dt, FitModel* fm, bool debug = false){
        m_settings = settings;
        m_vars = vars;
        m_dt = dt;
        m_fm = fm;
        m_debug = debug;
        m_log = Log("Plotter");
        gROOT->ProcessLine(".x /home/mackay/DoubleTagFits/style.C");
    }

    // void ScatterPlot(TString output, bool toy = false, TString ylabel = "");

    /** Plot a D decay projection */
    void PlotProjection(RooPlot* frame, bool filled, int nbins, std::string cat_name = "");

    /** Plot the pulls */
    RooPlot* PlotPulls(RooPlot* f, RooRealVar* var);

    /** Make a filled composite plot */
    void Plot(bool log, bool filled, bool pulls, std::string cat_name = "");

    /** Mass scatter plot */
    void ScatterPlot(RooDataSet d, bool toy);

    /** Get component name for legend */
    TString GetComponentName(TString component_name){
        TString plot_name;
        if (component_name.Contains("signal")){ plot_name = "signal"; }
        if (component_name.Contains("comb")){ plot_name = "comb"; }
        if (component_name.Contains("kpi_vs_comb")){ plot_name = "kpi_vs_comb"; }
        if (component_name.Contains("comb_vs_tag")){ plot_name = "comb_vs_tag"; }
        if (component_name.Contains("kpi_vs_kpi")){ plot_name = "kpi_vs_kpi"; }
        if (component_name.Contains("kpi_vs_kpipi0")){ plot_name = "kpi_vs_kpipi0"; }
        if (component_name.Contains("kpi_vs_kspi0")){ plot_name = "kpi_vs_kspi0"; }
        if (component_name.Contains("flat_qqbar")){ plot_name = "flat_qqbar"; }
        if (component_name.Contains("correlated_qqbar")){ plot_name = "correlated_qqbar"; }
        if (component_name.Contains("dstpdm")){ plot_name = "dstpdm"; }
        if (component_name.Contains("dstpdstm")){ plot_name = "dstpdstm"; }
        return plot_name;
    }

    /** Get the legend label */
    TString GetPlotLabel(TString component){
        TString tag = m_settings.getT("tag");
        std::map<TString, TString> latex_component_names = {
            {"signal", "K#pi vs. " + m_particle_labels[tag]},
            {"kpi_vs_comb", "K#pi vs. Comb."},
            {"comb_vs_tag", "Comb. vs. " + m_particle_labels[tag]},
            {"kpi_vs_kpipi0", "K#pi vs. K#pi#pi^{0}"},
            {"kpi_vs_kpi", "K#pi vs. K#pi"},
            {"flat_qqbar", "Combinatorial"},
            {"correlated_qqbar", "Correlated q#bar{q}"},
            {"dstpdm", "D^{*+}D^{-}"},
            {"dstpdstm", "D^{*+}D^{*-}"},
        };        
        return latex_component_names[component];
    }  

};
#endif // Plotter_H





