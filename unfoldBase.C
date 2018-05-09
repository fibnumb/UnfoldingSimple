#ifndef __CINT__
#include <iostream>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TList.h>
#include <TDirectory.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TPad.h>
#include <THistPainter.h>
#include <TObject.h>
#include <TMath.h>
#include <TFile.h>
#include <TGraphErrors.h>
#include <TLatex.h>
#include <TLine.h>
#include <TMarker.h>
#include <TStyle.h>
#include <TVirtualFitter.h>
#include <TMinuit.h>
#include <TBox.h>
#include <TGaxis.h>
#include "TRandom.h"
#include <TROOT.h>
#include <TSystem.h>
#include "TH1D.h"
#include "/Users/fibnumb/alice/roo-build/RooUnfold/include/RooUnfoldResponse.h"
#include "/Users/fibnumb/alice/roo-build/RooUnfold/include/RooUnfoldBayes.h"
//#include "/Users/fibnumb/alice/roo-build/RooUnfold/include/RooUnfoldUtils.C"
#include "/Users/fibnumb/alice/roo-build/RooUnfold/include/RooUnfoldSvd.h"
#include "/Users/fibnumb/alice/roo-build/RooUnfold/include/RooUnfoldBinByBin.h"
#include "/Users/fibnumb/alice/roo-build/RooUnfold/include/RooUnfoldErrors.h"
#else
class RooUnfoldResponse;
class RooUnfoldBayes;
class RooUnfold;
class TF1;
class TFile;
class TH1D;
class TH1;
class TH1F;
class TH2D;
class TGraphErrors;
class TGaxis;
class AliUnfolding;
#endif
using namespace std;

 TH1D *unfoldBayes(TH1D* data, RooUnfoldResponse* response);
 TH1D *unfoldSVD(TH1D* data, RooUnfoldResponse* response);


void unfoldBase(const Char_t* output="unfoldOutput.root", const Char_t* responseFileName="AnalysisJetResponseR02t0150C0300.root", const Char_t* dataFileName="AnalysisResults.root"){

    TFile *responseFile = TFile::Open(responseFileName,"READ");
    if (!responseFile|| responseFile->IsZombie() ) {
        Error("createUnfoldingFile", "Couldn't open %s", responseFileName);
        return;
    }
    TList *responseList = static_cast<TList*>(responseFile->Get("AliJetResponseMaker_Jet_AKTFullR020_tracks_pT0150_caloClusters_E0300_pt_scheme_Jet_AKTFullR020_mcparticles_pT0000_pt_scheme_Bias5_BiasType0_EMCALfid_histos"));
    
    TFile* dataFile = TFile::Open(dataFileName,"READ");
    if (!dataFile || dataFile->IsZombie()) {
        Error("createUnfoldingFile", "Couldn't open %s", dataFileName);
        return;
    }
    
    TList* dataList = static_cast<TList*>(dataFile->Get("AliAnalysisTaskEmcalJetSpectra8TeVTriggerQA_tracks_caloClusters_emcalCells_new_histos"));
    //TList* jetList = static_cast<TList*>(dataList->Get("AliAnalysisTaskEmcalJetSpectra8TeVTriggerQA_tracks_caloClusters_emcalCells_new_histos"));
    TList *JetR02list= (TList *)dataFile->Get("AliAnalysisTaskEmcalJetSpectra8TeVTriggerQA_tracks_caloClusters_emcalCells_new_histos")->FindObject("Jet_AKTFullR020_tracks_pT0150_caloClusters_E0300_pt_scheme");
    TFile* outputFile = TFile::Open(output, "RECREATE");
    cout << " Loaded responseFile and dataFile" << endl;

    TH1D *histMeasSpectra = (TH1D*) JetR02list->FindObject("histJetPt_0");           //! Measured Spectra
    Int_t nMeasSpecBins = histMeasSpectra->GetNbinsX();
    cout<<"Number of bins along x-axis Measured Spectra:   "<<nMeasSpecBins<<endl;
 
    TH2D *hist2Dresponse = (TH2D*) responseList->FindObject("fHistJet1PtvsJet2Pt");  //! Response Matrix
    hist2Dresponse->SetTitle("Response Matrix");
    TAxis *xAxis = hist2Dresponse->GetXaxis();
    TAxis *yAxis = hist2Dresponse->GetYaxis();
    TH1D *histProjX = hist2Dresponse->ProjectionX();
    TH1D *histProjY = hist2Dresponse->ProjectionY();
    Int_t nBinx = xAxis->GetNbins(); // Number of bins along x-axis
    Int_t nBiny = yAxis->GetNbins(); // Number of bins along y-axis
    cout<<"Number of bins along x-axis:\t"<<nBinx<<"\n";
    cout<<"Number of bins along y-axis:\t"<<nBiny<<"\n";
    
    
    //RooUnfoldResponse::responseMatrix(histProjX, histProjY);Jet finders in Response Maker Task
    RooUnfoldResponse* responseObject = new RooUnfoldResponse(histProjY, histProjX); //measured y-axis :: true x-axis from
    
    cout<<"Filling response matrix... \n";
    Int_t coord[2];
    Float_t binCont, coordPt[2];
    
    for(Int_t xBin = 1; xBin <= nBinx; xBin++){
        for(Int_t yBin = 0; yBin < nBiny; yBin++){
            binCont = (Float_t) hist2Dresponse->GetBinContent(xBin, yBin);
            
            coordPt[0] = (Float_t) xAxis->GetBinCenter(coord[0]);
            coordPt[1] = (Float_t) yAxis->GetBinCenter(coord[1]);
            
            responseObject->Fill(coordPt[0], coordPt[1], binCont);
        }
    }
    
    TH1D* unfolded = unfoldBayes(histMeasSpectra ,responseObject);
    
    TH1D *unfoldedSVD = 0x0;
    unfoldedSVD = unfoldSVD(histMeasSpectra, responseObject);
    
    
    
  hist2Dresponse->Write();
  unfolded->Write();
  histMeasSpectra->Write();
  unfoldedSVD->Write();
  outputFile->Close();
  responseFile->Close();
  dataFile->Close();
}


// --- Use RooUnfold stuff -------------------------------------------

// -- Use Bayesian Unfolding -----------------------------------------
TH1D* unfoldBayes(TH1D* data, RooUnfoldResponse* response)
{
    
    
    RooUnfold* unfold = new RooUnfoldBayes(response, data, 20);
    TH1D* unfolded= (TH1D*) unfold->Hreco();
    
    TString t = TString::Format("unfolded_bayes");
    
    unfolded->SetName(t);
    delete unfold;
    
    return unfolded;
}

// -- Use SVD Unfolding ----------------------------------------------
TH1D* unfoldSVD(TH1D* data, RooUnfoldResponse* response)
{

    RooUnfold* unfold = new RooUnfoldSvd(response, data);
    TH1D* unfolded = (TH1D*) unfold->Hreco();
    
    TString t = TString::Format("unfolded_bayes");
    
    unfolded->SetName(t);
    delete unfold;
    
    return unfolded;
    
}


// --- Get end of input histogram name -------------------------------
const Char_t*
getPostfix(const TH1* h)
{
    static TString t;
    t = h->GetName();
    t.ReplaceAll("mult", "");
    return t.Data();
}
//
//   EOF
//
