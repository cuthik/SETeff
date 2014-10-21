#!/usr/bin/python
# -*- coding: utf-8 -*- 

## Documentation for file
#
# More details. 
#
# @file makePlots.py
# @author cuto <Jakub.Cuth@cern.ch>
# @date 2014-10-20

import sys
sys.argv.append("-b")

from PlotTools import *

def ApplySimplyStyle (pl, funs):
    pl.AutoSetStyle(funs)
    for f in funs :
        f.SetFillStyle(0)
        f.SetMarkerSize(0)
        f.SetRange(0,20)

def plot_eff (outname, filenames):
    pl = PlotTools()
    binname= "_pt0eta0lumi0upar0ut0"
    # get file
    #fname = files[0]
    fname = filenames[0][1]
    # get histogram
    hname = "h_ratio" + binname
    hist = pl.GetHist(fname, hname)
    # get functions
    funs = list()
    for keyval in filenames :
        title = keyval[0]
        fname = keyval[1]
        #funname = "f_" + binname
        #fun = pl.GetFunction(fname,funname)
        fun = pl.GetHist(fname,hname).GetListOfFunctions().At(0)
        # get chi2
        chi2ndof = fun.GetChisquare()/fun.GetNDF()
        fun.SetTitle(title+" = "+str(chi2ndof))
        funs.append(fun)
        pass
    # plot everything
    pl.NewCanvas(outname)
    pl.SetFrameStyle1D([ hist ])
    ApplySimplyStyle(pl,funs)
    hist.Draw("EP")
    for f in funs : 
        #f.SetNpx(1000)
        #f.SetRange(0.,20.)
        #g = TGraph(f)
        #g.Draw("SAME")
        f.Draw("SAME")
        pass
    # legend with chi2
    pl.DrawLegend(funs,"l")
    pl.Save()
    pass

## Documentation for main
#
# More details. 
def main () :

    plot_eff("simple", [
        [ "p2_20" , "data/eff_study_NoBins_p2_20.root" ],
        [ "p3_20" , "data/eff_study_NoBins_p3_20.root" ],
        [ "p4_20" , "data/eff_study_NoBins_p4_20.root" ],
        [ "p5_20" , "data/eff_study_NoBins_p5_20.root" ],
        [ "c3"    , "data/eff_study_NoBins_c3.root"    ],
        [ "c4"    , "data/eff_study_NoBins_c4.root"    ],
        [ "c5"    , "data/eff_study_NoBins_c5.root"    ],
        ]);
    pass

if __name__ == '__main__' :
    main()
    pass

