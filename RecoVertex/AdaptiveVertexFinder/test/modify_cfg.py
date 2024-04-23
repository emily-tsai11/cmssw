from TTPU200_cfg import *

def addMTDTiming(process):
    process.inclusiveCandidateVertexFinder.producer = cms.string("b-tagging")
    process.inclusiveCandidateVertexFinderCvsL.producer = cms.string("c-tagging")

    process.inclusiveVertexFinder.producer = cms.string("default")
    process.inclusiveVertexFinder.useMTDTiming = cms.bool(False)
    process.inclusiveVertexFinder.timeValueMap = cms.InputTag("tofPID:t0")
    process.inclusiveVertexFinder.timeErrorMap = cms.InputTag("tofPID:sigmat0")
    process.inclusiveVertexFinder.timeQualityMap = cms.InputTag("mtdTrackQualityMVA:mtdQualMVA")
    process.inclusiveVertexFinder.timeQualityThreshold = cms.double(0.5)

    process.inclusiveVertexFinderMTDTiming = process.inclusiveVertexFinder.clone(
        producer = cms.string("timing"),
        useMTDTiming = cms.bool(True)
    )

    process.vertexMergerMTDTiming = process.vertexMerger.clone(
        secondaryVertices = cms.InputTag("inclusiveVertexFinderMTDTiming")
    )

    process.inclusiveVertexingMTDTimingTask = cms.Task(
        process.inclusiveSecondaryVertices,
        process.inclusiveVertexFinderMTDTiming,
        process.trackVertexArbitrator,
        process.vertexMergerMTDTiming
    )

    process.vertexrecoTask = cms.Task(
        process.caloJetsForTrkTask,
        process.generalV0Candidates,
        process.inclusiveVertexingTask,
        process.inclusiveVertexingMTDTimingTask,
        process.offlinePrimaryVertices,
        process.offlinePrimaryVertices4D,
        process.offlinePrimaryVertices4DWithBS,
        process.offlinePrimaryVertices4DnoPID,
        process.offlinePrimaryVertices4DnoPIDWithBS,
        process.offlinePrimaryVerticesWithBS,
        process.quickTrackAssociatorByHits,
        process.tofPID,
        process.tofPID4DnoPID,
        process.tpClusterProducer,
        process.trackRefsForJetsBeforeSorting,
        process.trackRefsForJetsBeforeSorting4D,
        process.trackRefsForJetsBeforeSorting4DnoPID,
        process.trackTimeValueMapProducer,
        process.trackWithVertexRefSelectorBeforeSorting,
        process.trackWithVertexRefSelectorBeforeSorting4D,
        process.trackWithVertexRefSelectorBeforeSorting4DnoPID,
        process.unsortedOfflinePrimaryVertices,
        process.unsortedOfflinePrimaryVertices4D,
        process.unsortedOfflinePrimaryVertices4DnoPID
    )

    process.MINIAODSIMoutput.outputCommands.extend((
        'keep *_tofPID_*_*',
        'keep *_mtdTrackQualityMVA_*_*',
        'keep *_offlinePrimaryVertices_*_*',
        'keep *_inclusiveVertexFinder_*_*',
        'keep *_inclusiveVertexFinderMTDTiming_*_*'
    ))

    return process

addMTDTiming(process)
# process.maxEvents.input = 30
# open("debug_dump.py", "w").write(process.dumpPython())
