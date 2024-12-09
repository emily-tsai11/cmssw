from CRABClient.UserUtilities import config
config = config()

config.General.requestName       = "TTToHadronic_PU200"
config.General.workArea          = "/eos/user/e/etsai/workspace/TTTPU200_CMSSW_14_0_9/src/RecoVertex/AdaptiveVertexFinder/test"
config.General.transferOutputs   = True
config.General.instance          = "prod"

config.JobType.pluginName        = "Analysis"
config.JobType.psetName          = "modify_cfg.py"
config.JobType.inputFiles        = ["/eos/user/e/etsai/workspace/TTTPU200_CMSSW_14_0_9/src/RecoVertex/AdaptiveVertexFinder/test/TTToHadronic_PU200_cfg.py"]
config.JobType.maxMemoryMB       = 4000

config.Data.inputDataset         = "/TT_TuneCP5_14TeV-powheg-pythia8/Phase2Spring24DIGIRECOMiniAOD-PU200_Trk1GeV_140X_mcRun4_realistic_v4-v2/GEN-SIM-DIGI-RAW-MINIAOD"
config.Data.inputDBS             = "global"
config.Data.splitting            = "FileBased"
config.Data.unitsPerJob          = 1
config.Data.outLFNDirBase        = "/store/group/phys_btag/etsai"
config.Data.publication          = False
config.Data.outputDatasetTag     = "TTToHadronic_PU200"

config.Site.storageSite          = "T2_CH_CERN"
