/**
 * \class HLTLevel1GTSeedCache
 *
 *
 * Description: cache the Event content accessed by many HLTLevel1GTSeed filters
 *
 * Implementation:
 *    In the current implementation (CMSSW 2.2.X and 3.0.x) of HLTLevel1GTSeed,
 *    33% of the CPU time is spent doing Event::getByLabel().
 *    As the InputTags for all HLTLevel1GTSeed are likely to be the same, this can be
 *    optimized moving the getByLabel to an external class which caches the requests
 *    event by event.
 *    This is that class.
 *
 * \author: Andrea Bocci - Università di Pisa, INFN Sez. di Pisa, and CERN
 *
 * $Date$
 * $Revision$
 *
 */

#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerReadoutRecord.h"
#include "DataFormats/L1GlobalTrigger/interface/L1GlobalTriggerObjectMapRecord.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticle.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1EmParticle.h"
#include "DataFormats/L1Trigger/interface/L1EmParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1EtMissParticle.h"
#include "DataFormats/L1Trigger/interface/L1EtMissParticleFwd.h"

#include "HLTrigger/HLTfilters/interface/HLTLevel1GTSeedCache.h"

/// constructor
HLTLevel1GTSeedCache::HLTLevel1GTSeedCache(const edm::ParameterSet & config, edm::ActivityRegistry & registry) {
  registry.watchPreProcessEvent(this, &HLTLevel1GTSeedCache::preProcessEvent);
}

/// destructor
HLTLevel1GTSeedCache::~HLTLevel1GTSeedCache() {
}

/// new Event detected
void HLTLevel1GTSeedCache::preProcessEvent(const edm::EventID & eventId, const edm::Timestamp & timestamp) {
  m_eventId = eventId;
  m_event   = 0;
  for (unsigned int i = 0; i < m_cache.size(); ++i) {
    m_cache[i].l1GtReadoutRecord.clear();
    m_cache[i].l1GtObjectMapRecord.clear();
    m_cache[i].l1Muon.clear();
    m_cache[i].l1IsoEG.clear();
    m_cache[i].l1NoIsoEG.clear();
    m_cache[i].l1CenJet.clear();
    m_cache[i].l1ForJet.clear();
    m_cache[i].l1TauJet.clear();
    m_cache[i].l1EnergySums.clear();
  }
}

/// register a filter with the cache service
unsigned int HLTLevel1GTSeedCache::registerConfiguration(const edm::ParameterSet & config) {
  for (unsigned int i = 0; i < m_configuration.size(); ++i)
    if (m_configuration[i].id == config.id())
      // this configuration is already being monitored, reuse it
      return i;
  
  // this is a new configuration, add it
  CacheConfiguration c;
  c.id = config.id();
  c.l1GtReadoutRecordTag   = config.getParameter<edm::InputTag>("l1GtReadoutRecordTag");
  c.l1GtObjectMapRecordTag = config.getParameter<edm::InputTag>("l1GtObjectMapTag");
  c.l1MuonTag              = config.getParameter<edm::InputTag>("l1MuonTag");
  c.l1IsoEGTag             = config.getParameter<edm::InputTag>("l1IsoEGTag");
  c.l1NoIsoEGTag           = config.getParameter<edm::InputTag>("l1NoIsoEGTag");
  c.l1CenJetTag            = config.getParameter<edm::InputTag>("l1CenJetTag");
  c.l1ForJetTag            = config.getParameter<edm::InputTag>("l1ForJetTag");
  c.l1TauJetTag            = config.getParameter<edm::InputTag>("l1TauJetTag");
  c.l1ExtraTag             = config.getParameter<edm::InputTag>("l1ExtraTag");
  m_configuration.push_back(c);
  // keep the cache big enough for all the configurations
  m_cache.resize( m_configuration.size() );
  return (m_configuration.size() - 1);
}

