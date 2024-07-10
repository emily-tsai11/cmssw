from TTToHadronic_noPU_cfg import *


def addTrackMCMatching(process):
    process.trackMCMatch = process.prunedTrackMCMatch.clone(
        associator = cms.string("quickTrackAssociatorByHits"),
        trackingParticles = cms.InputTag("mix", "MergedTrackTruth")
    )
    process.trackMCMatchTask = cms.Task(
        process.tpClusterProducer,
        process.trackMCMatch,
        process.quickTrackAssociatorByHits
    )
    process.recosim = cms.Task(
        process.muonSimClassificationByHitsTask,
        process.trackPrunedMCMatchTask,
        process.trackMCMatchTask
    )
    return process


def addMTDTrackTimeExtrapolationToPV(process):
    process.trackExtenderWithMTDPV = process.trackExtenderWithMTD.clone(
        TrackTimeReference = cms.string("primaryVertex")
    )
    process.fastTimingGlobalRecoTask = cms.Task(
        process.mtdTrackQualityMVA,
        process.trackExtenderWithMTD,
        process.trackExtenderWithMTDPV
    )
    return process


def addMTDToIVF(process):
    # Cluster with all tracks that have a time extrapolated to the beam spot
    process.inclusiveVertexFinderMTDBS = process.inclusiveVertexFinder.clone(
        clusterizer = cms.PSet(
            useMTD = cms.bool(True)
        )
    )
    process.vertexMergerMTDBS = process.vertexMerger.clone(
        secondaryVertices = cms.InputTag("inclusiveVertexFinderMTDBS")
    )
    process.inclusiveVertexingMTDBSTask = cms.Task(
        process.inclusiveSecondaryVertices,
        process.inclusiveVertexFinderMTDBS,
        process.trackVertexArbitrator,
        process.vertexMergerMTDBS
    )

    # Cluster with all tracks that have a time extrapolated to the beam spot and time range cut
    process.inclusiveVertexFinderMTDBS4 = process.inclusiveVertexFinder.clone(
        clusterizer = cms.PSet(
            useMTD = cms.bool(True),
            cutTimeRange = cms.bool(True),
        )
    )
    process.vertexMergerMTDBS4 = process.vertexMerger.clone(
        secondaryVertices = cms.InputTag("inclusiveVertexFinderMTDBS4")
    )
    process.inclusiveVertexingMTDBS4Task = cms.Task(
        process.inclusiveSecondaryVertices,
        process.inclusiveVertexFinderMTDBS4,
        process.trackVertexArbitrator,
        process.vertexMergerMTDBS4
    )

    # Cluster with all tracks that have a time extrapolated to the primary vertex
    process.inclusiveVertexFinderMTDPV = process.inclusiveVertexFinder.clone(
        clusterizer = cms.PSet(
            useMTD = cms.bool(True)
        ),
        trackTimeReference = cms.string("primaryVertex")
    )
    process.vertexMergerMTDPV = process.vertexMerger.clone(
        secondaryVertices = cms.InputTag("inclusiveVertexFinderMTDPV")
    )
    process.inclusiveVertexingMTDPVTask = cms.Task(
        process.inclusiveSecondaryVertices,
        process.inclusiveVertexFinderMTDPV,
        process.trackVertexArbitrator,
        process.vertexMergerMTDPV
    )

    # Update vertex reco task
    process.vertexrecoTask = cms.Task(
        process.caloJetsForTrkTask,
        process.generalV0Candidates,
        process.inclusiveVertexingTask,
        process.inclusiveVertexingMTDBSTask,
        process.inclusiveVertexingMTDBS4Task,
        process.inclusiveVertexingMTDPVTask,
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
    return process


def dropKeepBranches(process):
    process.MINIAODSIMoutput.outputCommands.extend((
        "drop *",
        "keep SimTracks_g4SimHits__SIM",
        "keep SimVertexs_g4SimHits__SIM",
        "keep edmHepMCProduct_generatorSmeared__SIM",
        "keep float_genParticles_t0_HLT",
        "keep *_mix_MergedTrackTruth_HLT",
        "keep recoGenJets_ak4GenJets__HLT",
        "keep recoGenJets_ak4GenJetsNoNu__HLT",
        "keep recoGenParticles_genParticles__HLT",
        "keep double_fixedGridRhoFastjetAll__BTV",
        "keep *_packedPFCandidateToGenAssociation__BTV",
        "keep *_trackMCMatch_*_BTV",
        "keep *_pfDeepCSVJetTags_*_BTV",
        "keep *_slimmedJets_tagInfos_BTV",
        "keep *_trackExtenderWithMTD*_generalTrackt0_BTV",
        "keep *_trackExtenderWithMTD*_generalTracksigmat0_BTV",
        "keep *_mtdTrackQualityMVA_mtdQualMVA_BTV",
        "keep recoJetFlavourInfoMatchingCollection_slimmedGenJetsFlavourInfos__BTV",
        "keep PileupSummaryInfos_slimmedAddPileupInfo_*_BTV",
        "keep patElectrons_slimmedElectrons__BTV",
        "keep patJets_slimmedJets__BTV",
        "keep patJets_slimmedJetsPuppi__BTV",
        "keep patMuons_slimmedMuons__BTV",
        "keep patPackedCandidates_packedPFCandidates__BTV",
        "keep patPackedGenParticles_packedGenParticles__BTV",
        "keep recoGenJets_slimmedGenJets__BTV",
        "keep recoGenParticles_prunedGenParticles__BTV",
        "keep *_pfImpactParameterTagInfos__BTV",
        "keep *_pfInclusiveSecondaryVertexFinderTagInfos__BTV",
        "keep recoTracks_generalTracks__BTV",
        "keep *_inclusiveVertexFinder_*_BTV",
        "keep *_inclusiveVertexFinderMTDBS_*_BTV",
        "keep *_inclusiveVertexFinderMTDBS4_*_BTV",
        "keep *_inclusiveVertexFinderMTDPV_*_BTV",
        "keep recoVertexs_offlinePrimaryVertices__BTV",
        "keep recoVertexs_offlineSlimmedPrimaryVertices__BTV",
        "keep recoVertexs_offlineSlimmedPrimaryVerticesWithBS__BTV",
        "keep recoVertexCompositePtrCandidates_slimmedSecondaryVertices__BTV",
    ))
    return process


def setMaxEvents(process, maxEvents):
    process.maxEvents.input = maxEvents
    return process


def setOutputFileName(process, fileName):
    process.MINIAODSIMoutput.fileName = fileName
    return process


def dumpDebug(process, debug):
    if debug: open("dumpDebug.py", "w").write(process.dumpPython())


setMaxEvents(process, 10)
setOutputFileName(process, "TTToHadronic_noPU_slimmed.root")
addTrackMCMatching(process)
addMTDTrackTimeExtrapolationToPV(process)
addMTDToIVF(process)
dropKeepBranches(process)
dumpDebug(process, False)
