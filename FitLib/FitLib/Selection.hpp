#ifndef SELECTION_H
#define SELECTION_H

#include "Settings.hpp"
#include "Log.hpp"

#include "TString.h"

namespace SelectionConstants {

    // DD from D*D & D*D*
    const double DD_EMISS_MIN = -0.03;
    const double DD_EMISS_MAX = 0.06;
    const double OTHER_EMISS_MIN = 0.07;
    const double OTHER_EMISS_MAX = 1.;

    // D*D from D*D*
    const double DSTD_MREC_MIN = -0.05;
    const double DSTD_MREC_MAX = 0.05;
    const double DSTDST_MREC_MIN = 0.07;
    const double DSTDST_MREC_MAX = 1.;

    // DDg from DDpi
    const double DSTDG_MMISS_MIN = -0.02;
    const double DSTDG_MMISS_MAX = 0.008;
    const double DSTDPI_MMISS_MIN = 0.011;
    const double DSTDPI_MMISS_MAX = 1.;

    // DST0DST0 from DSTpDSTm
    const double DSTDST_SLOWPI_MIN = 0.1;
    const double DSTDST_SLOWPI_MAX = 5.;

    // Good DSTDST
    const double DSTDST_DDG_MREC_MIN = 1.99;
    const double DSTDST_DDG_MREC_MAX = 2.04;
    const double DSTDST_DDG_DELTAM_MIN = -0.02;
    const double DSTDST_DDG_DELTAM_MAX = 0.02;

    // DSTDST even
    const double DSTDSTE_DDG_MMISS_MIN = 0.01;
    const double DSTDSTE_DDG_MMISS_MAX = 10.;

    // DSTDST odd
    const double DSTDSTO_DDG_MMISS_MIN = -0.02;
    const double DSTDSTO_DDG_MMISS_MAX = 0.01;

    // Bad DST region box
    const double BAD_DST_DDG_DELTAM_MIN = -0.08;
    const double BAD_DST_DDG_DELTAM_MAX = 0.03;
    const double BAD_DST_DDG_MREC_MIN = 2.0;
    const double BAD_DST_DDG_MREC_MAX = 2.05;
    const double BAD_DST_GRAD = (BAD_DST_DDG_MREC_MAX-BAD_DST_DDG_MREC_MIN) / (BAD_DST_DDG_DELTAM_MAX - BAD_DST_DDG_DELTAM_MIN);
    const double BAD_DST_YINT = BAD_DST_DDG_MREC_MAX - (BAD_DST_GRAD * BAD_DST_DDG_DELTAM_MAX);

    // Tag cuts
    const double KSPI0_FDS_MIN = 2.;
    const double KSPI0_FDS_MAX = 1e5;

    const double PIPIPI0_FDS_MIN = -1e5;
    const double PIPIPI0_FDS_MAX = 2.;

    const double KSPIPI_FDS_MIN = 2.;
    const double KSPIPI_FDS_MAX = 1e5;
    const double KSPIPI_KS_MINV_MIN = 0.485;
    const double KSPIPI_KS_MINV_MAX = 0.510;

}

class Selection {

public:

    Selection(Settings settings){
        _settings = settings;
        _tag = _settings.getT("tag");
        _prod = _settings.getT("prod");
    };

    Selection(TString tag, TString prod){
        _tag = tag;
        _prod = prod;
    }

    Selection(){}

    ~Selection(){}

    TString TagCut(TString tag){
        std::map<TString, TString> cuts = {
            {"KSPi0", "(" + _tag + "_ks_flightSig>" + Form("%f", SelectionConstants::KSPI0_FDS_MIN) + ") & (" + _tag + "_ks_flightSig<" + Form("%f", SelectionConstants::KSPI0_FDS_MAX) + ")" },
            {"PiPiPi0", "(" + _tag + "_ks_flightSig>" + Form("%f", SelectionConstants::PIPIPI0_FDS_MIN) + ") & (" + _tag + "_ks_flightSig<" + Form("%f", SelectionConstants::PIPIPI0_FDS_MAX) + ")" },
            {
              "KSPiPi", "(" + _tag + "_ks_flightSig>" + Form("%f", SelectionConstants::KSPIPI_FDS_MIN) + ") & (" + _tag + "_ks_flightSig<" + Form("%f", SelectionConstants::KSPIPI_FDS_MAX) + ") & " +
                        "(" + _tag + "_ks_mInv>" + Form("%f", SelectionConstants::KSPIPI_KS_MINV_MIN) + ") & (" + _tag + "_ks_mInv<" + Form("%f", SelectionConstants::KSPIPI_KS_MINV_MAX) + ") & " +
                        "(" + _tag + "_vs_KPi_bin!=0)"
            },
            {"KK", ""},
            {"PiPi", ""},
            {"KPi", "(KPi_vs_KPi_SignComp==0)"}
        };
        return cuts[tag];
    }

