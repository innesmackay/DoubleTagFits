#include "Plotter.hpp"

#include "TFile.h"
#include "TLine.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TPad.h"

#include "RooHist.h"

using namespace RooFit;

void Plotter::ScatterPlot(RooDataSet d, bool toy){

    // Create output filename
    TString prename = "";
    if (m_settings.key_exists("prename")) prename = m_settings.getT("prename");
    TString output = "output/" + m_settings.getT("tag") + "/" + m_settings.getT("prod") + "/" + prename + "scatter";
    if (toy) output += "_toy";

    // Set bins    
    int nbins = 40;
    if (m_settings.key_exists("nbins")){ nbins = m_settings.getI("nbins"); }

    // Styling
    TH1* hist = d.createHistogram("hist", *m_vars->m_kpi, RooFit::Binning(nbins), RooFit::YVar(*m_vars->m_tag, RooFit::Binning(nbins))) ;
    hist->SetStats(0);
    hist->SetTitle("");
    hist->SetMarkerStyle(20);
    hist->SetMarkerSize(0.5);
    if (toy){
        hist->SetMarkerColor(14);
        hist->SetMarkerSize(0.3);
    }

    hist->GetYaxis()->SetTitle("m(" + m_particle_labels[m_settings.getT("tag")] + ") [GeV/c^{2}]");
    hist->GetXaxis()->SetTitle("m(K^{-}#pi^{+}) [GeV/c^{2}]");

    // Plot
    TCanvas* canvas = new TCanvas("canvas", "", 800, 600);
    TPad* plot_pad = new TPad("plot_pad", "", 0.05, 0.17, 0.97, 0.98);
    plot_pad->Draw();
    plot_pad->cd();
    hist->Draw("SCAT");
    canvas->Print(output + ".pdf");

    // Save as root file
    TFile root_file(output + ".root", "RECREATE");
    hist->Write("hist");
    root_file.Close();

    return;

}

RooPlot* Plotter::PlotPulls(RooPlot* f, RooRealVar* var){

  RooHist* pulls = f->pullHist();
  RooPlot* pf = var->frame();
  pf->addPlotable(pulls, "P");
  pf->SetMinimum(-5.5);
  pf->SetMaximum(5.5);

  TAxis* x = pf->GetXaxis(); TAxis* y = pf->GetYaxis();
  y->SetTitleOffset(0.2);
  y->SetTitleSize(0.2);
  y->SetLabelSize(0.12);
  y->SetNdivisions(10);
  x->SetLabelSize(0.);
  pf->SetXTitle("");
  pf->SetYTitle("Pull");
  pf->SetTitle("");
  pf->Draw();

  TLine* line = new TLine(var->getMin(),0,var->getMax(),0);
  line->SetLineStyle(kDashed);
  line->Draw();

  return pf;
}


void Plotter::PlotProjection(RooPlot* frame, bool filled, int nbins){

    // Plot data and PDF
    m_dt->data->plotOn(frame, RooFit::LineColor(kBlack), RooFit::Name("Data"), RooFit::Binning(nbins));
    m_fm->pdf->plotOn(frame, RooFit::LineColor(kRed), RooFit::Name("Fit"), RooFit::LineWidth(1.));

    // Bkgs
    int i = 0;
    for (auto c: m_fm->components){
        if (c.first.find("signal") != std::string::npos) continue;
        TString component_name = c.second.name.c_str();
        if (m_debug) m_log.debug("Plotting " + component_name);
        if (filled){
            RooArgSet already_plotted;
            int j = 0;
            for (auto sc: m_fm->components){             
                if (sc.first.find("signal") != std::string::npos) continue;
                if (j>=i) already_plotted.add( *sc.second.shape );             
                j += 1;
            }
            m_fm->pdf->plotOn(frame,
                          RooFit::LineColor(m_component_colours[GetComponentName(component_name)]),
                          RooFit::Name(GetComponentName(component_name)),
                          RooFit::Components(already_plotted),
                          RooFit::LineStyle(kSolid),
                          RooFit::FillStyle(1001),
                          RooFit::FillColor(m_component_colours[GetComponentName(component_name)]),
                          RooFit::DrawOption("FL"),
                          RooFit::LineWidth(2)
                          );
        }
        else  m_fm->pdf->plotOn(frame,
                           RooFit::LineColor(m_component_colours[GetComponentName(component_name)]),
                           RooFit::Name(GetComponentName(component_name)),
                           RooFit::Components(*c.second.shape),
                           RooFit::LineStyle(kDashed),
                           RooFit::DrawOption("L"),
                           RooFit::LineWidth(1)
                           );
        i += 1;
    }

    // Plot signal
    m_fm->pdf->plotOn(frame,
                      LineColor(m_component_colours[GetComponentName("signal")]),
                      Name(GetComponentName("signal")),
                      Components(*m_fm->components["signal"].shape),
                      LineStyle(kDashed),
                      DrawOption("L"),
                      LineWidth(1)
                      );

    // Data/PDF again
    m_dt->data->plotOn(frame, RooFit::LineColor(kBlack), RooFit::Name("Data"), RooFit::Binning(nbins));
    m_fm->pdf->plotOn(frame, RooFit::LineColor(kRed), RooFit::Name("Fit"), RooFit::LineWidth(1));
    m_dt->data->plotOn(frame, RooFit::LineColor(kBlack), RooFit::Name("Data"), RooFit::Binning(nbins));

    return;

}

