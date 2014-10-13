#ifndef _SETeff_CXX
#define _SETeff_CXX

/**
 * @file SETeff.cpp
 *
 * @brief A source file for class SETeff.
 *
 * @author cuto <Jakub.Cuth@cern.ch>
 * @date 2014-09-23
 */

#include "SETeff.h"
#include "ZBLibraryClass.h"


// ROOT
#include "TCollection.h"
#include "TObject.h"

// STL
#include <stdexcept>
#include <ctime>
#ifdef NoC11
#include <cstdio>
#include <cstdlib>
#include <cmath>
#endif

using std::runtime_error;
using std::invalid_argument;
using std::transform;
using std::ofstream;
#ifndef NoC11
using std::to_string;
using std::stod;
#endif

namespace SETeff{

    /// Handler

    double EffHandler::GetEfficiency(double SET, double elpt, double eta, double lumi, double upar, double ut){
        double *params = 0;
        params = fit_pars.GetParArray(elpt, eta, lumi, upar, ut);
        double val = 0;
        switch(dist_type){
            case SETeff::SET       : val = SET;      break;
            case SETeff::SEToverPT : val = SET/elpt; break;
            default : val = SET;
        }
        switch(fun_type){
            case SETeff::p3_20  : return fun_p3_20  (&val,params);
            case SETeff::p5_200 : return fun_p5_200 (&val,params);
            case SETeff::p5_20  : return fun_p5_20  (&val,params);
        }
        return 1;
    }

    void EffHandler::SaveConfig(string filepath) {
        time_t now = time(0);
        ofstream outf;
        outf.open(filepath.c_str());
        outf << "# This file has been generated automatically " ;
        outf << ctime(&now) << "\n";
        //f << (ConfigFile) *this;
        outf.close();
    }

    void EffHandler::GetIndices(const ParSetIdentifier i, size_t &i_pt, size_t &i_eta, size_t &i_lumi, size_t &i_upar, size_t &i_ut){
        ParSetIdentifier index = i;
        int base = 1;
        // get max
        size_t N_pt   = bins_pt   .GetNBins();
        size_t N_eta  = bins_eta  .GetNBins();
        size_t N_lumi = bins_lumi .GetNBins();
        size_t N_upar = bins_upar .GetNBins();
        size_t N_ut   = bins_ut   .GetNBins();
        // set base
        base *= N_pt   ;
        base *= N_eta  ;
        base *= N_lumi ;
        base *= N_upar ;
        base *= N_ut   ;
        //decompose number
        index %= base ; base /= N_ut   ; i_ut   = index/base;
        index %= base ; base /= N_upar ; i_upar = index/base;
        index %= base ; base /= N_lumi ; i_lumi = index/base;
        index %= base ; base /= N_eta  ; i_eta  = index/base;
        index %= base ; base /= N_pt   ; i_pt   = index/base;
    }

    ParSetIdentifier EffHandler::GetIndex(size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut) const{
        ParSetIdentifier index = 0;
        int base = 1;
        index+= i_pt   * base; base *= bins_pt   .GetNBins();
        index+= i_eta  * base; base *= bins_eta  .GetNBins();
        index+= i_lumi * base; base *= bins_lumi .GetNBins();
        index+= i_upar * base; base *= bins_upar .GetNBins();
        index+= i_ut   * base; base *= bins_ut   .GetNBins();
        return index;
    }

    ParSetIdentifier EffHandler::GetIndex(double pt, double eta, double lumi, double upar, double ut) const{
        return GetIndex(
                bins_pt   .GetBin( pt   ) ,
                bins_eta  .GetBin( eta  ) ,
                bins_lumi .GetBin( lumi ) ,
                bins_upar .GetBin( upar ) ,
                bins_ut   .GetBin( ut   ) 
                );
    }

    ParSetIdentifier EffHandler::GetMaxIndex() const{
        int base = 1;
        base *= bins_pt   .GetNBins();
        base *= bins_eta  .GetNBins();
        base *= bins_lumi .GetNBins();
        base *= bins_upar .GetNBins();
        base *= bins_ut   .GetNBins();
        return base;
    }

    void EffHandler::SetMissingParamsToOne(){
        vector<double> par_vec (5,1.);
        for (size_t i_pt   =0; i_pt   < bins_pt   .GetNBins(); i_pt   ++ ){
        for (size_t i_eta  =0; i_eta  < bins_eta  .GetNBins(); i_eta  ++ ){
        for (size_t i_lumi =0; i_lumi < bins_lumi .GetNBins(); i_lumi ++ ){
        for (size_t i_upar =0; i_upar < bins_upar .GetNBins(); i_upar ++ ){
        for (size_t i_ut   =0; i_ut   < bins_ut   .GetNBins(); i_ut   ++ ){
            try {
                fit_pars.GetParArray( i_pt   , i_eta  , i_lumi , i_upar , i_ut   ); // if there is no entry it will throw runtime_error
            }
            catch (runtime_error &e){
                // add missing vector
                fit_pars.SetParams( par_vec,  i_pt   , i_eta  , i_lumi , i_upar , i_ut   );
            }
        } } } } } 
    }

