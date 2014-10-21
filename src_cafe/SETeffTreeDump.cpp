#ifndef _SETeffTreeDump_CXX
#define _SETeffTreeDump_CXX

/**
 * @file SETeffTreeDump.cpp
 *
 * @brief A source file for class SETeffTreeDump.
 *
 * @author cuto <Jakub.Cuth@cern.ch>
 * @date 2014-10-15
 */

#include "wmass_util/SETeffTreeDump.hpp"


/**
 * Basic constructor.
 *
 * Longer description of basic constructor.
 */


    SETeffTreeDump::SETeffTreeDump() : 
            IsPMCS       (false)
    {
        ClearVars();
    };

    void SETeffTreeDump::ClearVars(){
            run          = - 999 ;
            evt          = - 999 ;
            lumblk       = - 999 ;
            isReco       = 0     ;
            lumi         = - 999.;
            scalaret     = - 999.;
            scalaretZB   = - 999.;
            scalaretMB   = - 999.;
            scalaretHard = - 999.;
            elepT        = - 999.;
            deteta       = - 999.;
            eta          = - 999.;
            upara        = - 999.;
            ut           = - 999.;
    }

    SETeffTreeDump::~SETeffTreeDump(){
        if (f!=0 && f->IsOpen()) f->Close();
    };

    void SETeffTreeDump::LoadTree(string path, bool _isPMCS){
        IsPMCS=_isPMCS;
        f = TFile::Open (path.c_str(), "READ");
        if (f==0) throw runtime_error(path.insert(0,"Can not open file ").c_str());
        t = (TTree *) f->Get("dump");
        if (t==0) throw runtime_error(path.insert(0,"Can not open tree `dump` in file ").c_str());
        // load according to type of dump
        if ( true    ) t->SetBranchAddress ( "run"          , &run          );
        if ( true    ) t->SetBranchAddress ( "evt"          , &evt          );
        if ( !IsPMCS ) t->SetBranchAddress ( "lumblk"       , &lumblk       );
        if ( !IsPMCS ) t->SetBranchAddress ( "isReco"       , &isReco       );
        if ( true    ) t->SetBranchAddress ( "lumi"         , &lumi         );
        if ( true    ) t->SetBranchAddress ( "scalaret"     , &scalaret     );
        if ( IsPMCS  ) t->SetBranchAddress ( "scalaretZB"   , &scalaretZB   );
        if ( IsPMCS  ) t->SetBranchAddress ( "scalaretMB"   , &scalaretMB   );
        if ( IsPMCS  ) t->SetBranchAddress ( "scalaretHard" , &scalaretHard );
        if ( true    ) t->SetBranchAddress ( "elepT"        , &elepT        );
        if ( true    ) t->SetBranchAddress ( "deteta"       , &deteta       );
        if ( true    ) t->SetBranchAddress ( "eta"          , &eta          );
        if ( true    ) t->SetBranchAddress ( "upara"        , &upara        );
        if ( true    ) t->SetBranchAddress ( "ut"           , &ut           );
    }

    void SETeffTreeDump::NewTree(string filepath, bool _isPMCS){
        IsPMCS=_isPMCS;
        f = TFile::Open(filepath.c_str(), "RECREATE");
        t = new TTree ("dump", "Dump tree for SET eff.");
        // register branches
        if ( true    ) t->Branch ( "run"          , &run          , "run/I"          );
        if ( true    ) t->Branch ( "evt"          , &evt          , "evt/I"          );
        if ( !IsPMCS ) t->Branch ( "lumblk"       , &lumblk       , "lumblk/I"       );
        if ( !IsPMCS ) t->Branch ( "isReco"       , &isReco       , "isReco/I"       );
        if ( true    ) t->Branch ( "lumi"         , &lumi         , "lumi/D"         );
        if ( true    ) t->Branch ( "scalaret"     , &scalaret     , "scalaret/D"     );
        if ( IsPMCS  ) t->Branch ( "scalaretZB"   , &scalaretZB   , "scalaretZB/D"   );
        if ( IsPMCS  ) t->Branch ( "scalaretMB"   , &scalaretMB   , "scalaretMB/D"   );
        if ( IsPMCS  ) t->Branch ( "scalaretHard" , &scalaretHard , "scalaretHard/D" );
        if ( true    ) t->Branch ( "elepT"        , &elepT        , "elepT/D"        );
        if ( true    ) t->Branch ( "deteta"       , &deteta       , "deteta/D"       );
        if ( true    ) t->Branch ( "eta"          , &eta          , "eta/D"          );
        if ( true    ) t->Branch ( "upara"        , &upara        , "upara/D"        );
        if ( true    ) t->Branch ( "ut"           , &ut           , "ut/D"           );
    }

    void SETeffTreeDump::Fill(){
        t->Fill();
    }

    void SETeffTreeDump::Write(){
        f->cd();
        t->Write();
        f->Write();
    }

    Long64_t SETeffTreeDump::GetEntries() const {
        return t->GetEntries();
    }

    void SETeffTreeDump::GetEntry(Long64_t i){
        t->GetEntry(i);
    }


#endif //_SETeffTreeDump_CXX
