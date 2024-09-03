from TTToHadronic_PU200_cfg import *


def addMergedGenParticles(process):
    process.mergedGenParticles = cms.EDProducer("MergedGenParticleProducer",
        inputPruned = cms.InputTag("prunedGenParticles"),
        inputPacked = cms.InputTag("packedGenParticles"),
    )
    process.genParticlesTask = cms.Task(
        process.packedGenParticles,
        process.prunedGenParticles,
        process.mergedGenParticles,
        process.prunedGenParticlesWithStatusOne
    )
    return process


def addMTDTrackTimingToSVReco(process):
    # Add timing to clustering
    process.inclusiveCandidateVertexFinderMTDPV = process.inclusiveCandidateVertexFinder.clone(
        useMTDTrackTime = cms.bool(True)
    )
    process.candidateVertexMergerMTDPV = process.candidateVertexMerger.clone(
        secondaryVertices = cms.InputTag("inclusiveCandidateVertexFinderMTDPV")
    )
    process.candidateVertexArbitratorMTDPV = process.candidateVertexArbitrator.clone(
        secondaryVertices = cms.InputTag("candidateVertexMergerMTDPV")
    )
    process.inclusiveCandidateSecondaryVerticesMTDPV = process.inclusiveCandidateSecondaryVertices.clone(
        secondaryVertices = cms.InputTag("candidateVertexArbitratorMTDPV")
    )
    process.inclusiveCandidateVertexingTask.add(
        process.candidateVertexArbitratorMTDPV,
        process.candidateVertexMergerMTDPV,
        process.inclusiveCandidateSecondaryVerticesMTDPV,
        process.inclusiveCandidateVertexFinderMTDPV
    )
    # Add slimmed vertices with timing
    process.slimmedSecondaryVerticesMTDPV = process.slimmedSecondaryVertices.clone(
        src = cms.InputTag("inclusiveCandidateSecondaryVerticesMTDPV")
    )
    process.patTask.add(process.slimmedSecondaryVerticesMTDPV)
    # Return process
    return process


def dropKeepBranches(process):
    process.MINIAODSIMoutput.outputCommands.extend((
        "drop *",
        "keep SimTracks_g4SimHits__SIM", # for further "reconstructable" vertex definition
        "keep recoGenParticles_mergedGenParticles__BTV", # for "reconstructable" GenVertex definition
        "keep recoPFCandidates_particleFlow__BTV", # input to IVF
        "keep recoVertexs_offlineSlimmedPrimaryVertices__BTV", # for calculating impact parameters
        "keep *_inclusiveCandidateVertexFinder*_*_BTV", # before merging to get nClusters
        "keep *_inclusiveCandidateVertexFinderCvsL_*_BTV", # before merging to get nClusters
        "keep recoVertexCompositePtrCandidates_inclusiveCandidateSecondaryVertices*__BTV", # input to slimming
        "keep recoVertexCompositePtrCandidates_inclusiveCandidateSecondaryVerticesCvsL__BTV", # NOT input to slimming
        "keep recoVertexCompositePtrCandidates_slimmedSecondaryVertices*__BTV",
        "keep recoGenJets_slimmedGenJets__BTV",
        "keep patJets_slimmedJets__BTV",
        "keep recoJetFlavourInfoMatchingCollection_slimmedGenJetsFlavourInfos__BTV",
        # For track time validation
        "keep recoTracks_generalTracks__BTV",
        "keep *_trackExtenderWithMTD_generalTrackt0_BTV",
        "keep *_trackExtenderWithMTD_generalTracksigmat0_BTV",
        "keep *_trackExtenderFromPointWithMTD_generalTrackt0_BTV",
        "keep *_trackExtenderFromPointWithMTD_generalTracksigmat0_BTV",
        # "keep *_mtdTrackQualityMVA_mtdQualMVA_BTV", # Not well-defined
    ))
    return process


def setMaxEvents(process, maxEvents):
    process.maxEvents.input = maxEvents
    return process


def setInputFileName(process, fileName):
    process.source.fileNames = cms.untracked.vstring(fileName)
    return process


def setOutputFileName(process, fileName):
    process.MINIAODSIMoutput.fileName = fileName
    return process


def dumpDebug(process, debug):
    if debug: open("dumpDebug.py", "w").write(process.dumpPython())
    return process


setMaxEvents(process, -1)
# setInputFileName(process, "file:/eos/user/e/etsai/workspace/TTToHadronicPU200_ReReco_CMSSW_13_1_3/src/RecoVertex/AdaptiveVertexFinder/test/TTToHadronic_PU200_originalDAS.root")
setOutputFileName(process, "TTToHadronic_PU200_slimmed.root")
addMergedGenParticles(process)
addMTDTrackTimingToSVReco(process)
dropKeepBranches(process)
dumpDebug(process, False)