    void EffHandler::test_indeces(ostream &os){
                size_t N_pt   = 1;
                size_t N_eta  = 0;
                size_t N_lumi = 1;
                size_t N_upar = 1;
                size_t N_ut   = 0;
                os << 
                    N_pt    <<  " " <<
                    N_eta   <<  " " <<
                    N_lumi  <<  " " <<
                    N_upar  <<  " " <<
                    N_ut    << "\n";
                ParSetIdentifier i = GetIndex(
                        N_pt   ,
                        N_eta  ,
                        N_lumi ,
                        N_upar ,
                        N_ut   
                        );
                os << i << "\n";
                N_pt   = 0;
                N_eta  = 0;
                N_lumi = 0;
                N_upar = 0;
                N_ut   = 0;
                os << 
                    N_pt    <<  " " <<
                    N_eta   <<  " " <<
                    N_lumi  <<  " " <<
                    N_upar  <<  " " <<
                    N_ut    << "\n";
                GetIndices(i,
                        N_pt   ,
                        N_eta  ,
                        N_lumi ,
                        N_upar ,
                        N_ut
                        );
                os << 
                    N_pt    <<  " " <<
                    N_eta   <<  " " <<
                    N_lumi  <<  " " <<
                    N_upar  <<  " " <<
                    N_ut    << "\n";
            }

    /// Binning


    void EffBins::SetByVector(vector<double> &vec){
        if(vec.size()==0){
            clear();
            push_back(0.);
            push_back(1.);
        }
        else if(vec.size()==1){
            clear();
            push_back(vec[0]-1);
            push_back(vec[0]);
            push_back(vec[0]+1);
        }
        else vector<double>::operator=(vec);
    }

    void EffBins::SetByArray(size_t nbins, double* list){
        clear();
        for (size_t i=0; i<=nbins; i++){
            push_back(list[i]);
        }
    }

    void EffBins::SetByRange(size_t nbins, double low, double high){
        clear();
        if (nbins==0){ // one bin counter
            push_back(0.);
            push_back(1.);
        }
        if (nbins==1){// +-
            push_back(low-1.);
            push_back(low);
            push_back(low+1.);
        }
        if (nbins>1){// bins including over and underflow
            clear();
            double step = (high-low)/nbins;
            for(size_t i=0;i<=nbins;i++) push_back(low+step*i);
        }
    }

    /// Getters
    size_t EffBins::GetNBins() const{
        if (size()==0) return 1;
        return size()-1;
    }
    double * EffBins::GetBinArray(){ return & (*begin());}

    double EffBins::Val(double in){ // test for overflow, underflow
        if (size()==0.) return 0.;
        if (in < Underflow() ) return Underflow();
        if (in > Overflow()  ) return Overflow();
        return in;
    }

    size_t EffBins::GetBin(double in) const{
        //if (size()==2) return 0;
        size_t i = 1;
        for (; i<size(); i++){
            if (in<at(i)) return i-1;
        }
        return i-2;
    }

    double EffBins::Underflow() {return *( begin() );} // lowest accepted value
    double EffBins::Overflow()  {return *( end()-1 );} // highghest accepted value


    /// Config file

    ConfigFile::ConfigFile() :
        fun_type  (SETeff::p3_20),
        dist_type (SETeff::SET),
        bins_SET       ( "SET[GeV]"       ),
        bins_SEToverPT ( "SET / p_{T}"    ),
        bins_pt        ( "el p_{T}[GeV]"  ),
        bins_eta       ( "el #eta"        ),
        bins_lumi      ( "lumi[#mub]"     ),
        bins_upar      ( "u_{par}[GeV]"   ),
        bins_ut        ( "u_{T}[GeV]"     )
    {
    }

    void ConfigFile::LoadConfig(const char * path){
        // open file
        open(path);
        // basic config
        config_fit_fun();
        config_fit_dist();
        // binning
        config_bins ("SET"      , &bins_SET       );
        config_bins ("SEToverPT", &bins_SEToverPT );
        config_bins ("elpt"     , &bins_pt        );
        config_bins ("eleta"    , &bins_eta       );
        config_bins ("lumi"     , &bins_lumi      );
        config_bins ("upara"    , &bins_upar      );
        config_bins ("ut"       , &bins_ut        );
        // load parameter values
        config_fit_pars();
        // close file
        close();
    }

    void ConfigFile::open(string conf_filename){
        f.open(conf_filename.c_str());
    }

    void ConfigFile::close(){
        f.close();
    }

    void ConfigFile::config_fit_fun(){
        string s_data = GetString("fit_fun");
        if      (s_data.compare("p3_20"  ) == 0) fun_type=SETeff::p3_20  ;
        else if (s_data.compare("p5_200" ) == 0) fun_type=SETeff::p5_200 ;
        else if (s_data.compare("p5_20"  ) == 0) fun_type=SETeff::p5_20  ;
        else {
            s_data.insert(0,"Unknown function type: '");
            s_data.append("'");
            throw runtime_error(s_data);
        }
    }

    void ConfigFile::config_fit_dist(){
        string s_data = GetString("fit_dist");
        if      (s_data.compare("SET"       ) == 0) dist_type=SETeff::SET       ;
        else if (s_data.compare("SEToverPT" ) == 0) dist_type=SETeff::SEToverPT ;
        else {
            s_data.insert(0,"Unknown distribution type: '");
            s_data.append("'");
            throw runtime_error(s_data);
        }
    }

