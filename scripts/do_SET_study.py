#!/usr/bin/python
# -*- coding: utf-8 -*- 

## Documentation for file
#
# More details. 
#
# @file do_SET_study.py
# @author cuto <Jakub.Cuth@cern.ch>
# @date 2014-09-29

# load ROOT in batch
import sys
sys.argv.append("-b")
from ROOT import gROOT

# init SETeff
gROOT.ProcessLine(".L src/ZBLibraryClass.h++")
gROOT.ProcessLine(".L src/SETeff.h++")
from ROOT import SETeff


class SETeff_study :
    def __init__(s):
        s.fit_fun        = "p3_20" # "p5_200", "p5_20"
        s.fit_dist       = "SET" # "SEToverPT"
        s.bins_upara     = []
        s.bins_elpt      = []
        s.bins_eleta     = []
        s.bins_lumi      = []
        s.bins_SET       = s.bin_array(30,0,500)
        s.bins_SEToverPt = s.bin_array(30,0,20)
        s.fit_par = {
                'pt0eta0lum0upar0ut0' : [1., 1., 1.]
                }
        pass

    # public
    def LoadConfig(s, config_filename="data/config.txt") :
        f = open(config_filename,"r")
        s.read_str(f,"fit_fun")
        s.read_str(f,"fit_dist")
        s.read_array(f,"bins_SET")
        s.read_params(f) # all binning must be readed before
        f.close()
        pass


    def SaveConfig(s, config_filename="data/config.txt") :
        f = open(config_filename, 'w')
        f.write("# generated automatically byt do_SET_study.py ")
        f.write(strftime("%G/%m/%d %H:%M:%S %Z %z\n"))
        f.write("\n")
        f.write("fit_fun     " + s.fit_fun  + "\n")
        f.write("fit_dist    " + s.fit_dist + "\n")
        f.write("\n")
        s.write_bins(f,"bins_SET")
        f.write("\n")
        s.write_fitpars(f)
        f.close()
        pass


    # private
    def bin_array(s, Nbins=0, low="-Inf", high=0.) :
        # no bins -- overf and underf are at bin 0
        if Nbins==0 : return np.array([],'d')
        # one bin is underflow, second is overflow
        if Nbins==1 : return  np.array([low],'d')
        # n bins is first=underf,bins, overflow
        step = float(high-low)/Nbins
        return low + (np.arange(Nbins)+1.)*step
        pass

def config_example(study):
    study.fit_fun        = "p3_20" # "p5_200", "p5_20"
    study.fit_dist       = "SET" # "SEToverPT"
    study.bins_upara     = [ 0 ]
    study.bins_elpt      = [30,35,40,45,50,60,70]
    study.bins_eleta     = [0.2,0.4,0.6,0.8]
    study.bins_lumi      = [2,4,6]
    study.bins_SET       = study.bin_array(30,0,500)
    study.bins_SEToverPt = study.bin_array(30,0,20)

    study.fit_par = {
            'pt0eta0lum0upar0ut0' : [1., 1., 1.]
            }
    pass


def test():
    s = SETeff.Handler()
    s.LoadConfig("data/config_testJan.txt")

    s.ReadZBLibrary ("/prj_root/7011/wmass1/stark/RunIIcAnal/PMCS02/mitraillette/make_extras/extras/jenny/MC/MBZBLibrary/Lib_RunIIb3_21Msinglenu_killcell_dq.root");
    s.ReadFullMC    ("/prj_root/7011/wmass1/stark/RunIIcAnal/CAFprodJan2014/cabout/Results__MCWenu_RunIIb3_gamVeto_Dump/Dump.txt");
    s.ReadPMCS      ("../Dump__MCwenu_PMCS09_IIb3_gamVeto_newSETeffC2_upslM001_upAdj2Int_noSETeff_Dump_WMass_pythia_wenu_6409ta_v1_snap_002_4822989.txt");

    s.EstimateParameters();

    s.SaveConfig("data/config_test.txt")
    s.SaveStudy("data/eff_study.txt")
    pass



## Documentation for main
#
# More details. 
def main():
    test()
    return

    s = SETeff_study()
    s.BookHistograms()

    s.ReadZBLibrary       ("/prj_root/7011/wmass1/stark/RunIIcAnal/PMCS02/mitraillette/make_extras/extras/jenny/MC/MBZBLibrary/Lib_RunIIb3_21Msinglenu_killcell_dq.root");
    s.ReadFullMC_fromDump ("/prj_root/7011/wmass1/stark/RunIIcAnal/CAFprodJan2014/cabout/Results__MCWenu_RunIIb3_gamVeto_Dump/Dump.txt");
    s.ReadPMCS_fromDump   ("../Dump__MCwenu_PMCS09_IIb3_gamVeto_newSETeffC2_upslM001_upAdj2Int_noSETeff_Dump_WMass_pythia_wenu_6409ta_v1_snap_002_4822989.txt");

    s.CalculateEfficiencies();
    s.FitEfficiencies();
    s.SaveStudy();



    usage = PMCSSETeff()
    eff = usage.GetEfficiency(set,lumi,elpt,eleta,upara,ut)

    pass


if __name__ == '__main__' :
    main()
    pass

