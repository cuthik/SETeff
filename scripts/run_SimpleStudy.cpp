#ifndef run_SimpleStudy_cpp
#define run_SimpleStudy_cpp

/**
 * @file run_SimpleStudy.cpp
 * Description of this file
 *
 * @brief A brief description
 *
 * @author cuto <Jakub.Cuth@cern.ch>
 * @date 2014-10-20
 */

#include "SETeff.h"

#include <iostream>
#include <stdexcept>

using std::runtime_error;
using std::cout;
using std::endl;


void nobins_poly(string title, SETeff::EffFunType ftype){
    cout << title << endl;
    SETeff::EffHandler eff;
    try {
        eff.LoadConfig("data/config_NoBinsPoly.txt");
    } catch (runtime_error &e){
        cout << "something went wrong with loading configuration" << endl;
        cout << e.what() << endl;
        cout << "filling missing parameters to one" << endl;
        eff.SetMissingParamsToOne();
    }

    eff.fun_type = ftype;

    cout << "load zb" << endl;
    eff.LoadZBlib  ("data/ZBlib.root");
    cout << "load full" << endl;
    eff.LoadFullMC ("data/Dump_fullMC.root");
    cout << "load fast" << endl;
    eff.LoadPMCS   ("data/Dump_PMCS.root");

    cout << "estimating parameters" << endl;
    eff.EstimateParameters();

    cout << "saving" << endl;
    string tmp = "data/config_NoBins_"; tmp += title; tmp += ".txt" ;
    eff.SaveConfig(tmp);
    tmp = "data/eff_study_NoBins_"; tmp += title; tmp += ".root" ;
    eff.SaveStudy(tmp);
    //cout << "old dump" << endl;
    //eff.OldOutput(cout);
    //cout << endl;
    //cout << "new dump" << endl;
    //cout << eff;
    //cout << endl;

}

/**
 * Description of main program
 *
 */
int main(int argc, const char * argv[]){

    //nobins_poly("p2_20" , SETeff::p2_20 );
    //nobins_poly("p3_20" , SETeff::p3_20 );
    //nobins_poly("p4_20" , SETeff::p4_20 );
    //nobins_poly("p5_20" , SETeff::p5_20 );
    //nobins_poly("c3"    , SETeff::c3    );
    nobins_poly("c4"    , SETeff::c4    );
    nobins_poly("c5"    , SETeff::c5    );

    return 0;
}


#endif // run_SimpleStudy_cpp
