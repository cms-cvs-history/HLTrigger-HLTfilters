#ifndef HLTfilters_HLTLevel1GTSeedCache_h
#define HLTfilters_HLTLevel1GTSeedCache_h

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
 * $Date: 2009/03/16 09:45:51 $
 * $Revision: 1.1.2.1 $
 *
 */

// system include files
#include <vector>

// user include files
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ParameterSet/interface/InputTag.h"
#include "FWCore/ServiceRegistry/interface/ActivityRegistry.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Provenance/interface/ParameterSetID.h"
#include "DataFormats/Provenance/interface/EventID.h"
#include "DataFormats/Provenance/interface/Timestamp.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticle.h"
#include "DataFormats/L1Trigger/interface/L1MuonParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1EmParticle.h"
#include "DataFormats/L1Trigger/interface/L1EmParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1JetParticle.h"
#include "DataFormats/L1Trigger/interface/L1JetParticleFwd.h"
#include "DataFormats/L1Trigger/interface/L1EtMissParticle.h"
#include "DataFormats/L1Trigger/interface/L1EtMissParticleFwd.h"

// forward declarations
class L1GlobalTriggerReadoutRecord;
class L1GlobalTriggerObjectMapRecord;

// class declaration
class HLTLevel1GTSeedCache {
public:

    /// constructor
    explicit HLTLevel1GTSeedCache(const edm::ParameterSet & config, edm::ActivityRegistry & registry);

    /// destructor
    virtual ~HLTLevel1GTSeedCache();

    /// clear the cache as soon as an event is arriving
    void preProcessEvent(const edm::EventID & eventId, const edm::Timestamp & timestamp);

    /// register a filter with the cache service
    unsigned int registerConfiguration(const edm::ParameterSet & config);
    
    /// set the current Event
    bool loadEvent(const edm::Event & event);

    /// access the content of the cache (pass cahceId to identify the configuration)
    const edm::Handle<L1GlobalTriggerReadoutRecord>        & getL1GtReadoutRecord(  unsigned int cacheId) const;
    const edm::Handle<L1GlobalTriggerObjectMapRecord>      & getL1GtObjectMapRecord(unsigned int cacheId) const;
    const edm::Handle<l1extra::L1MuonParticleCollection>   & getL1Muon(             unsigned int cacheId) const;
    const edm::Handle<l1extra::L1EmParticleCollection>     & getL1IsoEG(            unsigned int cacheId) const;
    const edm::Handle<l1extra::L1EmParticleCollection>     & getL1NoIsoEG(          unsigned int cacheId) const;
    const edm::Handle<l1extra::L1JetParticleCollection>    & getL1CenJet(           unsigned int cacheId) const;
    const edm::Handle<l1extra::L1JetParticleCollection>    & getL1ForJet(           unsigned int cacheId) const;
    const edm::Handle<l1extra::L1JetParticleCollection>    & getL1TauJet(           unsigned int cacheId) const;
    const edm::Handle<l1extra::L1EtMissParticleCollection> & getL1EnergySums(       unsigned int cacheId) const;

private:

    struct Cache {
      edm::Handle<L1GlobalTriggerReadoutRecord>        l1GtReadoutRecord;
      edm::Handle<L1GlobalTriggerObjectMapRecord>      l1GtObjectMapRecord;
      edm::Handle<l1extra::L1MuonParticleCollection>   l1Muon;
      edm::Handle<l1extra::L1EmParticleCollection>     l1IsoEG;
      edm::Handle<l1extra::L1EmParticleCollection>     l1NoIsoEG;
      edm::Handle<l1extra::L1JetParticleCollection>    l1CenJet;
      edm::Handle<l1extra::L1JetParticleCollection>    l1ForJet;
      edm::Handle<l1extra::L1JetParticleCollection>    l1TauJet;
      edm::Handle<l1extra::L1EtMissParticleCollection> l1EnergySums;
    };

    struct CacheConfiguration {
      edm::ParameterSetID id;
      edm::InputTag l1GtReadoutRecordTag;
      edm::InputTag l1GtObjectMapRecordTag;
      edm::InputTag l1MuonTag;
      edm::InputTag l1IsoEGTag;
      edm::InputTag l1NoIsoEGTag;
      edm::InputTag l1CenJetTag;
      edm::InputTag l1ForJetTag;
      edm::InputTag l1TauJetTag;
      edm::InputTag l1ExtraTag;
    };
    
    // cached stuff
    const edm::Event *              m_event;
    edm::EventID                    m_eventId;
    std::vector<CacheConfiguration> m_configuration;
    std::vector<Cache>              m_cache;

    /// access the content of the cache (pass cahceId to identify the configuration)
    template <typename T>
    const edm::Handle<T> & get(unsigned int cacheId, edm::Handle<T> (Cache::*entry) ) const {
      const static edm::Handle<T> invalidHandle;
      if (cacheId < m_cache.size()) {
        // retrieve the Handle from the cache
        const edm::Handle<T> & handle = m_cache[cacheId].*entry;
        if (handle.isValid())
          // track the provenance of this collection
          m_event->addToGotBranchIDs( * handle.provenance() );
        return handle;
      } else {
        return invalidHandle;
      }
    }

};

#endif // HLTfilters_HLTLevel1GTSeedCache_h
