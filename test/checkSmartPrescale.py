import FWCore.ParameterSet.Config as cms

process = cms.Process( 'TEST' )

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool( True )
)

process.MessageLogger = cms.Service( "MessageLogger",
    destinations = cms.untracked.vstring( 'warnings',
      'errors',
      'infos',
      'debugs',
      'cout',
      'cerr',
      'log4cplus' ),
    categories = cms.untracked.vstring( 'FwkJob',
      'FwkReport',
      'FwkSummary',
      'Root_NoDictionary' ),
    statistics = cms.untracked.vstring( 'cerr' ),
    cerr = cms.untracked.PSet(
      INFO = cms.untracked.PSet(  limit = cms.untracked.int32( 0 ) ),
      noTimeStamps = cms.untracked.bool( False ),
      FwkReport = cms.untracked.PSet(
        reportEvery = cms.untracked.int32( 1 ),
        limit = cms.untracked.int32( 0 )
      ),
      default = cms.untracked.PSet(  limit = cms.untracked.int32( 10000000 ) ),
      Root_NoDictionary = cms.untracked.PSet(  limit = cms.untracked.int32( 0 ) ),
      FwkJob = cms.untracked.PSet(  limit = cms.untracked.int32( 0 ) ),
      FwkSummary = cms.untracked.PSet(
        reportEvery = cms.untracked.int32( 1 ),
        limit = cms.untracked.int32( 10000000 )
      ),
      threshold = cms.untracked.string( "INFO" )
    ),
    cout = cms.untracked.PSet(  threshold = cms.untracked.string( "ERROR" ) ),
    errors = cms.untracked.PSet(
      threshold = cms.untracked.string( "INFO" ),
      placeholder = cms.untracked.bool( True )
    ),
    warnings = cms.untracked.PSet(
      threshold = cms.untracked.string( "INFO" ),
      placeholder = cms.untracked.bool( True )
    ),
    infos = cms.untracked.PSet(
      threshold = cms.untracked.string( "INFO" ),
      Root_NoDictionary = cms.untracked.PSet(  limit = cms.untracked.int32( 0 ) ),
      placeholder = cms.untracked.bool( True )
    ),
    debugs = cms.untracked.PSet(
      threshold = cms.untracked.string( "INFO" ),
      placeholder = cms.untracked.bool( True )
    ),
    fwkJobReports = cms.untracked.vstring( 'FrameworkJobReport' ),
    FrameworkJobReport = cms.untracked.PSet(
      default = cms.untracked.PSet(  limit = cms.untracked.int32( 0 ) ),
      FwkJob = cms.untracked.PSet(  limit = cms.untracked.int32( 10000000 ) )
    ),
)

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32( 10000 )
)

process.source = cms.Source( 'PoolSource',
    fileNames = cms.untracked.vstring( 'file:/tmp/fwyzard/InputCollection.root' )
)

process.triggerResultsFilter = cms.EDFilter( 'TriggerResultsFilter',
    hltResults = cms.InputTag( 'TriggerResults', '', 'HLT' ),
    l1tResults = cms.InputTag( '' ),
    l1tIgnoreMask = cms.bool( False ),
    daqPartitions = cms.uint32( 0x01 ),
    throw = cms.bool( True ),
    triggerConditions = cms.vstring( 
      'HLT_TrackPointing / 4',
      'HLT_MET100',
      'HLT_L1MuOpen / 50',
      'HLT_L1Mu / 15',
      'HLT_ZeroBias / 2' )
)

process.hltHighLevelDev = cms.EDFilter( 'HLTHighLevelDev',
    TriggerResultsTag = cms.InputTag( 'TriggerResults','','HLT' ),
    andOr = cms.bool( True ),
    throw = cms.bool( True ),
    eventSetupPathsKey = cms.string( '' ),
    HLTOverallPrescale = cms.uint32( 1 ),
    HLTPaths = cms.vstring( 
      'HLT_TrackPointing',
      'HLT_MET100',
      'HLT_L1MuOpen',
      'HLT_L1Mu',
      'HLT_ZeroBias' ),
    HLTPathsPrescales = cms.vuint32( 4, 1, 50, 15, 2 )
)

process.OLD = cms.Path( process.hltHighLevelDev )
process.NEW = cms.Path( process.triggerResultsFilter )

process.NotNEW = cms.Path( process.hltHighLevelDev      + ~ process.triggerResultsFilter )
process.NotOLD = cms.Path( process.triggerResultsFilter + ~ process.hltHighLevelDev )