    void ConfigFile::config_bins(string bin_name, EffBins * bins){
        if (!StartWithNoCase(bin_name, "bins_")) bin_name.insert(0, "bins_"); // shortcut if it's not begging on "bins_"
        vector<double> s_bin_vec = GetVecDouble(bin_name);
        bins->SetByVector(s_bin_vec);
    }

    void ConfigFile::config_fit_pars(){
        // assuming all config was already done
        fit_pars.SetHandler( handler );
        // loop all posible bins where parameters are estimated -- this is estimated from binning definition
        for (size_t i_pt   =0; i_pt   < bins_pt   .GetNBins(); i_pt   ++ ){
        for (size_t i_eta  =0; i_eta  < bins_eta  .GetNBins(); i_eta  ++ ){
        for (size_t i_lumi =0; i_lumi < bins_lumi .GetNBins(); i_lumi ++ ){
        for (size_t i_upar =0; i_upar < bins_upar .GetNBins(); i_upar ++ ){
        for (size_t i_ut   =0; i_ut   < bins_ut   .GetNBins(); i_ut   ++ ){
            string par_name = GetParNameByIndices( i_pt   , i_eta  , i_lumi , i_upar , i_ut   );
            vector<double> par_vec = GetVecDouble(par_name);
            fit_pars.SetParams(par_vec,  i_pt   , i_eta  , i_lumi , i_upar , i_ut   );
        } } } } }
    }


    string ConfigFile::GetString(string name){
        string out;
        find_config(name);
        // join vectors if needed
        for(vector<string>::iterator it=data.begin()+1; it!=data.end(); ++it){
            if (it!=data.begin()+1) out.append(" ");
            out.append(*it);
        }
        return out;
    }

    vector<double> ConfigFile::GetVecDouble(string name){
        vector<double> out;
        find_config(name);
        // parse string to vector of doubles
        for(vector<string>::iterator it=data.begin()+1; it!=data.end(); ++it){
            out.push_back(StrinToDouble(*it));
        }
        return out;
    }

    void ParameterEstimator::OldOutput(ostream &os){
        os<<"==============================================================================================="<<"\n";
        os<<"WSETEffCorrPTEpert:     3"<<"\n";
        // fix bins
        size_t i_lumi=0;
        size_t i_upar=0;
        size_t i_ut=0;
        // loop bins
        for ( size_t i_eta = 0; i_eta < handler->bins_eta.GetNBins(); i_eta ++){
            os<<"WSETEffCorrPTEpertBase_"<<i_eta<<": 0. 0. 0. 0. 0."<<"\n";
            for ( size_t i_pt  = 0; i_pt  < handler->bins_pt.GetNBins(); i_pt  ++){
                ParSetIdentifier i = handler -> GetIndex(i_pt,i_eta,i_lumi,i_upar,i_ut);
                char b='A';
                b+=(char)i_pt;
                os<<"WSETEffCorrPTEpert"<<b<<"_"<<i_eta<<":    ";
                os << ToString(handler->fit_pars[i]) << "\n";
            }
            os<<"WSETEffCorrPTEpertNorm_"<<i_eta<<":  ";
            for ( size_t i_pt  = 0; i_pt  < handler->bins_pt.GetNBins(); i_pt  ++){
                ParSetIdentifier i              = handler -> GetIndex( i_pt, i_eta, i_lumi, i_upar, i_ut);
                ParSetIdentifier i_allpt_alleta = handler -> GetIndex( 0,    0,     i_lumi, i_upar, i_ut);
                os<< m_finalScale[i] / m_finalScaleMaxGlobal[i_allpt_alleta];
                if (i_pt<7) os<<"  ";
                else os<<"\n";
            }
        }
        os<<"==============================================================================================="<<"\n";
    }

    void ConfigFile::find_config(string config_name){ // read config file and find type of function
        f.seekg(0,f.beg); // move to beginnning
        string line;
        while(!f.eof()){
            getline(f,line);
            remove_comments(line);
            data = SplitString(line);
            if (line_config_is(config_name)){
                return;
            }
        }
        data.clear();
        throw runtime_error(config_name.insert(0, "Configuration not found: ").c_str());
    }

    bool ConfigFile::line_config_is(string conf_name){
        if (data.empty()) return false;
        string tmp = data[0];
        ToLowcase(tmp);
        ToLowcase(conf_name);
        if (0==tmp.compare(conf_name)) return true;
        return false;
    }

    void ConfigFile::remove_comments(string &line){
        size_t pos = line.find_first_of("#");
        if(pos!=string::npos) line.erase(pos);
    }

    /// Parameter storage

    EffParams::EffParams() :
        handler   ( 0 )
    {
    };

    void EffParams::SetParams(TF1 * fun, ParSetIdentifier i){
        vector<double> vec;
        for (Int_t j=0; j < fun->GetNpar(); j++) vec.push_back(fun->GetParameter(j));
        map<ParSetIdentifier,vector<double> >::operator[](i)=vec;
    }

