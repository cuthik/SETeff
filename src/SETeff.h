#ifndef _SETeff_H
#define _SETeff_H

/**
 * @file SETeff.h
 *
 * @brief A header file for class SETeff.
 *
 * @author cuto <Jakub.Cuth@cern.ch>
 * @date 2014-09-23
 */

#include <vector>
#include <utility>
#include <map>
#include <string>
#include <fstream>
#include <ostream>
#include <algorithm>

using std::size_t;
using std::vector;
using std::pair;
using std::map;
using std::string;
using std::ifstream;
using std::ostream;

namespace SETeff {

    enum EffFunType { p3_20, p5_200, p5_20}; ///< Fitting function switch.
    enum EffDist { SET, SEToverPT }; ///< Fitting distribution switch.

    // utilities
    string ToString(EffFunType in); ///< stringify 
    string ToString(EffDist in);
    string ToString(const vector<double> &data);
    void ToLowcase(string &s);
    bool StartWithNoCase(string s1,string s2);
    string GetParNameByIndices( size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut );
    vector<string> SplitString(string &source,bool keepEmpty=false);
    size_t NextWhitespace(string &source, size_t prev);

    void ConvertDumpToTree_fullMC (string text_dump, string root_dump);
    void ConvertDumpToTree_PMCS   (string text_dump, string root_dump);

    // efficiency functions
    double fun_p3_20  ( double* x, double* par);
    double fun_p5_200 ( double* x, double* par);
    double fun_p5_20  ( double* x, double* par);


    /** 
     * @class EffBins
     * @brief Handling of binning including overflow and underflows into histogram.
     *
     * The binnig is stored in vector of doubles. The first value represents lowest active value,
     * which means that first bin is always underflow bin. The last value is histogram range value.
     * The previous to last value is last active value, which means that last bin is always overflow.
     *
     * size : bins : descr              : binning scheme
     * 2    : 1    : just counter       : |overflow=underflow|
     * 3    : 2    : +/- distinguish    : |underflow|overflow|
     * >3   : >2   : including outflows : |under|..|n|..|over|
     *
     */
    class EffBins :
        public vector<double>
    {
        public :
            EffBins(): vector<double>() {
                push_back(0.);
                push_back(1.);
            }
            ~EffBins(){};

            /// Setters
            void SetByVector(vector<double> &vec){
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

            void SetByArray(size_t nbins, double* list){
                clear();
                for (size_t i=0; i<=nbins; i++){
                    push_back(list[i]);
                }
            }

            void SetByRange(size_t nbins=0, double low=0., double high=0.){
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
            size_t GetNBins() const{
                if (size()==0) return 1;
                return size()-1;
            }
            double * GetBinArray(){ return & (*begin());}

            double Val(double in){ // test for overflow, underflow
                if (size()==0.) return 0.;
                if (in < Underflow() ) return Underflow();
                if (in > Overflow()  ) return Overflow();
                return in;
            }

            size_t GetBin(double in){
                size_t i = 1;
                for (; i<size(); i++){
                    if (in<at(i)) return i-1;
                }
                return i;
            }

            double Underflow() {return *( begin() );} // lowest accepted value
            double Overflow()  {return *( end()-1 );} // highghest accepted value

    };

    struct ParSetIndices {
        size_t i_pt   = 0;
        size_t i_eta  = 0;
        size_t i_lumi = 0;
        size_t i_upar = 0;
        size_t i_ut   = 0;
    };
    typedef size_t ParSetIdentifier;
    class EffParams:
        public map<ParSetIdentifier,vector<double> >
    {
        public:
            EffParams();
            ~EffParams(){};

            ///Setters
            void SetBinnings( EffBins * _b_pt, EffBins * _b_eta, EffBins * _b_lumi, EffBins * _b_upar, EffBins * _b_ut);
            void SetParams(vector<double> &vec, double pt, double eta, double lumi, double upar, double ut);
            void SetParams(vector<double> &vec, size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut);

            ///Getters
            void GetIndices(const ParSetIdentifier i, size_t &i_pt, size_t &i_eta, size_t &i_lumi, size_t &i_upar, size_t &i_ut);
            ParSetIdentifier GetIndex(size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut) const;
            ParSetIdentifier GetIndex(double pt, double eta, double lumi, double upar, double ut) const;
            double * GetParArray(size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut);
            double * GetParArray(double pt, double eta, double lumi, double upar, double ut);
            vector<double> GetParVec(size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut) const;
            vector<double> GetParVec(double pt, double eta, double lumi, double upar, double ut) const;

        private:
            // binning pointers
            EffBins *b_pt;
            EffBins *b_eta;
            EffBins *b_lumi;
            EffBins *b_upar;
            EffBins *b_ut;

            double * GetParArray(ParSetIdentifier i);
            vector<double> GetParVec(ParSetIdentifier i) const;
            void test_key(ParSetIdentifier key) const;
    };

    class ConfigFile {
        public :
            ConfigFile();
            ~ConfigFile(){};

            void LoadConfig(const char * path);

            // read config methods
            string GetString(string name);
            vector<double> GetVecDouble(string name);
            friend ostream& operator<< (ostream& os, const ConfigFile& in);

        protected :
            // config
            EffFunType fun_type;
            EffDist dist_type;

            // fit dist bins
            EffBins bins_SET;
            EffBins bins_SEToverPT;

            // fit dependency bins
            EffBins bins_pt;
            EffBins bins_eta;
            EffBins bins_lumi;
            EffBins bins_upar;
            EffBins bins_ut;

            // container of parameter values
            EffParams fit_pars;

        private :
            // read helpers;
            ifstream f;
            vector<string> data;

            // read methods
            void open(string conf_filename);
            void close();
            void config_bins(string bin_name, EffBins * bins );
            void config_fit_fun();
            void config_fit_dist();
            void config_fit_pars();
            void find_config(string config_name);
            bool line_config_is(string conf_name);
            void remove_comments(string &line);
            size_t next_delim(string &source, size_t prev);
    };


    typedef pair<int,int> EventIdentifier;
    typedef map<EventIdentifier,double> ZBmap;
    class ParameterEstimator {
        public :
            ParameterEstimator();
            ~ParameterEstimator();

            void LoadZBlib  (string path);
            void LoadFullMC (string path);
            void LoadPMCS   (string path);

            virtual void EstimateParameters();
            void MakeEfficiencies();
            void SaveStudy(string outfile);

        protected :
            ConfigFile * config;
            EffBins * b_pt   ;
            EffBins * b_eta  ;
            EffBins * b_lumi ;
            EffBins * b_upar ;
            EffBins * b_ut   ;

            ZBmap   m_zblib;
            //TTree * t_fullMC;
            //TTree * t_PMCS;

            //THashList hists;
    };



    /**
     * Description of class.
     *
     * Longer description of class
     */
    class EffHandler : public ConfigFile, public ParameterEstimator {
        public:
            EffHandler() {};
            ~EffHandler(){};

            double GetEfficiency(double SET, double elpt=0, double eta=0, double lumi=0, double upar=0, double ut=0);
            void SetMissingParamsToOne();
            void SaveConfig(string filepath) ;
            void EstimateParameters(){
                ParameterEstimator::config = (ConfigFile *) this;
                ParameterEstimator::EstimateParameters();
            }

            void test_indeces(ostream &os);

    };

}

#endif //_SETeff_H
