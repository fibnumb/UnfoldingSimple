{
    gSystem->Load("/Users/fibnumb/alice/sw/BUILD/RooUnfold-latest-roo-build/RooUnfold/libRooUnfold");
    gSystem->AddIncludePath("-I${ALICE_ROOT}/include -I${ALICE_PHYSICS}/include -I$ROOTSYS/include");
    gSystem->Load("/Users/fibnumb/alice/sw/osx_x86-64/RooUnfold/roo-build-2/libRooUnfold.dylib");
    gSystem->Load("libRooUnfold");
    // AliUnfolding
    gSystem->Load("libVMC");
    gSystem->Load("libMinuit");
    gSystem->Load("libSTEERBase");
    gSystem->Load("libESD");
    gSystem->Load("libAOD");
    gSystem->Load("libANALYSIS");
    gSystem->Load("libANALYSISalice");
    
    
    gROOT->LoadMacro("unfoldBase.C++g");
    
    unfoldBase("unfoldOutput.root","AnalysisJetResponseR02t0150C0300.root","AnalysisResults200KLHC12e.root");
}

