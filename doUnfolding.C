{
    cout<<"Loading RooUnfold Library"<<endl;
  gSystem->Load("/Users/fibnumb/alice/sw/BUILD/RooUnfold-latest/RooUnfold/libRooUnfold");
    cout<<"Loading AliRoot && AliPhysics includes"<<endl;
  gSystem->AddIncludePath("-I${ALICE_ROOT}/include -I${ALICE_PHYSICS}/include");
   
    
    cout<<"Running unfolding task"<<endl;
    
    gROOT->ProcessLine(.x RooUnfoldExample.cxx);
    
  //gROOT->LoadMacro("unfoldBase.C++g");
    //gInterpreter->ExecuteMacro("unfoldBase.C")
    
}

