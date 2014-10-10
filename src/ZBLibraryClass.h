//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Oct 12 21:07:48 2009 by ROOT version 5.22/00a
// from TTree binary_tuple/binary_tuple
// found on file: binary_library.root
//////////////////////////////////////////////////////////

#ifndef ZBLibraryClass_h
#define ZBLibraryClass_h

#define __NEWZBLIB__

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <iostream>

class ZBLibraryClass {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           event_runNum;
   Int_t           event_eventNum;
   Float_t         event_InstLum;
   Int_t           event_nPVS;
   Float_t         event_HKset;
   Float_t         event_HKmetx;
   Float_t         event_HKmety;
#ifdef __NEWZBLIB__
   Int_t           event_tickNum;
   Int_t           event_timeLastL1Accept;
#endif
   // List of branches
   TBranch        *b_event;   //!

   ZBLibraryClass(TTree *tree=0);
   virtual ~ZBLibraryClass();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef ZBLibraryClass_cxx
ZBLibraryClass::ZBLibraryClass(TTree *tree)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("binary_library.root");
      if (!f) {
         f = new TFile("binary_library.root");
      }
      tree = (TTree*)gDirectory->Get("binary_tuple");

   }
   Init(tree);
}

ZBLibraryClass::~ZBLibraryClass()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t ZBLibraryClass::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t ZBLibraryClass::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (!fChain->InheritsFrom(TChain::Class()))  return centry;
   TChain *chain = (TChain*)fChain;
   if (chain->GetTreeNumber() != fCurrent) {
      fCurrent = chain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void ZBLibraryClass::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);
   fChain->SetBranchAddress("event",&event_runNum); 
   Notify();
}

Bool_t ZBLibraryClass::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.
   b_event = fChain->GetBranch("event");	
   return kTRUE;	
}

void ZBLibraryClass::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t ZBLibraryClass::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef ZBLibraryClass_cxx
