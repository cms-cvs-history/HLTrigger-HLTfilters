/** \class HLTFiltCand
 *
 * See header file for documentation
 *
 *  $Date: 2007/03/26 11:31:42 $
 *  $Revision: 1.1 $
 *
 *  \author Martin Grunewald
 *
 */

#include "HLTrigger/HLTfilters/interface/HLTFiltCand.h"

#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/EgammaCandidates/interface/Electron.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
#include "DataFormats/MuonReco/interface/Muon.h"
#include "DataFormats/JetReco/interface/CaloJet.h"
#include "DataFormats/METReco/interface/CaloMET.h"

#include "DataFormats/RecoCandidate/interface/RecoChargedCandidate.h"
#include "DataFormats/RecoCandidate/interface/RecoEcalCandidate.h"

#include "DataFormats/Common/interface/RefToBase.h"
#include "DataFormats/HLTReco/interface/HLTFilterObject.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

//
// constructors and destructor
//
 
HLTFiltCand::HLTFiltCand(const edm::ParameterSet& iConfig) :
  photTag_ (iConfig.getParameter<edm::InputTag>("photTag")),
  elecTag_ (iConfig.getParameter<edm::InputTag>("elecTag")),
  muonTag_ (iConfig.getParameter<edm::InputTag>("muonTag")),
  tausTag_ (iConfig.getParameter<edm::InputTag>("tausTag")),
  jetsTag_ (iConfig.getParameter<edm::InputTag>("jetsTag")),
  metsTag_ (iConfig.getParameter<edm::InputTag>("metsTag")),
  trckTag_ (iConfig.getParameter<edm::InputTag>("trckTag")),
  ecalTag_ (iConfig.getParameter<edm::InputTag>("ecalTag")),

  min_Pt_  (iConfig.getParameter<double>("MinPt"))
{
  LogDebug("") << "MinPt cut " << min_Pt_
   << " g: " << photTag_.encode()
   << " e: " << elecTag_.encode()
   << " m: " << muonTag_.encode()
   << " t: " << tausTag_.encode()
   << " j: " << jetsTag_.encode()
   << " M: " << metsTag_.encode()
   <<" TR: " << trckTag_.encode()
   <<" SC: " << ecalTag_.encode()
   ;

   //register your products
   produces<reco::HLTFilterObjectWithRefs>();
}

HLTFiltCand::~HLTFiltCand()
{
}

//
// member functions
//

