if [ "$#" -ne 2 ]
then
  echo please specify the fail scenario and the number of events. exiting program!
  exit
fi

echo fail scenario = $1
echo number of events = $2


#################### CHANGE THESE ####################

# ROOT_FILE=TTbar_PU200_D88
# ROOT_FILE=TTbar_noPU_D88
ROOT_FILE=TRK2026D88PU200MB2-v5
# ROOT_FILE=test

######################################################


NEW_DIR=f$1e$2

# consolidate root files
hadd $ROOT_FILE.root job*/$ROOT_FILE.root

# make histograms
./../makeHists.csh $ROOT_FILE.root

# move output into its own directory
mkdir $NEW_DIR
mv $ROOT_FILE.root $NEW_DIR
mv output_$ROOT_FILE.root $NEW_DIR
mv MVAoutput_$ROOT_FILE.root $NEW_DIR
mv TrkPlots/ $NEW_DIR
mv MVA_plots/ $NEW_DIR
mv FitResults/ $NEW_DIR
mv results.out $NEW_DIR

exit
