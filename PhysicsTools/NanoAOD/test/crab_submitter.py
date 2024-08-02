from CRABClient.UserUtilities import config
config = config()

config.General.workArea          = "/eos/user/e/etsai/workspace/TopLFV_ReNano_CMSSW_13_2_2/src/PhysicsTools/NanoAOD/test"
config.General.transferOutputs   = True
config.General.instance          = "prod"

config.JobType.pluginName        = "Analysis"
config.JobType.psetName          = "modify_cfg.py"
config.JobType.maxMemoryMB       = 4000
config.JobType.numCores          = 2

config.Data.inputDBS             = "global"
config.Data.splitting            = "FileBased"
config.Data.unitsPerJob          = 1
config.Data.outLFNDirBase        = "/store/user/etsai/LFV_ReNanoAOD"
config.Data.publication          = False

config.Site.storageSite          = "T2_CH_CERN"
