#ifndef study_SETeff
#define study_SETeff

/**
 * @file study_SETeff.cxx
 * Description of this file
 *
 * @brief A brief description
 *
 * @author cuto <Jakub.Cuth@cern.ch>
 * @date 2014-09-23
 */



// this should be in SETeff class

// vector<double> upara_binning;
// upara_binning.push_back(0.);
// 
// vector<double> elpt_binning;
// elpt_binning.push_back(30.);
// elpt_binning.push_back(35.);
// elpt_binning.push_back(40.);
// elpt_binning.push_back(45.);
// elpt_binning.push_back(50.);
// elpt_binning.push_back(55.);
// elpt_binning.push_back(60.);
// elpt_binning.push_back(70.);




typedef pair<Int_t,Int_t> EventID;

class SETeff_study {

    public:
        SETeff_study(){};
        ~SETeff_study(){};


        BookHistograms(){
            HBook1D("h_ZBlib_SET", Nbins, 0, 500);
        };


        void ReadZBLibrary(TString ZBlibPath){
            // load ttree
            TFile *f = new TFile(ZBlibPath);
            TTree *zbtree = (TTree*)gDirectory->Get("binary_tuple");
            Int_t maxLZB = zbtree->GetEntries();
            cout << "ZB tree has "<< maxLZB << " entries" << endl;
            cout << "Read library of ZB events ..." << endl;
            ZBmap.clear();
            ZBLibraryClass *zblibrary = new ZBLibraryClass(zbtree);
            nLZB=0;
            for (Int_t entryi=0; entryi < maxLZB; entryi++){
                zblibrary->GetEntry(entryi);
                EventID evtID( zblibrary->event_runNum, zblibrary->event_eventNum );
                pair<EventID,Float_t> item(evtID,zblibrary->event_HKset);
                ZBMap.insert(item);
                // counting and filling
                nLZB++;
                HFill1D("h_ZBlib_SET", item.second);
            }
            f->Close();
            cout<<" ... read "<<nLZB<<" events."<<endl;
        }

    private:
        map<EventID,Float_t> ZBMap;

        // histograms
        THashList hist_list;
        void HBook1D(TString name, TString title, nbins, xmin, xmax){
            TH1D *h = new TH1D(name,title,nbins,xmin,xmax);
            hist_list.AddLast(h);
        }
        void HFill1D(TString name, double value, double weight=1.0){
            TH1 * h = hist_list.Find(name);
            if (h==0) throw invalid_argument(Form("No histogram with name %s",name.Data()));
            h->Fill(value,weight);
        }
};


/**
 * Description of main program
 *
 */
int main(int argc, const char * argv[]){

    SETeff_study s;
    s.BookHistograms();

    s.ReadZBLibrary("/prj_root/7011/wmass1/stark/RunIIcAnal/PMCS02/mitraillette/make_extras/extras/jenny/MC/MBZBLibrary/Lib_RunIIb3_21Msinglenu_killcell_dq.root");
    s.ReadFullMC_fromDump("/prj_root/7011/wmass1/stark/RunIIcAnal/CAFprodJan2014/cabout/Results__MCWenu_RunIIb3_gamVeto_Dump/Dump.txt");
    s.ReadPMCS_fromDump("../Dump__MCwenu_PMCS09_IIb3_gamVeto_newSETeffC2_upslM001_upAdj2Int_noSETeff_Dump_WMass_pythia_wenu_6409ta_v1_snap_002_4822989.txt");


    s.CalculateEfficiencies();
    s.FitEfficiencies();
    s.SaveStudy();

    return 0;
}


#endif // study_SETeff
