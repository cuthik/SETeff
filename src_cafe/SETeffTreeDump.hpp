#ifndef _SETeffTreeDump_H
#define _SETeffTreeDump_H

/**
 * @file SETeffTreeDump.hpp
 *
 * @brief A header file for class SETeffTreeDump.
 * @date 2014-10-15
 */


#include "TTree.h"
#include "TFile.h"

/**
 * Description of class.
 *
 * Longer description of class
 */
class SETeffTreeDump {
    public:
        SETeffTreeDump();
        ~SETeffTreeDump();

        void LoadTree(string path, bool isPMCS=false);
        void NewTree(string filepath, bool _isPMCS=false);
        Long64_t GetEntries() const;
        void GetEntry(Long64_t i);
        void Fill();
        void ClearVars();
        void Write();

        int    run          ;
        int    evt          ;
        int    lumblk       ;
        int    isReco       ; ///< recoFlag. 0 no reco; 1 reco; ...;
        double lumi         ;
        double scalaret     ;
        double scalaretZB   ;
        double scalaretMB   ;
        double scalaretHard ;
        double elepT        ;
        double deteta       ;
        double eta          ;
        double upara        ;
        double ut           ;

        bool IsPMCS;

    private :
        TFile *f;
        TTree *t;
};


#endif //_SETeffTreeDump_H
