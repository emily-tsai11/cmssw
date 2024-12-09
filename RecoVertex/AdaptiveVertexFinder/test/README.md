### To get cmsDriver.py commands
1. Find the dataset on DAS and copy the dataset name from DAS
2. Go to MCM: https://cms-pdmv-prod.web.cern.ch/mcm/
3. Put the dataset name from DAS in "by dataset name” (expect this step to fail)
4. Click on "Output Dataset”
5. Again paste the dataset name from DAS into “dataset name as shown in DAS” and click search
6. Click on "get setup command”
7. Take note of the CMSSW version
8. Copy the second cmsDriver.py command and make sure to:
   1. Change input and output file names (input should be the MiniAOD file you're re-running)
   2. Correct the number of events
   3. Add `--dump_python` to the end
   4. Change the process name (`--processName=NAME`)
9. Set up and compile the correct CMSSW version
10. Do cmsRun on the generated file to re-run the MiniAOD

### Currently used sample

The currently used sample is [here](https://cmsweb.cern.ch/das/request?input=dataset%3D%2FTT_TuneCP5_14TeV-powheg-pythia8%2FPhase2Spring24DIGIRECOMiniAOD-PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2%2FGEN-SIM-DIGI-RAW-MINIAOD&instance=prod/global) on DAS.

`TTToHadronic_PU200_cfg.py` generated in `CMSSW_14_0_9` with:
```
cmsDriver.py  --python_filename TTToHadronic_PU200_cfg.py --eventcontent MINIAODSIM --customise SLHCUpgradeSimulations/Configuration/aging.customise_aging_1000,Configuration/DataProcessing/Utils.addMonitoring --datatier GEN-SIM-DIGI-RAW-MINIAOD --inputCommands "keep *" --fileout file:TToHadronic_PU200.root --conditions 140X_mcRun4_realistic_v4 --customise_commands "process.MINIAODSIMoutput.outputCommands.append('keep *_*_*_HLT'); process.MINIAODSIMoutput.outputCommands.append('keep *_*_*_SIM');" --step RAW2DIGI,RECO,RECOSIM,PAT --geometry Extended2026D110 --nStreams 2 --filein file:/eos/cms/store/group/phys_btag/etsai/Phase2Spring24_TTPU200_Trk1GeV.root --era Phase2C17I13M9 --no_exec --mc -n -1 --processName=BTV --dump_python || exit $? ;
```