    void EffParams::SetParams(vector<double> &vec, double pt, double eta, double lumi, double upar, double ut){
        ParSetIdentifier i = handler->GetIndex(pt,eta,lumi,upar,ut);
        map<ParSetIdentifier,vector<double> >::operator[](i)=vec;
    }

    void EffParams::SetParams(vector<double> &vec, size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut){
        ParSetIdentifier i = handler->GetIndex(i_pt,i_eta,i_lumi,i_upar,i_ut);
        map<ParSetIdentifier,vector<double> >::operator[](i)=vec;
    }

    double * EffParams::GetParArray(ParSetIdentifier i){
        test_key(i);
        return (double*) & (operator[](i)[0]);
    }

    double * EffParams::GetParArray(size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut){
        return GetParArray(handler->GetIndex(i_pt,i_eta,i_lumi,i_upar,i_ut));
    }

    double * EffParams::GetParArray(double pt, double eta, double lumi, double upar, double ut){
        return GetParArray(handler->GetIndex(pt,eta,lumi,upar,ut));
    }

    vector<double> EffParams::GetParVec(ParSetIdentifier i) const{
        test_key(i);
        return find(i)->second;
    }

    vector<double> EffParams::GetParVec(size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut) const{
        return GetParVec(handler->GetIndex(i_pt,i_eta,i_lumi,i_upar,i_ut));
    }

    vector<double> EffParams::GetParVec(double pt, double eta, double lumi, double upar, double ut) const{
        return GetParVec(handler->GetIndex(pt,eta,lumi,upar,ut));
    }


    void EffParams::test_key(ParSetIdentifier key) const{
        if (count(key)==0){
            string what = "Parameter map has no key ";
            what.append(ToString(key));
            throw runtime_error(what.c_str());
        }
        // if ( at(key).size() != 5){
        //     string what = "Parameter map has key with no items ";
        //     what.append(ToString(key));
        //     throw runtime_error(what.c_str());
        // }
    }



    /// Dump Tree
    TreeDump::TreeDump() : 
            IsPMCS       (0)
    {
        ClearVars();
    };

