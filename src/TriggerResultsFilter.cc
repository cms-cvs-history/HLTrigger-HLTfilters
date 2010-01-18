/** \class TriggerResultsFilter
 *
 * See header file for documentation
 *
 *  $Date: 2010/01/17 15:29:46 $
 *  $Revision: 1.1 $
 *
 *  Authors: Martin Grunewald, Andrea Bocci
 *
 */

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <boost/foreach.hpp>

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "HLTrigger/HLTfilters/interface/TriggerResultsFilter.h"
#include "HLTrigger/HLTfilters/interface/TriggerExpressionEvaluator.h"
#include "HLTrigger/HLTfilters/interface/TriggerExpressionParser.h"

//
// constructors and destructor
//
TriggerResultsFilter::TriggerResultsFilter(const edm::ParameterSet & config) :
  m_triggerResults( config.getParameter<edm::InputTag> ("triggerResults")),
  m_throw         ( config.getParameter<bool> ("throw")),
  m_expressions(),
  m_eventCache(config)
{
  // parse the logical expressions into functionals
  const std::vector<std::string> & expressions = config.getParameter<std::vector<std::string> >("triggerConditions");
  unsigned int size = expressions.size();
  m_expressions.resize(size);
  for (unsigned int i = 0; i < size; ++i)
    m_expressions[i] = hlt::parseTriggerCondition(expressions[i]);
}

TriggerResultsFilter::~TriggerResultsFilter()
{
  for (unsigned int i = 0; i < m_expressions.size(); ++i)
    delete m_expressions[i];
}

//
// member functions
//

// ------------ method called to produce the data  ------------
bool TriggerResultsFilter::filter(edm::Event & event, const edm::EventSetup & setup)
{
  if (not m_eventCache.setEvent(event)) {
    // couldn't properly access all information from the Event
    return false;
  }

  // run the trigger results filters
  bool result = false;
  BOOST_FOREACH(hlt::TriggerExpressionEvaluator * expression, m_expressions)
    if ((*expression)(m_eventCache))
      result = true;
  
  return result; 
}

// register as framework plugin
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(TriggerResultsFilter);