    TString ProdCut(TString prod, bool smeared=false){

        TString descriptor = _tag;
        if (smeared){ descriptor = "Smeared_" + _tag; }

        std::map<TString, TString> cuts = {
            {"D0D0", "(" + descriptor + "_vs_KPi_EMiss>" + Form("%f", SelectionConstants::DD_EMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_EMiss<" + Form("%f", SelectionConstants::DD_EMISS_MAX) + ")" },
            {
                "DST0D0_g", "(" + descriptor + "_vs_KPi_EMiss>" + Form("%f", SelectionConstants::OTHER_EMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_EMiss<" + Form("%f", SelectionConstants::OTHER_EMISS_MAX) + ") & " +
                            "(" + descriptor + "_vs_KPi_BestMRec>" + Form("%f", SelectionConstants::DSTD_MREC_MIN) + ") & " + "(" + descriptor + "_vs_KPi_BestMRec<" + Form("%f", SelectionConstants::DSTD_MREC_MAX) + ") & " +
                            "(" + descriptor + "_vs_KPi_MMiss2>" + Form("%f", SelectionConstants::DSTDG_MMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_MMiss2<" + Form("%f", SelectionConstants::DSTDG_MMISS_MAX) + ")"
            },
            {
                "DST0D0_pi", "(" + descriptor + "_vs_KPi_EMiss>" + Form("%f", SelectionConstants::OTHER_EMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_EMiss<" + Form("%f", SelectionConstants::OTHER_EMISS_MAX) + ") & " +
                            "(" + descriptor + "_vs_KPi_BestMRec>" + Form("%f", SelectionConstants::DSTD_MREC_MIN) + ") & " + "(" + descriptor + "_vs_KPi_BestMRec<" + Form("%f", SelectionConstants::DSTD_MREC_MAX) + ") & " +
                            "(" + descriptor + "_vs_KPi_MMiss2>" + Form("%f", SelectionConstants::DSTDPI_MMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_MMiss2<" + Form("%f", SelectionConstants::DSTDPI_MMISS_MAX) + ")"
            },
            {
              "DST0DST0_even", "(" + descriptor + "_vs_KPi_EMiss>" + Form("%f", SelectionConstants::OTHER_EMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_EMiss<" + Form("%f", SelectionConstants::OTHER_EMISS_MAX) + ") & " +
                               "(" + descriptor + "_vs_KPi_BestMRec>" + Form("%f", SelectionConstants::DSTDST_MREC_MIN) + ") & " + "(" + descriptor + "_vs_KPi_BestMRec<" + Form("%f", SelectionConstants::DSTDST_MREC_MAX) + ") & " +
                               "(slowestPion_p>" + Form("%f", SelectionConstants::DSTDST_SLOWPI_MIN) + ") & " + "(slowestPion_p<" + Form("%f", SelectionConstants::DSTDST_SLOWPI_MAX) + ") & " +
                               "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_DSTMRec>" + Form("%f", SelectionConstants::DSTDST_DDG_MREC_MIN) + ") & " + "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_DSTMRec<" + Form("%f", SelectionConstants::DSTDST_DDG_MREC_MAX) + ") & " +
                               "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_deltaM>" + Form("%f", SelectionConstants::DSTDST_DDG_DELTAM_MIN) + ") & " + "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_deltaM<" + Form("%f", SelectionConstants::DSTDST_DDG_DELTAM_MAX) + ") & " +
                               "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_DSTMRec<" + Form("%f", SelectionConstants::BAD_DST_GRAD) + "*" + descriptor + "_vs_KPi_bestDSTDSTPhoton_deltaM + " + Form("%f", SelectionConstants::BAD_DST_YINT) + ") & " +
                               "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_MM2>" + Form("%f", SelectionConstants::DSTDSTE_DDG_MMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_MM2<" + Form("%f", SelectionConstants::DSTDSTE_DDG_MMISS_MAX) + ")"
            },
            {
              "DST0DST0_odd", "(" + descriptor + "_vs_KPi_EMiss>" + Form("%f", SelectionConstants::OTHER_EMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_EMiss<" + Form("%f", SelectionConstants::OTHER_EMISS_MAX) + ") & " +
                               "(" + descriptor + "_vs_KPi_BestMRec>" + Form("%f", SelectionConstants::DSTDST_MREC_MIN) + ") & " + "(" + descriptor + "_vs_KPi_BestMRec<" + Form("%f", SelectionConstants::DSTDST_MREC_MAX) + ") & " +
                               "(slowestPion_p>" + Form("%f", SelectionConstants::DSTDST_SLOWPI_MIN) + ") & " + "(slowestPion_p<" + Form("%f", SelectionConstants::DSTDST_SLOWPI_MAX) + ") & " +
                               "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_DSTMRec>" + Form("%f", SelectionConstants::DSTDST_DDG_MREC_MIN) + ") & " + "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_DSTMRec<" + Form("%f", SelectionConstants::DSTDST_DDG_MREC_MAX) + ") & " +
                               "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_deltaM>" + Form("%f", SelectionConstants::DSTDST_DDG_DELTAM_MIN) + ") & " + "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_deltaM<" + Form("%f", SelectionConstants::DSTDST_DDG_DELTAM_MAX) + ") & " +
                               "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_DSTMRec<" + Form("%f", SelectionConstants::BAD_DST_GRAD) + "*" + descriptor + "_vs_KPi_bestDSTDSTPhoton_deltaM + " + Form("%f", SelectionConstants::BAD_DST_YINT) + ") & " +
                               "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_MM2>" + Form("%f", SelectionConstants::DSTDSTO_DDG_MMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_MM2<" + Form("%f", SelectionConstants::DSTDSTO_DDG_MMISS_MAX) + ")"
            }
        };

        return cuts[prod];
    }


    TString GetTrueBinCut(int bin){
        std::string binCut = "( (KPi_KCharge>0 & KSPiPi_vs_KPi_true_bin == " + std::to_string(bin) + ") | (KPi_KCharge<0 & KSPiPi_vs_KPi_true_bin == " + std::to_string(-1*bin) + ") )";
        return binCut.c_str();
    }

    TString GetBinCut(int bin){
        std::string binCut = "( (KPi_KCharge>0 & KSPiPi_vs_KPi_bin == " + std::to_string(bin) + ") | (KPi_KCharge<0 & KSPiPi_vs_KPi_bin == " + std::to_string(-1*bin) + ") )";
        return binCut.c_str();
    }

private:
    Settings _settings;
    TString _tag;
    TString _prod;

};

#endif //  Selection_H