    void TreeDump::ClearVars(){
            run          = - 999 ;
            evt          = - 999 ;
            lumblk       = - 999 ;
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

    TreeDump::~TreeDump(){
        if (f!=0 && f->IsOpen()) f->Close();
    };

    void TreeDump::LoadTree(string path, bool _isPMCS){
        IsPMCS=_isPMCS;
        f = TFile::Open (path.c_str(), "READ");
        if (f==0) throw runtime_error(path.insert(0,"Can not open file ").c_str());
        t = (TTree *) f->Get("dump");
        if (t==0) throw runtime_error(path.insert(0,"Can not open tree `dump` in file ").c_str());
        // load according to type of dump
        if ( true    ) t->SetBranchAddress ( "run"          , &run          );
        if ( true    ) t->SetBranchAddress ( "evt"          , &evt          );
        if ( !IsPMCS ) t->SetBranchAddress ( "lumblk"       , &lumblk       );
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

    void TreeDump::NewTree(string filepath, bool _isPMCS){
        IsPMCS=_isPMCS;
        f = TFile::Open(filepath.c_str(), "RECREATE");
        t = new TTree ("dump", "Dump tree for SET eff.");
        // register branches
        if ( true    ) t->Branch ( "run"          , &run          , "run/I"          );
        if ( true    ) t->Branch ( "evt"          , &evt          , "evt/I"          );
        if ( !IsPMCS ) t->Branch ( "lumblk"       , &lumblk       , "lumblk/I"       );
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

    void TreeDump::Fill(){
        t->Fill();
    }

    void TreeDump::Write(){
        f->cd();
        t->Write();
        f->Write();
    }

    Long64_t TreeDump::GetEntries() const {
        return t->GetEntries();
    }

    void TreeDump::GetEntry(Long64_t i){
        t->GetEntry(i);
    }

    /// Parameter Estimator

    ParameterEstimator::ParameterEstimator() :
            UseFullSET (true  ),
            UsePhysEta (false )
    {
    }

    ParameterEstimator::~ParameterEstimator() {
    }

    void ParameterEstimator::LoadZBlib  (string path){
        if (UseFullSET) return;
        BookHist(0,"SET_ZBlib", &handler->bins_SET);
        TFile *f = new TFile(path.c_str());
        TTree *zbtree = (TTree*)gDirectory->Get("binary_tuple");
        if (zbtree->GetEntries() == 0) throw runtime_error("ZBtree has no entries.");
        Int_t maxLZB = zbtree->GetEntries();
        Double_t SET = 0;
        ZBLibraryClass *zblibrary = new ZBLibraryClass(zbtree);
        Int_t nLZB=0;
        for (Int_t entryi=0; entryi < maxLZB; entryi++){
            zblibrary->GetEntry(entryi);
            SET = zblibrary->event_HKset;
            EventIdentifier evtID(
                    zblibrary->event_runNum,
                    zblibrary->event_eventNum
                    );
            m_zblib.insert(pair<EventIdentifier,Float_t>(evtID,SET));
            //SET = handler->bins_SET.Val(SET);
            GetHistogram(0,"SET_ZBlib") -> Fill (SET);
            nLZB++;
        }
        f->Close();
        if (nLZB == 0) throw runtime_error("No events loaded from ZBtree.");
    }

    void ParameterEstimator::LoadFullMC (string path){
        t_fullMC.LoadTree(path);
    }

    void ParameterEstimator::LoadPMCS   (string path){
        t_PMCS.LoadTree(path,true);
    }

    void ParameterEstimator::EstimateParameters(){
        // the configuration file have to be loaded before
        BookHistogramsAndScales(); // reserve all needed histograms and functions
        FillHistograms(); // loop tree and fill histograms
        MakeEfficiencies(); // from histograms create ratio histograms, calculate scales
        FitEfficiencies();
        CalculateScales();
    }

    void ParameterEstimator::BookHistogramsAndScales(){
        size_t i_pt,i_eta,i_lumi,i_upar,i_ut    ;
               i_pt=i_eta=i_lumi=i_upar=i_ut =-1;
        // main binning
        EffBins *                                    binning = & handler->bins_SET;
        if (handler->dist_type == SETeff::SEToverPT) binning = & handler->bins_SEToverPT;
        // for all indexes
        for (ParSetIdentifier i = 0; i < handler->GetMaxIndex(); i++){
            // main hist
            BookHist(i, "full"  ,binning);
            BookHist(i, "fast"  ,binning);
            BookHist(i, "ratio" ,binning);
            // main function
            BookFunction(i);
            // scales
            m_finalScale[i]=-999.;
            handler->GetIndices(i, i_pt,i_eta,i_lumi,i_upar,i_ut);
            if (i_pt==0){
                m_finalScaleMax[i] =-999.;
                BookHist(i,"full_allpt", & handler->bins_SET );
                BookHist(i,"fast_allpt", & handler->bins_SET );
            }
            if (i_pt==0&&i_eta==0){
                m_finalScaleMaxGlobal[i]=-999.;
            }
        }
    }

    void ParameterEstimator::BookHist ( ParSetIdentifier i, string base , EffBins * binning ){
        string name = get_list_name ( i, base, "h_");
        if ( l_hists(name.c_str()) != 0) throw invalid_argument(name.insert(0,"trying to book already existing histogram: ").c_str());
        string title = name;
        title.append(";");
        title.append(binning->GetTitle());
        title.append(";entries");
        TH1D * h = new TH1D ( name.c_str(), title.c_str(), binning->GetNBins(), binning->GetBinArray() );
        l_hists.Add(h);
    }

    void ParameterEstimator::BookFunction(ParSetIdentifier i){
        string name = get_list_name(i,"","f_");
        TF1 * fcn = 0;
        switch (handler->fun_type){
            case SETeff::p3_20  : fcn = new TF1( name.c_str(), SETeff::fun_p3_20  , 0, 250, 3 ) ; break ;
            case SETeff::p5_200 : fcn = new TF1( name.c_str(), SETeff::fun_p5_200 , 0, 250, 5 ) ; break ;
            case SETeff::p5_20  : fcn = new TF1( name.c_str(), SETeff::fun_p5_20  , 0, 250, 5 ) ; break ;
        }
        l_hists.Add( fcn );
    }

    void ParameterEstimator::FillHistograms(){
        double SET  = 0;
        double pt   = 0;
        double eta  = 0;
        double lumi = 0;
        double upar = 0;
        double ut   = 0;
        /// loop fullMC
        for (Long64_t i_entry=0; i_entry < t_fullMC.GetEntries(); i_entry++){
            t_fullMC.GetEntry(i_entry);
            if (UseFullSET) SET  = t_fullMC.scalaret ;
            else {
                EventIdentifier evID(t_fullMC.run, t_fullMC.evt);
                ZBmap::iterator it=m_zblib.find(evID);
                if (it==m_zblib.end()) throw runtime_error("not in library!");
                else SET=it->second;
            }
            if (UsePhysEta) eta = t_fullMC.eta    ;
            else            eta = t_fullMC.deteta ;
            eta = fabs(eta);
            pt   = t_fullMC.elepT    ;
            lumi = t_fullMC.lumi     ;
            upar = t_fullMC.upara    ;
            ut   = t_fullMC.ut       ;
            FillStdHist("full", SET, pt, eta, lumi, upar, ut);
            ParSetIdentifier i=handler->GetIndex(-999.,eta,lumi,upar,ut);
            GetHistogram(i,"full_allpt") -> Fill(SET);
        }
        /// loop PMCS
        for (Long64_t i_entry=0; i_entry < t_PMCS.GetEntries(); i_entry++){
            t_PMCS.GetEntry(i_entry);
            if (UseFullSET) SET  = t_PMCS.scalaret ;
            else {
                EventIdentifier evID(t_PMCS.run, t_PMCS.evt);
                ZBmap::iterator it=m_zblib.find(evID);
                if (it==m_zblib.end()) throw runtime_error("not in library!");
                else SET=it->second;
            }
            if (UsePhysEta) eta = t_PMCS.eta    ;
            else            eta = t_PMCS.deteta ;
            eta = fabs(eta);
            pt   = t_PMCS.elepT    ;
            lumi = t_PMCS.lumi     ;
            upar = t_PMCS.upara    ;
            ut   = t_PMCS.ut       ;
            FillStdHist("fast", SET, pt, eta, lumi, upar, ut);
            ParSetIdentifier i=handler->GetIndex(-999.,eta,lumi,upar,ut);
            GetHistogram(i,"fast_allpt") -> Fill(SET);
        }
    }

    void ParameterEstimator::FillStdHist(string base, double set, double pt, double eta, double lumi, double upar, double ut){
        ParSetIdentifier i = handler -> GetIndex(pt,eta,lumi,upar,ut);
        double val = 0;
        switch (handler->dist_type){
            case SETeff::SET       : val = set    ; break;
            case SETeff::SEToverPT : val = set/pt ; break;
        }
        GetHistogram(i,base)->Fill(val);
    }

    TH1D * ParameterEstimator::GetHistogram(ParSetIdentifier i, string base_name){
        TH1D * out =0;
        string name = get_list_name(i,base_name,"h_");
        out = (TH1D *) l_hists(name.c_str());
        if (out == 0) {
            name.insert(0,"there is no histogram with name '");
            name.append("'");
            throw invalid_argument(name);
        }
        return out;
    }

    TF1 * ParameterEstimator::GetFunction(ParSetIdentifier i){
        TF1 * out =0;
        string name = get_list_name(i,"","f_");
        out = (TF1 *) l_hists(name.c_str());
        if (out == 0) {
            name.insert(0,"there is no function with name '");
            name.append("'");
            throw invalid_argument(name);
        }
        return out;
    }

    string ParameterEstimator::get_list_name(ParSetIdentifier i, string base_name, string out){
        out.append(base_name);
        out.append("_");
        // by bin indices
        size_t i_pt   =0;
        size_t i_eta  =0;
        size_t i_lumi =0;
        size_t i_upar =0;
        size_t i_ut   =0;
        handler->GetIndices(i,
                i_pt   ,
                i_eta  ,
                i_lumi ,
                i_upar ,
                i_ut   
                );
        out.append(
                GetParNameByIndices(
                    i_pt   ,
                    i_eta  ,
                    i_lumi ,
                    i_upar ,
                    i_ut   
                    )
                );
        // by par identifier
        //out.append(ToString(i));
        return out;
    }

    void ParameterEstimator::MakeEfficiencies(){
        for (size_t i_lumi =0; i_lumi < handler -> bins_lumi .GetNBins(); i_lumi ++ ){
        for (size_t i_upar =0; i_upar < handler -> bins_upar .GetNBins(); i_upar ++ ){
        for (size_t i_ut   =0; i_ut   < handler -> bins_ut   .GetNBins(); i_ut   ++ ){
        for (size_t i_pt   =0; i_pt   < handler -> bins_pt   .GetNBins(); i_pt   ++ ){
        for (size_t i_eta  =0; i_eta  < handler -> bins_eta  .GetNBins(); i_eta  ++ ){
            ParSetIdentifier i = handler->GetIndex( i_pt   , i_eta  , i_lumi , i_upar , i_ut );
            TH1D * h_full  = GetHistogram(i,"full"  );
            TH1D * h_fast  = GetHistogram(i,"fast"  );
            TH1D * h_ratio = GetHistogram(i,"ratio" );
            make_scaled_ratio(h_full,h_fast,h_ratio);
        } } } } }
    }

    void ParameterEstimator::make_scaled_ratio(TH1D* A, TH1D* B, TH1D * C, double scale){
        scale*= A->Integral() / B->Integral();
        // ratio bin by bin
        size_t N_bins = A->GetXaxis()->GetNbins()+1;
        for (size_t i_bin = 0; i_bin <= N_bins; i_bin++){
            double ratio = 0;
            double ratio_error = 0;
            if ( (B->GetBinContent(i_bin)>0)  && (A->GetBinContent(i_bin)!=0)  ){
                ratio  = A->GetBinContent(i_bin);
                ratio /= B->GetBinContent(i_bin);
                ratio /= scale;
                ratio_error  = pow( A->GetBinError(i_bin) / A->GetBinContent(i_bin), 2);
                ratio_error += pow( B->GetBinError(i_bin) / B->GetBinContent(i_bin), 2);
                ratio_error  = sqrt(ratio_error);
                ratio_error *= ratio;
            }
            C->SetBinContent (i_bin, ratio       );
            C->SetBinError   (i_bin, ratio_error );
        }
    }

    void ParameterEstimator::FitEfficiencies(){
        float elpt_cut=40.; // increase number of parameters after 40GeV
        for (size_t i_pt   =0; i_pt   < handler -> bins_pt   . GetNBins(); i_pt   ++ ){
        for (size_t i_eta  =0; i_eta  < handler -> bins_eta  . GetNBins(); i_eta  ++ ){
        for (size_t i_lumi =0; i_lumi < handler -> bins_lumi . GetNBins(); i_lumi ++ ){
        for (size_t i_upar =0; i_upar < handler -> bins_upar . GetNBins(); i_upar ++ ){
        for (size_t i_ut   =0; i_ut   < handler -> bins_ut   . GetNBins(); i_ut   ++ ){
            ParSetIdentifier i = handler->GetIndex( i_pt   , i_eta  , i_lumi , i_upar , i_ut );
            TH1D * h_ratio = GetHistogram(i,"ratio");
            TF1  * f_fit = GetFunction(i);
            f_fit->SetParameter(0, 0.);
            f_fit->SetParameter(1, 0.);
            f_fit->SetParameter(2, 0.);
            // fix parameter for lower than elpt_cut
            if (i_pt < handler->bins_pt.GetBin(elpt_cut)){
                f_fit->FixParameter(3,0.);
                f_fit->FixParameter(4,0.);
            } else {
                f_fit->SetParameter(3,0.);
                f_fit->FixParameter(4,0.);
            }
            h_ratio->Fit(f_fit);
            handler->fit_pars.SetParams(f_fit,i);
        } } } } }
    }

    void ParameterEstimator::CalculateScales(){
        for (size_t i_lumi =0; i_lumi < handler -> bins_lumi . GetNBins(); i_lumi ++ ){
        for (size_t i_upar =0; i_upar < handler -> bins_upar . GetNBins(); i_upar ++ ){
        for (size_t i_ut   =0; i_ut   < handler -> bins_ut   . GetNBins(); i_ut   ++ ){
            ParSetIdentifier i_allpt_alleta = handler->GetIndex( 0    , 0     , i_lumi , i_upar , i_ut );
        for (size_t i_eta  =0; i_eta  < handler -> bins_eta  . GetNBins(); i_eta  ++ ){
            ParSetIdentifier i_allpt        = handler->GetIndex( 0    , i_eta , i_lumi , i_upar , i_ut );
        for (size_t i_pt   =0; i_pt   < handler -> bins_pt   . GetNBins(); i_pt   ++ ){
            ParSetIdentifier i              = handler->GetIndex( i_pt , i_eta , i_lumi , i_upar , i_ut );
            m_finalScale[i]  = GetHistogram(i,"full"  ) ->Integral();
            m_finalScale[i] /= ModIntegral(i);
            m_finalScale[i] /= GlobalNorm(i_allpt);
            // find maximal scale
            if (m_finalScaleMax[i_allpt]<m_finalScale[i]) m_finalScaleMax[i_allpt]=m_finalScale[i];
            // find maximal global scale
            if ( isOkBins(i_pt, i_eta) && 
                    m_finalScaleMaxGlobal[i_allpt_alleta] < m_finalScale[i]
                    ) m_finalScaleMaxGlobal[i_allpt_alleta] = m_finalScale[i];
        } } } } }
    }

    bool ParameterEstimator::isOkBins(size_t i_pt, size_t i_eta){
        if ( i_eta == 1 && i_pt == 7) return false;
        if ( i_eta == 3 && i_pt == 7) return false;
        if ( i_eta == 4 && i_pt == 7) return false;
        if ( i_eta == 1 && i_pt == 0) return false;
        if ( i_eta == 2 && i_pt == 0) return false;
        return true;
    }


    double ParameterEstimator::ModIntegral(ParSetIdentifier i){
        if ( m_modintegral.count(i) == 0){ // if not exist then calculate it
            TH1D * h_fast = GetHistogram(i,"fast");
            m_modintegral[i] = 0;
            for ( Int_t i_bin=0; i_bin <= h_fast->GetXaxis()->GetNbins(); i_bin++){
                double val    = h_fast->GetBinContent(i_bin);
                double center = h_fast->GetXaxis()->GetBinCenter(i_bin);
                m_modintegral[i] += val * GetFunction(i)->Eval(center);
            }
        }
        return m_modintegral[i];
    }

    double ParameterEstimator::GlobalNorm(ParSetIdentifier i){
        if ( m_globalnorm.count(i) == 0){ // if not exist then calculate it
            m_globalnorm[i]  = GetHistogram(i,"full_allpt") ->Integral();
            m_globalnorm[i] /= GetHistogram(i,"fast_allpt") ->Integral();
        }
        return m_globalnorm[i];
    }

    void ParameterEstimator::SaveStudy(string outfile){
        TFile *f = TFile::Open(outfile.c_str(), "RECREATE");
        TIter next(&l_hists);
        TObject * obj = 0;
        while ( (obj = next()) ) obj->Write();
        f->Write();
        f->Close();
    }

    /// namespace utilities

    ostream& operator<< (ostream& os, const ConfigFile& in){
        os << "fit_fun   " << ToString(in.fun_type  ) << "\n";
        os << "fit_dist  " << ToString(in.dist_type ) << "\n";
        os << "\n";
        os << "bins_SET       " << ToString(in.bins_SET       ) << "\n";
        os << "bins_SEToverPT " << ToString(in.bins_SEToverPT ) << "\n";
        os << "bins_elpt      " << ToString(in.bins_pt        ) << "\n";
        os << "bins_eleta     " << ToString(in.bins_eta       ) << "\n";
        os << "bins_lumi      " << ToString(in.bins_lumi      ) << "\n";
        os << "bins_upar      " << ToString(in.bins_upar      ) << "\n";
        os << "bins_ut        " << ToString(in.bins_ut        ) << "\n";
        os << "\n";

        // for all pars
        for (size_t i_pt   =0; i_pt   < in. bins_pt   .GetNBins(); i_pt   ++ ){
        for (size_t i_eta  =0; i_eta  < in. bins_eta  .GetNBins(); i_eta  ++ ){
        for (size_t i_lumi =0; i_lumi < in. bins_lumi .GetNBins(); i_lumi ++ ){
        for (size_t i_upar =0; i_upar < in. bins_upar .GetNBins(); i_upar ++ ){
        for (size_t i_ut   =0; i_ut   < in. bins_ut   .GetNBins(); i_ut   ++ ){
            os << GetParNameByIndices( i_pt   , i_eta  , i_lumi , i_upar , i_ut   );
            os << "  ";
            os << ToString(in.fit_pars.GetParVec( i_pt   , i_eta  , i_lumi , i_upar , i_ut   ));
            os << "\n";
        } } } } }

        return os;
    }

    string ToString(EffFunType in){
        switch (in){
            case p3_20  : return string("p3_20"  ); break;
            case p5_200 : return string("p5_200" ); break;
            case p5_20  : return string("p5_20"  ); break;
        }
        return string("");
    }

    string ToString(EffDist in){
        switch (in){
            case SET       : return string("SET"       ); break;
            case SEToverPT : return string("SEToverPT" ); break;
        }
        return string("");
    }

    string ToString(const vector<double> &data){
        string out;
        for(vector<double>::const_iterator it=data.begin(); it!=data.end(); ++it){
            if(it!=data.begin()) out.append("  ");
            out.append(ToString(*it));
        }
        return out;
    }

    string ToString(size_t i) {
#ifdef NoC11
        char buffer[50];
        sprintf(buffer, "%d",i);
        return string(buffer);
#else
        return to_string(i);
#endif
    };

    string ToString(double a) {
#ifdef NoC11
        char buffer[50];
        sprintf(buffer, "%f",a);
        return string(buffer);
#else
        return to_string(a);
#endif
    };

    double StrinToDouble(string a){
#ifdef NoC11
        return atof(a.c_str());
#else
        return stod(a);
#endif

    }

    void ToLowcase(string & s){
        transform(s.begin(),s.end(),s.begin(),::tolower);
    }

    bool StartWithNoCase(string s1,string s2){
        ToLowcase(s1);
        ToLowcase(s2);
        if ( 0 == s1.compare(0,s2.size(),s2)) return true;
        return false;
    }

    string GetParNameByIndices( size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut ){
        string out;
        out.append("pt"   ); out.append(ToString(i_pt   ));
        out.append("eta"  ); out.append(ToString(i_eta  ));
        out.append("lumi" ); out.append(ToString(i_lumi ));
        out.append("upar" ); out.append(ToString(i_upar ));
        out.append("ut"   ); out.append(ToString(i_ut   ));
        return out;
    }

    vector<string> SplitString(string &source,bool keepEmpty){
        vector<string> results;
        size_t prev = 0;
        size_t next = 0;
        while ((next = NextWhitespace(source,prev)) != std::string::npos) {
            if (keepEmpty || (next - prev != 0)) {
                results.push_back(source.substr(prev, next - prev));
            }
            prev = next + 1;
        }
        if (prev < source.size()) { // put last item.. if any
            results.push_back(source.substr(prev));
        }
        return results;
    }

    size_t NextWhitespace(string &source, size_t prev){
        size_t next = string::npos;
        string delims = " \t";
        for (size_t i_del = 0; i_del < delims.size(); i_del++){
            size_t tmp_p = source.find_first_of(delims[i_del], prev);
            if (tmp_p < next) next = tmp_p;
        }
        return next;
    }


    void ConvertDumpToTree_fullMC (string text_dump, string root_dump, bool isPMCS){
        TFile *fout = TFile::Open(root_dump.c_str(),"RECREATE");
        TTree* tree = new TTree("dump","dump of SET information from fullMC events");
        if (isPMCS) tree->ReadFile(text_dump.c_str(), "run/I:evt/I:lumi/D:scalaret/D:scalaretZB/D:scalaretMB/D:scalaretHard/D:elepT/D:deteta/D:eta/D:upara/D:ut/D");
        else tree->ReadFile(text_dump.c_str(), "run/I:evt/I:lumblk/I:lumi/D:scalaret/D:elepT/D:deteta/D:eta/D:upara/D:ut/D");
        fout->Write();
        fout->Close();
    }

    void ConvertDumpToTree_PMCS   (string text_dump, string root_dump){
       ConvertDumpToTree_fullMC(text_dump, root_dump, true);
    }

    double fun_p3_20(double* x, double* par) {
        double xi=x[0]/20.;
        double res=
            1.+
            par[0]*xi+
            par[1]*xi*xi+
            par[2]*xi*xi*xi;
        return res;
    }


    double fun_p5_200(double* x, double* par) {
        double xi=x[0]/200.;
        double res=
            1.+
            par[0]*xi+
            par[1]*xi*xi+
            par[2]*xi*xi*xi+
            par[3]*xi*xi*xi*xi+
            par[4]*xi*xi*xi*xi*xi;
        return res;
    }


    double fun_p5_20(double* x, double* par) {
        double xi=x[0]/20.;
        double res=
            1.+
            par[0]*xi+
            par[1]*xi*xi+
            par[2]*xi*xi*xi+
            par[3]*xi*xi*xi*xi+
            par[4]*xi*xi*xi*xi*xi;
        return res;
    }

}

#endif //_SETeff_CXX
