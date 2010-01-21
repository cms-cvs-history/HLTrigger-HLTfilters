#ifndef HLTrigger_HLTfilters_TriggerExpressionData_h
#define HLTrigger_HLTfilters_TriggerExpressionData_h

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Provenance/interface/ParameterSetID.h"
#include "DataFormats/Provenance/interface/EventID.h"

namespace edm {
  class Event;
  class TriggerResults;
  class TriggerNames;
}

namespace triggerExpression {

class Data {
public:
  explicit Data(const edm::ParameterSet & config) :
    // configuration
    m_triggerResultsTag(config.getParameter<edm::InputTag> ("triggerResults")),
    m_throw(config.getParameter<bool> ("throw")),
    // values
    m_triggerResults(0),
    m_triggerNames(0),
    m_parameterSetID(),
    m_updated(false),
    m_eventNumber()
  { }

  bool setEvent(const edm::Event & event);

  const edm::TriggerResults & triggerResults() const {
    return * m_triggerResults;
  }

  const edm::TriggerNames & triggerNames() const {
    return * m_triggerNames;
  }

  bool configurationUpdated() const {
    return m_updated;
  }

  edm::EventNumber_t eventNumber() const {
    return m_eventNumber;
  }

private:
  // configuration
  edm::InputTag m_triggerResultsTag;
  bool m_throw;

  // values
  const edm::TriggerResults * m_triggerResults;
  const edm::TriggerNames   * m_triggerNames;

  edm::ParameterSetID         m_parameterSetID;
  bool                        m_updated;

  edm::EventNumber_t          m_eventNumber;
};

} // namespace triggerExpression

#endif // HLTrigger_HLTfilters_TriggerExpressionData_h