TTnoPU_cfg.py generated with:
```
cmsDriver.py --python_filename TTnoPU_cfg.py --eventcontent MINIAODSIM --customise SLHCUpgradeSimulations/Configuration/aging.customise_aging_1000,Configuration/DataProcessing/Utils.addMonitoring --datatier GEN-SIM-DIGI-RAW-MINIAOD --inputCommands "keep *" --fileout file:TTnoPU.root --conditions 131X_mcRun4_realistic_v5 --customise_commands "process.MINIAODSIMoutput.outputCommands.append('keep *_*_*_HLT'); process.MINIAODSIMoutput.outputCommands.append('keep *_*_*_SIM');" --step RAW2DIGI,RECO,RECOSIM,PAT --geometry Extended2026D95 --nStreams 3 --filein file:/eos/cms/store/group/phys_btag/etsai/TTToHadronic_noPU_eba97a4e.root --era Phase2C17I13M9 --no_exec --mc -n -1 --dump_python || exit $? ;
```

TTPU200_cfg.py generated with:
```
cmsDriver.py --python_filename TTPU200_cfg.py --eventcontent MINIAODSIM --customise SLHCUpgradeSimulations/Configuration/aging.customise_aging_1000,Configuration/DataProcessing/Utils.addMonitoring --datatier GEN-SIM-DIGI-RAW-MINIAOD --inputCommands "keep *" --fileout file:TTPU200.root --conditions 131X_mcRun4_realistic_v9 --customise_commands "process.MINIAODSIMoutput.outputCommands.append('keep *_*_*_HLT'); process.MINIAODSIMoutput.outputCommands.append('keep *_*_*_SIM');" --step RAW2DIGI,RECO,RECOSIM,PAT --geometry Extended2026D95 --nStreams 3 --filein file:/eos/cms/store/group/phys_btag/etsai/TTToHadronic_PU200_a80e13072afd.root --era Phase2C17I13M9 --no_exec --mc -n -1 --dump_python || exit $? ;
```
