#ifndef runtest_cpp
#define runtest_cpp

/**
 * @file runtest.cpp
 * Description of this file
 *
 * @brief A brief description
 *
 * @author cuto <Jakub.Cuth@cern.ch>
 * @date 2014-10-01
 */

#include "SETeff.h"

#include <iostream>
#include <stdexcept>

using std::runtime_error;
using std::cout;
using std::endl;


void test_config(){
    SETeff::EffHandler eff;
    try {
        eff.LoadConfig("data/config_example.txt");
    } catch (runtime_error &e){
        cout << "something went wrong with loading configuration" << endl;
        cout << e.what() << endl;
        cout << "filling missing parameters to one" << endl;
        eff.SetMissingParamsToOne();
    }
    cout << eff;
    cout << " value of efficiency for SET 30 " << eff.GetEfficiency(30,1.,0.,0.,-1.0,0.) << endl;

    eff.test_indeces(cout);
    cout << endl;

    //SETeff::ParSetIdentifier i = eff.GetIndex(25., 0.3, 3, -1, 1);

}

void test_estimate_parameters(){
    // prepare trees (instead of text files)
    //SETeff::ConvertDumpToTree_fullMC ("data/Dump_fullMC.txt" , "data/Dump_fullMC.root" );
    //SETeff::ConvertDumpToTree_PMCS   ("data/Dump_PMCS.txt"   , "data/Dump_PMCS.root"   );

    //SETeff::ConvertDumpToTree_fullMC ( "/prj_root/7011/wmass1/stark/RunIIcAnal/CAFprodJan2014/cabout/Results__MCWenu_RunIIb3_gamVeto_Dump/Dump.txt"                        , "data/Dump_fullMC.root" );
    //SETeff::ConvertDumpToTree_PMCS   ( "../fromJan/Dump__MCwenu_PMCS09_IIb3_gamVeto_newSETeffC2_upslM001_upAdj2Int_noSETeff_Dump_WMass_pythia_wenu_6409ta_v1_snap_002_4822989.txt" , "data/Dump_PMCS.root"   );

    SETeff::EffHandler eff;
    try {
        //eff.LoadConfig("data/config_example.txt");
        eff.LoadConfig("data/config_testJan.txt");
    } catch (runtime_error &e){
        cout << "something went wrong with loading configuration" << endl;
        cout << e.what() << endl;
        cout << "filling missing parameters to one" << endl;
        eff.SetMissingParamsToOne();
    }

    //eff.LoadZBlib  ("data/ZBlib.root");

    cout << "load zb" << endl;
    eff.LoadZBlib  ("/prj_root/7011/wmass1/stark/RunIIcAnal/PMCS02/mitraillette/make_extras/extras/jenny/MC/MBZBLibrary/Lib_RunIIb3_21Msinglenu_killcell_dq.root");
    cout << "load full" << endl;
    eff.LoadFullMC ("data/Dump_fullMC.root");
    cout << "load fast" << endl;
    eff.LoadPMCS   ("data/Dump_PMCS.root");

    cout << "estimating parameters" << endl;
    eff.EstimateParameters();

    cout << "saving" << endl;
    eff.SaveConfig("data/config_test.txt");
    eff.SaveStudy("data/eff_study.root");
    cout << "old dump" << endl;
    eff.OldOutput(cout);
    cout << endl;
    cout << "new dump" << endl;
    cout << eff;
    cout << endl;

}

/**
 * Description of main program
 *
 */
int main(int argc, const char * argv[]){

    //test_config();
    test_estimate_parameters();

    return 0;
}


#endif // runtest_cpp
