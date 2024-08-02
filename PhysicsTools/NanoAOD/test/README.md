# Re-nano of Run 2 TOP nanoAOD for TOP LFV extension analysis

Based on central NanoAOD v9, CMSSW_10_6_27. However, ParticleNet is ported from CMSSW_13_2_2, so this repository requires `lxplus8`. TOP lepton MVA ID is also added.

Code is pulled from:
- https://github.com/cms-top/cmssw/tree/topNanoV9_from-CMSSW_10_6_27
- https://github.com/hqucms/cmssw/tree/dev/CMSSW_13_2_2/NanoAOD-puppiAK4
- https://github.com/HephyAnalysisSW/Analysis/blob/UL/Tools/python/mvaTOPreader.py

## Setup
```
cmsrel CMSSW_13_2_2
cd CMSSW_13_2_2/src
cmsenv
git cms-init
git clone git@github.com:emily-tsai11/cmssw.git -b puppiAK4Nano_13_2_2-toplfv
scram b -j
```

## To run
To run the current default file(s), a working grid certificate is required.
```
cd PhysicsTools/NanoAOD/test
cmsRun topNano-v9-1-1_2016ULpostVFP_MC_cfg.py
```
