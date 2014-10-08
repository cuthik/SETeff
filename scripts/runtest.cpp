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
    SETeff::ConvertDumpToTree_fullMC ("data/Dump_fullMC.txt" , "data/Dump_fullMC.root" );
    SETeff::ConvertDumpToTree_PMCS   ("data/Dump_PMCS.txt"   , "data/Dump_PMCS.root"   );

    SETeff::EffHandler eff;
    eff.LoadConfig("data/config_example.txt");

    eff.LoadZBlib  ("data/ZBlib.root");
    eff.LoadFullMC ("data/Dump_fullMC.root");
    eff.LoadPMCS   ("data/Dump_PMCS.root");

    eff.EstimateParameters();

    eff.SaveConfig("data/config_test.txt");
    eff.SaveStudy("data/eff_study.root");
}

/**
 * Description of main program
 *
 */
int main(int argc, const char * argv[]){

    test_config();

    return 0;
}


#endif // runtest_cpp
