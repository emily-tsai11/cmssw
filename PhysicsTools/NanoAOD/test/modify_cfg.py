# Parse job options
import FWCore.ParameterSet.VarParsing as VarParsing
options = VarParsing.VarParsing()
options.register("inputFiles",
  "root://cms-xrd-global.cern.ch///store/mc/RunIISummer20UL16MiniAODv2/TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/MINIAODSIM/106X_mcRun2_asymptotic_v17-v1/120000/01572CA8-C7AE-0346-B660-8AB4F7C2AE36.root",
  VarParsing.VarParsing.multiplicity.list, VarParsing.VarParsing.varType.string, "Input file(s)")
options.register("year",        "2016postVFP", VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, "Dataset year")
options.register("type",        "mc",          VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.string, "Data or mc")
options.register("maxEvents",   100,           VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int,    "Maximum number of events")
options.register("reportEvery", 10,            VarParsing.VarParsing.multiplicity.singleton, VarParsing.VarParsing.varType.int,    "Report every N events")
options.parseArguments()

# Import configuration file
print("Era is %s and dataset type is %s" % (options.year, options.type))
if options.year=="2016postVFP":
    if options.type=="data": from topNano_v9_1_1_2016postVFPUL_data_cfg import *
    elif options.type=="mc": from topNano_v9_1_1_2016postVFPUL_MC_cfg import *
    else: print("Incorrect type specified.")
elif options.year=="2016preVFP":
    if options.type=="data": from topNano_v9_1_1_2016preVFPUL_data_cfg import *
    elif options.type=="mc": from topNano_v9_1_1_2016preVFPUL_MC_cfg import *
    else: print("Incorrect type specified.")
elif options.year=="2017":
    if options.type=="data": from topNano_v9_1_1_2017UL_data_cfg import *
    elif options.type=="mc": from topNano_v9_1_1_2017UL_MC_cfg import *
    else: print("Incorrect type specified.")
elif options.year=="2018":
    if options.type=="data": from topNano_v9_1_1_2018UL_data_cfg import *
    elif options.type=="mc": from topNano_v9_1_1_2018UL_MC_cfg import *
    else: print("Incorrect type specified.")
else: print("Incorrect year specified.")

# Set input file
process.source.fileNames = options.inputFiles

# Set max events and reporting threshold
process.maxEvents.input = options.maxEvents
process.MessageLogger.cerr.FwkReport.reportEvery = options.reportEvery

# Write debug dump file
open("dumpDebug.py", "w").write(process.dumpPython())
