#include <iostream>
#include <fstream>

#include "TRandom.h"
#include "TH1D.h"
#include "TH2D.h"
#include "THn.h"
#include "THnSparse.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TAxis.h"
#include "TString.h"
#include "TFile.h"

#include "/Users/fibnumb/alice/roo-build/RooUnfold/include/RooUnfold.h"
#include "/Users/fibnumb/alice/roo-build/RooUnfold/include/RooUnfoldResponse.h"
#include "/Users/fibnumb/alice/roo-build/RooUnfold/include/RooUnfoldBayes.h"
#include "/Users/fibnumb/alice/roo-build/RooUnfold/include/RooUnfoldSvd.h"
#include "/Users/fibnumb/alice/roo-build/RooUnfold/include/RooUnfoldTUnfold.h"
#include "/Users/fibnumb/alice/roo-build/RooUnfold/include/RooUnfoldInvert.h"
#include "/Users/fibnumb/alice/roo-build/RooUnfold/include/RooUnfoldBinByBin.h"

void AndyUnfold(){

    cout<<"Welcome to AndyUnfold.C! \n";
    cout<<"Finding input .root files... \n";

    TFile *fileTrain = TFile::Open("AnalysisJetResponseR02t0150C0300.root");
    TH2D *hist2D = (TH2D*) fileTrain->TFile::Get("fHistJet1PtvsJet2Pt"); // Andy's response matrix
    hist2D->SetTitle("Response Matrix");
    TH1D *histProjX = hist2D->ProjectionX();
    TH1D *histProjY = hist2D->ProjectionY();

    TFile *fileReal = TFile::Open("AnalysisResults_LR.root");
    TH1D *histMeas = (TH1D*) fileReal->TFile::Get("Jet_AKTFullR020_tracks_pT0150_caloClusters_E0300_pt_scheme/histJetPt_0"); // TH2 measured for unfolding
    histMeas->SetTitle("Measured Distribution");

    cout<<"Files opened successfully. \n";



    //--------------------------------------------------------------------------------------------



    cout<<"Initializing response matrix... \n";
    RooUnfoldResponse responseMatrix (histProjX, histProjY); // Create response matrix with same binning as measuredXtrue
    cout<<"Response matrix ready. \n";

    Int_t coord[2];
    Float_t binCont, coordPt[2];
    TAxis *xAxis = hist2D->GetXaxis();
    TAxis *yAxis = hist2D->GetYaxis();
    Int_t nBinx = xAxis->GetNbins(); // Number of bins along x-axis
    Int_t nBiny = yAxis->GetNbins(); // Number of bins along y-axis
    cout<<"Number of bins along x-axis:\t"<<nBinx<<"\n";
    cout<<"Number of bins along y-axis:\t"<<nBiny<<"\n";

    cout<<"Filling response matrix... \n";

    for(Int_t xBin = 1; xBin <= nBinx; xBin++){
        for(Int_t yBin = 0; yBin < nBiny; yBin++){
            binCont = (Float_t) hist2D->GetBinContent(xBin, yBin);

            coordPt[0] = (Float_t) xAxis->GetBinCenter(coord[0]);
        	coordPt[1] = (Float_t) yAxis->GetBinCenter(coord[1]);

            responseMatrix.Fill(coordPt[0], coordPt[1], binCont);
        }
    }

    cout<<"Response matrix filled. \n";



    //--------------------------------------------------------------------------------------------------



    // Unfolding

    cout<<"Beginning unfolding... \n";

    RooUnfoldBinByBin unfoldBBB(&responseMatrix, histMeas);
    TH2F *histBBB = (TH2F*) unfoldBBB.Hreco();
    histBBB->SetTitle("Unfolded with Bin by Bin");

    RooUnfoldBayes unfoldBay(&responseMatrix, histMeas,3);
    TH2F *histBay = (TH2F*) unfoldBay.Hreco();
    histBay->SetTitle("Unfolded with Bayesian");

    cout<<"Unfolding complete. \n";



    //--------------------------------------------------------------------------------------------------



    // Plotting histograms

    cout<<"Plotting histograms... \n";

    TCanvas *plot = new TCanvas("plot","True, Measured, and Reconstructed Histograms",700,800);
    plot->Divide(2,2);

    // Plotting true
    plot->cd(1);
    hist2D->Draw("colz");

    // Plotting measured
    plot->cd(2);
    histMeas->Draw();

    // Plotting reconstructed
    plot->cd(3);
    histBBB->Draw("colz"); // Bin by bin method
    plot->cd(4);
    histBay->Draw("colz"); // Bayesian method

}
