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

#include <stdexcept>
#include <ctime>

using std::runtime_error;
using std::to_string;
using std::stod;
using std::transform;
using std::ofstream;

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
        outf.open(filepath);
        outf << "# This file has been generated automatically " ;
        outf << ctime(&now) << "\n";
        //f << (ConfigFile) *this;
        outf.close();
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
                ParSetIdentifier i = fit_pars.GetIndex(
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
                fit_pars.GetIndices(i,
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

    /// Config file

    ConfigFile::ConfigFile() :
        fun_type  (SETeff::p3_20),
        dist_type (SETeff::SET)
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
        fit_pars.SetBinnings( & bins_pt  , & bins_eta , & bins_lumi, & bins_upar, & bins_ut  );
        // loop all posible bins where parameters are estimated -- this is estimated from binning definition
        for (size_t i_pt   =0; i_pt   < bins_pt   .GetNBins(); i_pt   ++ ){
        for (size_t i_eta  =0; i_eta  < bins_eta  .GetNBins(); i_eta  ++ ){
        for (size_t i_lumi =0; i_lumi < bins_lumi .GetNBins(); i_lumi ++ ){
        for (size_t i_upar =0; i_upar < bins_upar .GetNBins(); i_upar ++ ){
        for (size_t i_ut   =0; i_ut   < bins_ut   .GetNBins(); i_ut   ++ ){
                            string par_name = GetParNameByIndices( i_pt   , i_eta  , i_lumi , i_upar , i_ut   );
                            vector<double> par_vec = GetVecDouble(par_name);
                            fit_pars.SetParams(par_vec,  i_pt   , i_eta  , i_lumi , i_upar , i_ut   );
                        }
                    }
                }
            }
        }
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
            out.push_back(stod(*it));
        }
        return out;
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
        b_pt   ( 0 ),
        b_eta  ( 0 ),
        b_lumi ( 0 ),
        b_upar ( 0 ),
        b_ut   ( 0 )
    {
    };


    void EffParams::SetBinnings( EffBins * _b_pt, EffBins * _b_eta, EffBins * _b_lumi, EffBins * _b_upar, EffBins * _b_ut){
        b_pt   = _b_pt   ;
        b_eta  = _b_eta  ;
        b_lumi = _b_lumi ;
        b_upar = _b_upar ;
        b_ut   = _b_ut   ;
    }

    void EffParams::SetParams(vector<double> &vec, double pt, double eta, double lumi, double upar, double ut){
        ParSetIdentifier i = GetIndex(pt,eta,lumi,upar,ut);
        map<ParSetIdentifier,vector<double> >::operator[](i)=vec;
    }

    void EffParams::SetParams(vector<double> &vec, size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut){
        ParSetIdentifier i = GetIndex(i_pt,i_eta,i_lumi,i_upar,i_ut);
        map<ParSetIdentifier,vector<double> >::operator[](i)=vec;
    }

    void EffParams::GetIndices(const ParSetIdentifier i, size_t &i_pt, size_t &i_eta, size_t &i_lumi, size_t &i_upar, size_t &i_ut){
        ParSetIdentifier index = i;
        int base = 1;
        // get max
        size_t N_pt   = b_pt   -> GetNBins();
        size_t N_eta  = b_eta  -> GetNBins();
        size_t N_lumi = b_lumi -> GetNBins();
        size_t N_upar = b_upar -> GetNBins();
        size_t N_ut   = b_ut   -> GetNBins();
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


        //base /= N_ut   ; i_ut   = index % base; index -= i_ut    * base;
        //base /= N_upar ; i_upar = index % base; index -= i_upar  * base;
        //base /= N_lumi ; i_lumi = index % base; index -= i_lumi  * base;
        //base /= N_eta  ; i_eta  = index % base; index -= i_eta   * base;
        //base /= N_pt   ; i_pt   = index % base; index -= i_pt    * base;

       //printf "
           //index  =%i
           //base   =%i
           //i_ut   =%i
           //i_upar =%i
           //i_lumi =%i
           //i_eta  =%i
           //i_pt   =%i
           //",
           //index  ,
           //base   ,
           //i_ut   ,
           //i_upar ,
           //i_lumi ,
           //i_eta  ,
           //i_pt   

       //printf " index  =%i base   =%i i_ut   =%i i_upar =%i i_lumi =%i i_eta  =%i i_pt   =%i ", index  , base   , i_ut   , i_upar , i_lumi , i_eta  , i_pt   


    }

    ParSetIdentifier EffParams::GetIndex(size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut) const{
        ParSetIdentifier index = 0;
        int base = 1;
        index+= i_pt   * base; base *= b_pt   -> GetNBins();
        index+= i_eta  * base; base *= b_eta  -> GetNBins();
        index+= i_lumi * base; base *= b_lumi -> GetNBins();
        index+= i_upar * base; base *= b_upar -> GetNBins();
        index+= i_ut   * base; base *= b_ut   -> GetNBins();
        return index;
    }

    ParSetIdentifier EffParams::GetIndex(double pt, double eta, double lumi, double upar, double ut) const{
        return GetIndex(
                b_pt   -> GetBin( pt   ) ,
                b_eta  -> GetBin( eta  ) ,
                b_lumi -> GetBin( lumi ) ,
                b_upar -> GetBin( upar ) ,
                b_ut   -> GetBin( ut   ) 
                );
    }

    double * EffParams::GetParArray(ParSetIdentifier i){
        test_key(i);
        return (double*) & (at(i)[0]);
    }

    double * EffParams::GetParArray(size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut){
        return GetParArray(GetIndex(i_pt,i_eta,i_lumi,i_upar,i_ut));
    }

    double * EffParams::GetParArray(double pt, double eta, double lumi, double upar, double ut){
        return GetParArray(GetIndex(pt,eta,lumi,upar,ut));
    }

    vector<double> EffParams::GetParVec(ParSetIdentifier i) const{
        test_key(i);
        return at(i);
    }

    vector<double> EffParams::GetParVec(size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut) const{
        return GetParVec(GetIndex(i_pt,i_eta,i_lumi,i_upar,i_ut));
    }

    vector<double> EffParams::GetParVec(double pt, double eta, double lumi, double upar, double ut) const{
        return GetParVec(GetIndex(pt,eta,lumi,upar,ut));
    }


    void EffParams::test_key(ParSetIdentifier key) const{
        if (count(key)==0){
            string what = "Parameter map has no key ";
            what.append(to_string(key));
            throw runtime_error(what.c_str());
        }
        // if ( at(key).size() != 5){
        //     string what = "Parameter map has key with no items ";
        //     what.append(to_string(key));
        //     throw runtime_error(what.c_str());
        // }
    }

    /// Parameter Estimator

    ParameterEstimator::ParameterEstimator() {
    }

    ParameterEstimator::~ParameterEstimator() {
    }

    void ParameterEstimator::LoadZBlib  (string path){
    }

    void ParameterEstimator::LoadFullMC (string path){
    }

    void ParameterEstimator::LoadPMCS   (string path){
    }

    void ParameterEstimator::EstimateParameters(){
        // the configuration file have to be loaded before
        //BookHistograms();
        //FillHistograms();
        MakeEfficiencies();
        //FitEfficiencies();
    }

    void ParameterEstimator::MakeEfficiencies(){
        for (size_t i_pt   =0; i_pt   < b_pt   -> GetNBins(); i_pt   ++ ){
        for (size_t i_eta  =0; i_eta  < b_eta  -> GetNBins(); i_eta  ++ ){
        for (size_t i_lumi =0; i_lumi < b_lumi -> GetNBins(); i_lumi ++ ){
        for (size_t i_upar =0; i_upar < b_upar -> GetNBins(); i_upar ++ ){
        for (size_t i_ut   =0; i_ut   < b_ut   -> GetNBins(); i_ut   ++ ){

        } } } } }

    }

    void ParameterEstimator::SaveStudy(string outfile){
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
            if(it!=data.begin()) out.append(" ");
            out.append(to_string(*it));
        }
        return out;
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
        out.append("pt"   ); out.append(to_string(i_pt   ));
        out.append("eta"  ); out.append(to_string(i_eta  ));
        out.append("lumi" ); out.append(to_string(i_lumi ));
        out.append("upar" ); out.append(to_string(i_upar ));
        out.append("ut"   ); out.append(to_string(i_ut   ));
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


    void ConvertDumpToTree_fullMC (string text_dump, string root_dump){

    }

    void ConvertDumpToTree_PMCS   (string text_dump, string root_dump){

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
