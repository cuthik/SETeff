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

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TF1.h"
#include "THashList.h"

// STL
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

    // typedefs
    typedef size_t                       ParSetIdentifier;
    typedef map<ParSetIdentifier,double> EffScales;
    typedef pair<int,int>                EventIdentifier;
    typedef map<EventIdentifier,double>  ZBmap;

    // utilities
    string ToString(EffFunType in); ///< stringify 
    string ToString(EffDist in);
    string ToString(const vector<double> &data);
    string ToString(size_t i);
    string ToString(double a);
    double StrinToDouble(string a);
    void ToLowcase(string &s);
    bool StartWithNoCase(string s1,string s2);
    string GetParNameByIndices( size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut );
    vector<string> SplitString(string &source,bool keepEmpty=false);
    size_t NextWhitespace(string &source, size_t prev);
    void ConvertDumpToTree_fullMC (string text_dump, string root_dump, bool isPMCS=false);
    void ConvertDumpToTree_PMCS   (string text_dump, string root_dump);

    // efficiency functions
    double fun_p3_20  ( double* x, double* par);
    double fun_p5_200 ( double* x, double* par);
    double fun_p5_20  ( double* x, double* par);

    // classes
    class EffHandler;

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
            EffBins(const char * _title="x"): vector<double>(), title(_title) {
                push_back(0.);
                push_back(1.);
            }
            ~EffBins(){};

            /// Setters
            void SetByVector(vector<double> &vec);
            void SetByArray(size_t nbins, double* list);
            void SetByRange(size_t nbins=0, double low=0., double high=0.);

            /// Getters
            size_t GetNBins() const;
            double * GetBinArray();
            double Val(double in);
            size_t GetBin(double in) const;
            double Underflow() ; // lowest accepted value
            double Overflow()  ; // highghest accepted value
            string GetTitle() const { return title;};
        private :
            string title;
    };

    class EffParams:
        public map<ParSetIdentifier,vector<double> >
    {
        public:
            EffParams();
            ~EffParams(){};

            ///Setters
            inline void SetHandler(EffHandler * _h) {handler = _h;};
            void SetParams(TF1 * fun, ParSetIdentifier i);
            void SetParams(vector<double> &vec, double pt, double eta, double lumi, double upar, double ut);
            void SetParams(vector<double> &vec, size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut);

            ///Getters
            double * GetParArray(size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut);
            double * GetParArray(double pt, double eta, double lumi, double upar, double ut);
            vector<double> GetParVec(size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut) const;
            vector<double> GetParVec(double pt, double eta, double lumi, double upar, double ut) const;

        private:
            // handler pointer
            EffHandler * handler;

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
            EffScales fit_scales;

        protected :
            // parent
            EffHandler * handler;

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

    class TreeDump {
        public :
            TreeDump();
            ~TreeDump();

            void LoadTree(string path, bool isPMCS=false);
            Long64_t GetEntries() const;
            void GetEntry(Long64_t i);

            int    run          ;
            int    evt          ;
            int    lumblk       ;
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

        private :
            TFile *f;
            TTree *t;
    };


    class ParameterEstimator {
        public :
            ParameterEstimator();
            ~ParameterEstimator();

            void LoadZBlib  (string path);
            void LoadFullMC (string path);
            void LoadPMCS   (string path);

            void EstimateParameters();
            void SaveStudy(string outfile);

            void OldOutput(ostream &os);

            bool UseFullSET;
            bool UsePhysEta;

        protected :
            // parent
            EffHandler * handler;

            ZBmap    m_zblib;
            TreeDump t_fullMC;
            TreeDump t_PMCS;

            EffScales m_finalScale;
            EffScales m_finalScaleMax;
            EffScales m_finalScaleMaxGlobal;

            EffScales m_modintegral;
            EffScales m_globalnorm;

            THashList l_hists;
            //THashList l_fitfuns;

            void BookHistogramsAndScales();
            void BookHist ( ParSetIdentifier i, string base , EffBins * binning );
            void BookFunction( ParSetIdentifier i);
            void FillHistograms();
            void FillStdHist(string base, double SET, double pt, double eta, double lumi, double upar, double ut);
            TH1D * GetHistogram(ParSetIdentifier i, string base_name);
            TF1  * GetFunction (ParSetIdentifier i);
            void MakeEfficiencies();
            void FitEfficiencies();
            void CalculateScales();
            void make_scaled_ratio(TH1D * A, TH1D *B, TH1D * C, double scale = 1.);
            bool isOkBins(size_t i_pt, size_t i_eta);
            double ModIntegral(ParSetIdentifier i);
            double GlobalNorm(ParSetIdentifier i);

            string get_list_name(ParSetIdentifier, string base_name="", string out="o_");
    };



    /**
     * Wrapping class over all operations with SET efficiencies.
     *
     * Collecting configuration, estimating parameters and efficiency evaulation.
     */
    class EffHandler : public ConfigFile, public ParameterEstimator {
        public:
            EffHandler() {ConfigFile::handler = this; ParameterEstimator::handler = this; };
            ~EffHandler(){};

            void SetMissingParamsToOne(); ///< Auto-fill missing entries.
            void SaveConfig(string filepath) ;

            // Getters
            double GetEfficiency(double SET, double elpt=0, double eta=0, double lumi=0, double upar=0, double ut=0);
            void GetIndices(const ParSetIdentifier i, size_t &i_pt, size_t &i_eta, size_t &i_lumi, size_t &i_upar, size_t &i_ut);
            ParSetIdentifier GetIndex(size_t i_pt, size_t i_eta, size_t i_lumi, size_t i_upar, size_t i_ut) const;
            ParSetIdentifier GetIndex(double pt, double eta, double lumi, double upar, double ut) const;
            ParSetIdentifier GetMaxIndex() const;

            void test_indeces(ostream &os);
    };

}

#endif //_SETeff_H
