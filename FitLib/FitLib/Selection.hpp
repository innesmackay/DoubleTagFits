#ifndef SELECTION_H
#define SELECTION_H

#include "Log.hpp"

#include "TString.h"

namespace Selection {

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

    inline TString TagCut(TString tag){
        std::map<TString, TString> cuts = {
            {"KSPi0", "(" + tag + "_ks_flightSig>" + Form("%f", KSPI0_FDS_MIN) + ") & (" + tag + "_ks_flightSig<" + Form("%f", KSPI0_FDS_MAX) + ")" },
            {"PiPiPi0", "(" + tag + "_ks_flightSig>" + Form("%f", PIPIPI0_FDS_MIN) + ") & (" + tag + "_ks_flightSig<" + Form("%f", PIPIPI0_FDS_MAX) + ")" },
            {
              "KSPiPi", "(" + tag + "_ks_flightSig>" + Form("%f", KSPIPI_FDS_MIN) + ") & (" + tag + "_ks_flightSig<" + Form("%f", KSPIPI_FDS_MAX) + ") & " +
                        "(" + tag + "_ks_mInv>" + Form("%f", KSPIPI_KS_MINV_MIN) + ") & (" + tag + "_ks_mInv<" + Form("%f", KSPIPI_KS_MINV_MAX) + ") & " +
                        "(" + tag + "_vs_KPi_bin!=0)"
            },
            {"KK", ""},
            {"PiPi", ""},
            {"KPi", "(KPi_vs_KPi_SignComp==0)"}
        };
        return cuts[tag];
    }

    inline TString ProdCut(TString prod, TString descriptor){

        std::map<TString, TString> cuts = {
            {"D0D0", "(" + descriptor + "_vs_KPi_EMiss>" + Form("%f", DD_EMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_EMiss<" + Form("%f", DD_EMISS_MAX) + ")" },
            {
                "DST0D0_g", "(" + descriptor + "_vs_KPi_EMiss>" + Form("%f", OTHER_EMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_EMiss<" + Form("%f", OTHER_EMISS_MAX) + ") & " +
                            "(" + descriptor + "_vs_KPi_BestMRec>" + Form("%f", DSTD_MREC_MIN) + ") & " + "(" + descriptor + "_vs_KPi_BestMRec<" + Form("%f", DSTD_MREC_MAX) + ") & " +
                            "(" + descriptor + "_vs_KPi_MMiss2>" + Form("%f", DSTDG_MMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_MMiss2<" + Form("%f", DSTDG_MMISS_MAX) + ")"
            },
            {
                "DST0D0_pi", "(" + descriptor + "_vs_KPi_EMiss>" + Form("%f", OTHER_EMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_EMiss<" + Form("%f", OTHER_EMISS_MAX) + ") & " +
                            "(" + descriptor + "_vs_KPi_BestMRec>" + Form("%f", DSTD_MREC_MIN) + ") & " + "(" + descriptor + "_vs_KPi_BestMRec<" + Form("%f", DSTD_MREC_MAX) + ") & " +
                            "(" + descriptor + "_vs_KPi_MMiss2>" + Form("%f", DSTDPI_MMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_MMiss2<" + Form("%f", DSTDPI_MMISS_MAX) + ")"
            },
            {
              "DST0DST0_even", "(" + descriptor + "_vs_KPi_EMiss>" + Form("%f", OTHER_EMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_EMiss<" + Form("%f", OTHER_EMISS_MAX) + ") & " +
                               "(" + descriptor + "_vs_KPi_BestMRec>" + Form("%f", DSTDST_MREC_MIN) + ") & " + "(" + descriptor + "_vs_KPi_BestMRec<" + Form("%f", DSTDST_MREC_MAX) + ") & " +
                               "(slowestPion_p>" + Form("%f", DSTDST_SLOWPI_MIN) + ") & " + "(slowestPion_p<" + Form("%f", DSTDST_SLOWPI_MAX) + ") & " +
                               "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_DSTMRec>" + Form("%f", DSTDST_DDG_MREC_MIN) + ") & " + "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_DSTMRec<" + Form("%f", DSTDST_DDG_MREC_MAX) + ") & " +
                               "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_deltaM>" + Form("%f", DSTDST_DDG_DELTAM_MIN) + ") & " + "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_deltaM<" + Form("%f", DSTDST_DDG_DELTAM_MAX) + ") & " +
                               "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_DSTMRec<" + Form("%f", BAD_DST_GRAD) + "*" + descriptor + "_vs_KPi_bestDSTDSTPhoton_deltaM + " + Form("%f", BAD_DST_YINT) + ") & " +
                               "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_MM2>" + Form("%f", DSTDSTE_DDG_MMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_MM2<" + Form("%f", DSTDSTE_DDG_MMISS_MAX) + ")"
            },
            {
              "DST0DST0_odd", "(" + descriptor + "_vs_KPi_EMiss>" + Form("%f", OTHER_EMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_EMiss<" + Form("%f", OTHER_EMISS_MAX) + ") & " +
                               "(" + descriptor + "_vs_KPi_BestMRec>" + Form("%f", DSTDST_MREC_MIN) + ") & " + "(" + descriptor + "_vs_KPi_BestMRec<" + Form("%f", DSTDST_MREC_MAX) + ") & " +
                               "(slowestPion_p>" + Form("%f", DSTDST_SLOWPI_MIN) + ") & " + "(slowestPion_p<" + Form("%f", DSTDST_SLOWPI_MAX) + ") & " +
                               "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_DSTMRec>" + Form("%f", DSTDST_DDG_MREC_MIN) + ") & " + "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_DSTMRec<" + Form("%f", DSTDST_DDG_MREC_MAX) + ") & " +
                               "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_deltaM>" + Form("%f", DSTDST_DDG_DELTAM_MIN) + ") & " + "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_deltaM<" + Form("%f", DSTDST_DDG_DELTAM_MAX) + ") & " +
                               "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_DSTMRec<" + Form("%f", BAD_DST_GRAD) + "*" + descriptor + "_vs_KPi_bestDSTDSTPhoton_deltaM + " + Form("%f", BAD_DST_YINT) + ") & " +
                               "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_MM2>" + Form("%f", DSTDSTO_DDG_MMISS_MIN) + ") & " + "(" + descriptor + "_vs_KPi_bestDSTDSTPhoton_MM2<" + Form("%f", DSTDSTO_DDG_MMISS_MAX) + ")"
            }
        };
        return cuts[prod];
    }

    inline TString BinCut(int bin){
        std::string binCut = "( (KPi_KCharge<0 & KSPiPi_vs_KPi_bin == " + std::to_string(bin) + ") | (KPi_KCharge>0 & KSPiPi_vs_KPi_bin == " + std::to_string(-1*bin) + ") )";
        return binCut.c_str();
    }

};

#endif //  Selection_H