void Plotter::Plot(bool log, bool filled, bool pulls){

    // Create output filename
    TString prename = "";
    if (m_settings.key_exists("prename")) prename = m_settings.getT("prename");
    TString output = "output/" + m_settings.getT("tag") + "/" + m_settings.getT("prod") + "/" + prename + "fit";
    if (filled) output += "_filled";
    if (log) output += "_log";
    if (pulls) output += "_pulls";

    // Set bins    
    int nbins = 40;
    if (m_settings.key_exists("nbins")){ nbins = m_settings.getI("nbins"); }

    // Setup plots
    TCanvas canvas("canvas", "", 10, 44, 1200, 800);
    RooPlot* kpi_frame = m_vars->m_kpi->frame();    
    TPad kpi_plot_pad("kpi_plot_pad", "", 0.02, 0.2, 0.47, 0.98);
    TPad kpi_pull_pad("kpi_pull_pad", "", 0.02, 0.02, 0.47, 0.2);
    RooPlot* tag_frame = m_vars->m_tag->frame();
    TPad tag_plot_pad("tag_plot_pad", "", 0.51, 0.2, 0.98, 0.98);
    TPad tag_pull_pad("tag_pull_pad", "", 0.51, 0.02, 0.98, 0.2);
    if (!pulls){
        kpi_plot_pad.SetPad(0.05, 0.17, 0.47, 0.98);
        tag_plot_pad.SetPad(0.51, 0.17, 0.93, 0.98);
    }
    kpi_plot_pad.Draw();
    kpi_pull_pad.Draw();
    tag_plot_pad.Draw();
    tag_pull_pad.Draw();
    if (log){
        kpi_plot_pad.SetLogy();
        tag_plot_pad.SetLogy();
    }
    tag_frame->GetXaxis()->SetTitle("m(" + m_particle_labels[m_settings.getT("tag")] + ") (GeV/c^{2})");
    kpi_frame->GetXaxis()->SetTitle("m(K^{-}#pi^{+}) (GeV/c^{2})");
    double kpi_bin_size = (m_vars->m_kpi->getMax() - m_vars->m_kpi->getMin()) / nbins;
    double tag_bin_size = (m_vars->m_tag->getMax() - m_vars->m_tag->getMin()) / nbins;
    TString kpi_ylabel = ("Candidates / " + std::to_string(kpi_bin_size).substr(0, 5) + " GeV/c^{2}").c_str();
    TString tag_ylabel = ("Candidates / " + std::to_string(tag_bin_size).substr(0, 5) + " GeV/c^{2}").c_str();
    kpi_frame->GetYaxis()->SetTitle(kpi_ylabel);
    tag_frame->GetYaxis()->SetTitle(tag_ylabel);

    // Plot projections
    m_log.info("Plotting data and PDF on Kpi frame");
    kpi_plot_pad.cd();
    PlotProjection(kpi_frame, filled, nbins);
    kpi_frame->Draw();
    tag_plot_pad.cd();
    PlotProjection(tag_frame, filled, nbins);
    tag_frame->Draw();

    // Plot pulls
    RooPlot* kpi_pull_frame;
    RooPlot* tag_pull_frame;
    if (pulls){
        kpi_pull_pad.cd();
        kpi_pull_frame = PlotPulls(kpi_frame, m_vars->m_kpi);
        tag_pull_pad.cd();
        tag_pull_frame = PlotPulls(tag_frame, m_vars->m_tag);
    }

    // Make legend
    kpi_plot_pad.cd();
    TLegend* legend = new TLegend(0.55, 0.5, 0.92, 0.9);
    legend->AddEntry("Data", "Data", "ep");
    legend->AddEntry("Fit", "Fit", "l");
    for (auto c: m_fm->components){
        if (filled) legend->AddEntry(GetComponentName(c.first.c_str()), GetPlotLabel(GetComponentName(c.first.c_str())), "lf");
        else legend->AddEntry(GetComponentName(c.first.c_str()), GetPlotLabel(GetComponentName(c.first.c_str())), "l");
    }
    legend->Draw();

    // Save
    canvas.Draw();
    canvas.Print(output + ".pdf");
    TFile root_file(output + ".root", "RECREATE");
    kpi_frame->Write("kpi_frame");
    tag_frame->Write("tag_frame");
    if (pulls){
        tag_pull_frame->Write("tag_pull_frame");
        kpi_pull_frame->Write("kpi_pull_frame");
    }
    root_file.Close();

    return;

}