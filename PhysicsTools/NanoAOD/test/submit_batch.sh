source submit_function.sh

submitBatch() {
  cat ${YEAR}_${TYPE}.list | while read LINE 
  do
    OPTS=($LINE)
    DATASET=${OPTS[0]}
    SAMPLE=${OPTS[1]}
    submitDataset
  done
}

for year in 2016postVFP 2016preVFP 2017 2018
do
  for type in data MC
  do
    YEAR=$year
    TYPE=$type
    CFG=/eos/user/e/etsai/workspace/TopLFV_ReNano_CMSSW_13_2_2/src/PhysicsTools/NanoAOD/test/topNano_v9_1_1_${YEAR}UL_${TYPE}_cfg.py
    submitBatch
  done
done
