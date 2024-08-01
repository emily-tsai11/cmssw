# Backport PNet to Run 2

## Setup
```
cmsrel CMSSW_13_2_2
cd CMSSW_13_2_2/src
cmsenv
git cms-init
git cms-checkout-topic hqucms:dev/CMSSW_13_2_2/NanoAOD-puppiAK4
```

## Copy over relevant files
```
cd PhysicsTools/NanoAOD/python
```
<!-- git checkout remotes/my-cmssw/topNanoV9-1-1_10_6_27-toplfv -- genWeightsTable_cfi.py -->
<!-- git checkout remotes/my-cmssw/topNanoV9-1-1_10_6_27-toplfv -- ttbarCategorization_cff.py -->

```
cd PhysicsTools/NanoAOD/test
git checkout remotes/my-cmssw/topNanoV9-1-1_10_6_27-toplfv -- topNano_v9-1-1_2016ULpostVFP_MC_cfg.py
```