/// set the current Event
bool HLTLevel1GTSeedCache::loadEvent(const edm::Event & event) {
  if (event.id() != m_eventId) {
    // trying to load the wrong Event
    // raise an exception ?
    return false;
  }
  if (m_event) {
    // event already set, nothing to do
    return true;  
  }
  // this is the first time we are called for this event, fill the cache
  m_event = & event;
  for (unsigned int i = 0; i < m_cache.size(); ++i) {
    event.getByLabel(m_configuration[i].l1GtReadoutRecordTag,   m_cache[i].l1GtReadoutRecord);
    event.getByLabel(m_configuration[i].l1GtObjectMapRecordTag, m_cache[i].l1GtObjectMapRecord);
    event.getByLabel(m_configuration[i].l1MuonTag,              m_cache[i].l1Muon);
    event.getByLabel(m_configuration[i].l1IsoEGTag,             m_cache[i].l1IsoEG);
    event.getByLabel(m_configuration[i].l1NoIsoEGTag,           m_cache[i].l1NoIsoEG);
    event.getByLabel(m_configuration[i].l1CenJetTag,            m_cache[i].l1CenJet);
    event.getByLabel(m_configuration[i].l1ForJetTag,            m_cache[i].l1ForJet);
    event.getByLabel(m_configuration[i].l1TauJetTag,            m_cache[i].l1TauJet);
    event.getByLabel(m_configuration[i].l1ExtraTag,             m_cache[i].l1EnergySums);
  }
  return true;
}

/// access the content of the cache (pass cahceId to identify the configuration)
const edm::Handle<L1GlobalTriggerReadoutRecord> & HLTLevel1GTSeedCache::getL1GtReadoutRecord(unsigned int cacheId) const {
  static edm::Handle<L1GlobalTriggerReadoutRecord> invalidHandle;
  return (cacheId < m_cache.size()) ? m_cache[cacheId].l1GtReadoutRecord : invalidHandle;
}

const edm::Handle<L1GlobalTriggerObjectMapRecord> & HLTLevel1GTSeedCache::getL1GtObjectMapRecord(unsigned int cacheId) const {
  static edm::Handle<L1GlobalTriggerObjectMapRecord> invalidHandle;
  return (cacheId < m_cache.size()) ? m_cache[cacheId].l1GtObjectMapRecord : invalidHandle;
}

const edm::Handle<l1extra::L1MuonParticleCollection> & HLTLevel1GTSeedCache::getL1Muon(unsigned int cacheId) const {
  static edm::Handle<l1extra::L1MuonParticleCollection> invalidHandle;
  return (cacheId < m_cache.size()) ? m_cache[cacheId].l1Muon : invalidHandle;
}

const edm::Handle<l1extra::L1EmParticleCollection> & HLTLevel1GTSeedCache::getL1IsoEG(unsigned int cacheId) const {
  static edm::Handle<l1extra::L1EmParticleCollection> invalidHandle;
  return (cacheId < m_cache.size()) ? m_cache[cacheId].l1IsoEG : invalidHandle;
}

const edm::Handle<l1extra::L1EmParticleCollection> & HLTLevel1GTSeedCache::getL1NoIsoEG(unsigned int cacheId) const {
  static edm::Handle<l1extra::L1EmParticleCollection> invalidHandle;
  return (cacheId < m_cache.size()) ? m_cache[cacheId].l1NoIsoEG : invalidHandle;
}

const edm::Handle<l1extra::L1JetParticleCollection> & HLTLevel1GTSeedCache::getL1CenJet(unsigned int cacheId) const {
  static edm::Handle<l1extra::L1JetParticleCollection> invalidHandle;
  return (cacheId < m_cache.size()) ? m_cache[cacheId].l1CenJet : invalidHandle;
}

const edm::Handle<l1extra::L1JetParticleCollection> & HLTLevel1GTSeedCache::getL1ForJet(unsigned int cacheId) const {
  static edm::Handle<l1extra::L1JetParticleCollection> invalidHandle;
  return (cacheId < m_cache.size()) ? m_cache[cacheId].l1ForJet : invalidHandle;
}

const edm::Handle<l1extra::L1JetParticleCollection> & HLTLevel1GTSeedCache::getL1TauJet(unsigned int cacheId) const {
  static edm::Handle<l1extra::L1JetParticleCollection> invalidHandle;
  return (cacheId < m_cache.size()) ? m_cache[cacheId].l1TauJet : invalidHandle;
}

const edm::Handle<l1extra::L1EtMissParticleCollection> & HLTLevel1GTSeedCache::getL1EnergySums(unsigned int cacheId) const {
  static edm::Handle<l1extra::L1EtMissParticleCollection> invalidHandle;
  return (cacheId < m_cache.size()) ? m_cache[cacheId].l1EnergySums : invalidHandle;
}

// declare this as a simple EDM Service
#include "FWCore/ServiceRegistry/interface/ServiceMaker.h"
DEFINE_FWK_SERVICE(HLTLevel1GTSeedCache);
