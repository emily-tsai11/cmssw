### cmsDriver.py command
```
cmsDriver.py --python_filename TTToHadronic_PU200_cfg.py --eventcontent MINIAODSIM --customise SLHCUpgradeSimulations/Configuration/aging.customise_aging_1000,Configuration/DataProcessing/Utils.addMonitoring --datatier GEN-SIM-DIGI-RAW-MINIAOD --inputCommands "keep *" --fileout file:TToHadronic_PU200.root --conditions 131X_mcRun4_realistic_v9 --customise_commands "process.MINIAODSIMoutput.outputCommands.append('keep *_*_*_HLT'); process.MINIAODSIMoutput.outputCommands.append('keep *_*_*_SIM');" --step RAW2DIGI,RECO,RECOSIM,PAT --geometry Extended2026D95 --nStreams 3 --filein file:TTToHadronic_PU200_originalDAS.root --era Phase2C17I13M9 --no_exec --mc -n -1 --processName=BTV --dump_python
```
