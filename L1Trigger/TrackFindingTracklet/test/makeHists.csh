#!/bin/tcsh
############################################################################
# Create L1 track histograms & print summary of tracking performance,      #
# by running ROOT macro L1TrackNtuplePlot.C or MiniPlot.C on .root file    #
# from L1TrackNtupleMaker_cfg.py                                           #
#                                                                          #
# To use:                                                                  #
#   ./makeHists.csh rootFileName                                           #
#                                                                          #
# (where rootFileName is the name of the input .root file, including its   #
#   directory name, if it's not in the current one. If rootFileName is not #
#   specified, it defaults to NuGun_PU200_D76.root)                        #
############################################################################

if ($#argv == 0) then
	set rootFileName = "output/NuGun_PU200_D76.root"
	# set rootFileName = "output/ZMM_PU200_D76.root"
else
	set rootFileName = $1
endif

if ( -e $rootFileName) then
	echo "Processing $rootFileName"
else
	echo "ERROR: ROOT file $rootFileName not found"
	exit(1)
endif

# Get directory name
set dirName = `dirname $rootFileName`/
# Get file name without directory name
set fileName = `basename $rootFileName`
# Get stem of filename, removing ".root".
set fileStem = `echo $fileName | awk -F . '{print $1;}'`

# Find plotting macro
eval `scramv1 runtime -csh`
# set plotMacro = $CMSSW_BASE/src/L1Trigger/TrackFindingTracklet/test/L1TrackNtuplePlot.C
set plotMacro = $CMSSW_BASE/src/L1Trigger/TrackFindingTracklet/test/MiniPlot.C
if ( -e $plotMacro ) then
	# Run plotting macro
	if (-e ${dirName}TrackPlots) rm -r ${dirName}TrackPlots
	\root -l -b -q ${plotMacro}'("'${fileStem}'","'${dirName}'")' >! ${dirName}${fileStem}_results.out
	cat ${dirName}${fileStem}_results.out
	echo "Tracking performance summary written to ${dirName}${fileStem}_results.out"
	echo "Histograms written to ${dirName}TrackPlots/"
else if ( -e $plotMacro ) then
else
	echo "ERROR: $plotMacro not found"
	exit(2)
endif

exit
