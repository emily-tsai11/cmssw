submitDataset () {
  crab submit -c crab_submitter.py \
  General.requestName=$YEAR\_$SAMPLE \
  JobType.inputFiles=[\"$CFG\"] \
  JobType.pyCfgParams=[\"year=$YEAR\",\"type=$TYPE\"] \
  Data.inputDataset=$DATASET \
  Data.outputDatasetTag=$YEAR
}
