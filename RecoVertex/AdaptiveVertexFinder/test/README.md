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
9. Set up and compiile the correct CMSSW version
10. Do cmsRun on the generated file to re-run the MiniAOD

### Currently used cmsDriver.py commands
`TTPU200_cfg.py` generated in `CMSSW_13_1_3` with:
```
cmsDriver.py --python_filename TTPU200_cfg.py --eventcontent MINIAODSIM --customise SLHCUpgradeSimulations/Configuration/aging.customise_aging_1000,Configuration/DataProcessing/Utils.addMonitoring --datatier GEN-SIM-DIGI-RAW-MINIAOD --inputCommands "keep *" --fileout file:TTPU200.root --conditions 131X_mcRun4_realistic_v9 --customise_commands "process.MINIAODSIMoutput.outputCommands.append('keep *_*_*_HLT'); process.MINIAODSIMoutput.outputCommands.append('keep *_*_*_SIM');" --step RAW2DIGI,RECO,RECOSIM,PAT --geometry Extended2026D95 --nStreams 3 --filein file:/eos/cms/store/group/phys_btag/etsai/TTToHadronic_PU200_a80e13072afd.root --era Phase2C17I13M9 --no_exec --mc -n -1 --processName=BTV --dump_python || exit $? ;
```