// ------------ method called to produce the data  ------------
bool
HLTFiltCand::filter(edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace std;
   using namespace edm;
   using namespace reco;

   // All HLT filters must create and fill an HLT filter object,
   // recording any reconstructed physics objects satisfying (or not)
   // this HLT filter, and place it in the Event.

   // The filter object
   auto_ptr<HLTFilterObjectWithRefs> 
     filterobject (new HLTFilterObjectWithRefs(path(),module()));
   // Ref to Candidate objects to be recorded in filter object
   RefToBase<Candidate> ref;


   // Specific filter code

   // get hold of products from Event

   Handle<PhotonCollection>   photons;
   Handle<ElectronCollection> electrons;
   Handle<MuonCollection>     muons;
   Handle<CaloJetCollection>  taus;
   Handle<CaloJetCollection>  jets;
   Handle<CaloMETCollection>  mets;
   Handle<RecoChargedCandidateCollection> trcks;
   Handle<RecoEcalCandidateCollection>    ecals;

   iEvent.getByLabel(photTag_,photons  );
   iEvent.getByLabel(elecTag_,electrons);
   iEvent.getByLabel(muonTag_,muons    );
   iEvent.getByLabel(tausTag_,taus     );
   iEvent.getByLabel(jetsTag_,jets     );
   iEvent.getByLabel(metsTag_,mets     );
   iEvent.getByLabel(trckTag_,trcks    );
   iEvent.getByLabel(ecalTag_,ecals    );


   // look for at least one g,e,m,t,j,M,TR,SC above its pt cut

   // photons
   int nphot(0);
   PhotonCollection::const_iterator aphot(photons->begin());
   PhotonCollection::const_iterator ophot(photons->end());
   PhotonCollection::const_iterator iphot;
   for (iphot=aphot; iphot!=ophot; iphot++) {
     if (iphot->pt() >= min_Pt_) {
       nphot++;
       ref=RefToBase<Candidate>(PhotonRef(photons,distance(aphot,iphot)));
       filterobject->putParticle(ref);
     }
   }

   // electrons
   int nelec(0);
   ElectronCollection::const_iterator aelec(electrons->begin());
   ElectronCollection::const_iterator oelec(electrons->end());
   ElectronCollection::const_iterator ielec;
   for (ielec=aelec; ielec!=oelec; ielec++) {
     if (ielec->pt() >= min_Pt_) {
       nelec++;
       ref=RefToBase<Candidate>(ElectronRef(electrons,distance(aelec,ielec)));
       filterobject->putParticle(ref);
     }
   }

   // muon
   int nmuon(0);
   MuonCollection::const_iterator amuon(muons->begin());
   MuonCollection::const_iterator omuon(muons->end());
   MuonCollection::const_iterator imuon;
   for (imuon=amuon; imuon!=omuon; imuon++) {
     if (imuon->pt() >= min_Pt_) {
       nmuon++;
       ref=RefToBase<Candidate>(MuonRef(muons,distance(amuon,imuon)));
       filterobject->putParticle(ref);
     }
   }

   // taus (are stored as jets)
   int ntaus(0);
   CaloJetCollection::const_iterator ataus(taus->begin());
   CaloJetCollection::const_iterator otaus(taus->end());
   CaloJetCollection::const_iterator itaus;
   for (itaus=ataus; itaus!=otaus; itaus++) {
     if (itaus->pt() >= min_Pt_) {
       ntaus++;
       ref=RefToBase<Candidate>(CaloJetRef(taus,distance(ataus,itaus)));
       filterobject->putParticle(ref);
     }
   }

   // jets
   int njets(0);
   CaloJetCollection::const_iterator ajets(jets->begin());
   CaloJetCollection::const_iterator ojets(jets->end());
   CaloJetCollection::const_iterator ijets;
   for (ijets=ajets; ijets!=ojets; ijets++) {
     if (ijets->pt() >= min_Pt_) {
       njets++;
       ref=RefToBase<Candidate>(CaloJetRef(jets,distance(ajets,ijets)));
       filterobject->putParticle(ref);
     }
   }

   // mets
   int nmets(0);
   CaloMETCollection::const_iterator amets(mets->begin());
   CaloMETCollection::const_iterator omets(mets->end());
   CaloMETCollection::const_iterator imets;
   for (imets=amets; imets!=omets; imets++) {
     if (imets->pt() >= min_Pt_) {
       nmets++;
       ref=RefToBase<Candidate>(CaloMETRef(mets,distance(amets,imets)));
       filterobject->putParticle(ref);
     }
   }

   // trcks
   int ntrck(0);
   RecoChargedCandidateCollection::const_iterator atrcks(trcks->begin());
   RecoChargedCandidateCollection::const_iterator otrcks(trcks->end());
   RecoChargedCandidateCollection::const_iterator itrcks;
   for (itrcks=atrcks; itrcks!=otrcks; itrcks++) {
     if (itrcks->pt() >= min_Pt_) {
       ntrck++;
       ref=RefToBase<Candidate>(RecoChargedCandidateRef(trcks,distance(atrcks,itrcks)));
       filterobject->putParticle(ref);
     }
   }

   // ecals
   int necal(0);
   RecoEcalCandidateCollection::const_iterator aecals(ecals->begin());
   RecoEcalCandidateCollection::const_iterator oecals(ecals->end());
   RecoEcalCandidateCollection::const_iterator iecals;
   for (iecals=aecals; iecals!=oecals; iecals++) {
     if (iecals->pt() >= min_Pt_) {
       necal++;
       ref=RefToBase<Candidate>(RecoEcalCandidateRef(ecals,distance(aecals,iecals)));
       filterobject->putParticle(ref);
     }
   }

   // final filter decision:
   const bool accept ( (nphot>0) && (nelec>0) && (nmuon>0) && (ntaus>0) &&
		       (njets>0) && (nmets>0) && (ntrck>0) && (necal>0) );

   // All filters: put filter object into the Event
   iEvent.put(filterobject);

   LogDebug("") << "Number of g/e/m/t/j/M/SC/TR objects accepted:"
		<< " " << nphot
		<< " " << nelec
		<< " " << nmuon
		<< " " << ntaus
		<< " " << njets
		<< " " << nmets
		<< " " << necal
		<< " " << ntrck
                ;

   // return with final filter decision
   return accept;
}