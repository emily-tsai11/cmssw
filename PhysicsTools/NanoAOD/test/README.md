# Re-nano of Run 2 TOP nanoAOD for TOP LFV extension analysis

Based on central NanoAOD v9-1-1, CMSSW_10_6_27. However, ParticleNet is ported from CMSSW_13_2_2, so this repository requires `lxplus8`. TOP lepton MVA ID is also added.

Code is pulled from:
- https://github.com/cms-top/cmssw/tree/topNanoV9_from-CMSSW_10_6_27
- https://github.com/hqucms/cmssw/tree/dev/CMSSW_13_2_2/NanoAOD-puppiAK4
- Top Lepton MVA producer
  - https://github.com/GhentAnalysis/heavyNeutrino/blob/UL_master/multilep/interface/LeptonMvaHelper.h
  - https://github.com/GhentAnalysis/heavyNeutrino/blob/UL_master/multilep/src/LeptonMvaHelper.cc

## Setup
```
cmsrel CMSSW_13_2_2
cd CMSSW_13_2_2/src
cmsenv
git cms-init
git clone --recurse-submodules --remote-submodules git@github.com:emily-tsai11/cmssw.git -b 1322_TopLFVReNano
scram setup xgboost
export LD_PRELOAD=/cvmfs/cms.cern.ch/el8_amd64_gcc11/external/xgboost/1.7.5-341fbcc552d52a1d84e61813dc6267a2/lib64/libxgboost.so
scram b -j
```
NOTE: the `lib64` path for `libxgboost.so` was copied from `$CMSSW_BASE/config/toolbox/el8_amd64_gcc11/tools/selected/xgboost.xml`.

## To run locally
To run the default file, a working grid certificate is required.
```
cd PhysicsTools/NanoAOD/test
cmsRun modify_cfg.py
```

## To submit crab jobs
To submit a single dataset, use:
```
./submit_single.py
```
To submit all datasets, use:
```
./submit_batch.py
```
Datasets are listed in the `.list` files.

## To update submodules
```
cd CMSSW_13_2_2/src
git submodule update --remote
```

## XGBoost documentation
https://cms-ml.github.io/documentation/inference/xgboost.html#cc-usage-with-cmssw
