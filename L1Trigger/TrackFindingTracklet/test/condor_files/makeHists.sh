#!/bin/bash


#################### CHANGE THESE ####################

# SAMPLE_NAME=TTbar_PU200_D88
# SAMPLE_NAME=TTbar_noPU_D88
# SAMPLE_NAME=TRK2026D88PU200MB2-v5
SAMPLE_NAME=TTbar_D88_PU200_v5
# SAMPLE_NAME=test

######################################################


hadd $SAMPLE_NAME/$SAMPLE_NAME.root $SAMPLE_NAME/root_files/*.root

# make histograms
./../makeHists.csh $SAMPLE_NAME/$SAMPLE_NAME.root

# move output into its own directory
tar -czf TrkPlots.tgz TrkPlots/
mv TrkPlots/ $SAMPLE_NAME
mv TrkPlots.tgz $SAMPLE_NAME
mv MVA_plots/ $SAMPLE_NAME
mv FitResults/ $SAMPLE_NAME
mv results.out $SAMPLE_NAME

exit
