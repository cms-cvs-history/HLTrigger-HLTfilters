#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "HLTrigger/HLTfilters/interface/HLTDoubletDZ.h"

#include "DataFormats/Common/interface/Handle.h"

#include "DataFormats/Common/interface/Ref.h"
#include "DataFormats/HLTReco/interface/TriggerFilterObjectWithRefs.h"

#include "DataFormats/Candidate/interface/Particle.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DataFormats/Math/interface/deltaR.h"

#include<cmath>

//
// constructors and destructor
//
template<typename T1, int Tid1, typename T2, int Tid2>
HLTDoubletDZ<T1,Tid1,T2,Tid2>::HLTDoubletDZ(const edm::ParameterSet& iConfig) : HLTFilter(iConfig),
  inputTag1_(iConfig.template getParameter<edm::InputTag>("inputTag1")),
  inputTag2_(iConfig.template getParameter<edm::InputTag>("inputTag2")),
  minDR_ (iConfig.template getParameter<double>("MinDR")),
  maxDZ_ (iConfig.template getParameter<double>("MaxDZ")),
  min_N_    (iConfig.template getParameter<int>("MinN")),
  coll1_(),
  coll2_()
{
   // same collections to be compared?
   same_ = (inputTag1_.encode()==inputTag2_.encode());
}

template<typename T1, int Tid1, typename T2, int Tid2>
HLTDoubletDZ<T1,Tid1,T2,Tid2>::~HLTDoubletDZ()
{
}

template<typename T1, int Tid1, typename T2, int Tid2>
void
HLTDoubletDZ<T1,Tid1,T2,Tid2>::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  edm::ParameterSetDescription desc;
  makeHLTFilterDescription(desc);
  desc.add<edm::InputTag>("inputTag1",edm::InputTag(""));
  desc.add<edm::InputTag>("inputTag2",edm::InputTag(""));
  desc.add<double>("MinDR",-1.0);
  desc.add<double>("MaxDZ",0.2);
  desc.add<int>("MinN",1);
  descriptions.add(std::string("hlt")+std::string(typeid(HLTDoubletDZ<T1,Tid1,T2,Tid2>).name()),desc);
}

// ------------ method called to produce the data  ------------
template<typename T1, int Tid1, typename T2, int Tid2>
bool
HLTDoubletDZ<T1,Tid1,T2,Tid2>::hltFilter(edm::Event& iEvent, const edm::EventSetup& iSetup, trigger::TriggerFilterObjectWithRefs & filterproduct)
{
   using namespace std;
   using namespace edm;
   using namespace reco;
   using namespace trigger;

   // All HLT filters must create and fill an HLT filter object,
   // recording any reconstructed physics objects satisfying (or not)
   // this HLT filter, and place it in the Event.

   bool accept(false);

   // get hold of pre-filtered object collections
   Handle<TriggerFilterObjectWithRefs> coll1,coll2;
   if (iEvent.getByLabel (inputTag1_,coll1) && iEvent.getByLabel (inputTag2_,coll2)) {
     coll1_.clear();
     coll1->getObjects(Tid1,coll1_);
     const size_type n1(coll1_.size());
     coll2_.clear();
     coll2->getObjects(Tid2,coll2_);
     const size_type n2(coll2_.size());

     if (saveTags()) {
       InputTag tagOld;
       tagOld=InputTag();
       for (size_type i1=0; i1!=n1; ++i1) {
	 const ProductID pid(coll1_[i1].id());
	 const string&    label(iEvent.getProvenance(pid).moduleLabel());
	 const string& instance(iEvent.getProvenance(pid).productInstanceName());
	 const string&  process(iEvent.getProvenance(pid).processName());
	 InputTag tagNew(InputTag(label,instance,process));
	 if (tagOld.encode()!=tagNew.encode()) {
	   filterproduct.addCollectionTag(tagNew);
	   tagOld=tagNew;
	 }
       }
       tagOld=InputTag();
       for (size_type i2=0; i2!=n2; ++i2) {
	 const ProductID pid(coll2_[i2].id());
	 const string&    label(iEvent.getProvenance(pid).moduleLabel());
	 const string& instance(iEvent.getProvenance(pid).productInstanceName());
	 const string&  process(iEvent.getProvenance(pid).processName());
	 InputTag tagNew(InputTag(label,instance,process));
	 if (tagOld.encode()!=tagNew.encode()) {
	   filterproduct.addCollectionTag(tagNew);
	   tagOld=tagNew;
	 }
       }
     }

     int n(0);
     T1Ref r1;
     T2Ref r2;
     Particle::LorentzVector p1,p2,p;
     for (unsigned int i1=0; i1!=n1; i1++) {
       r1=coll1_[i1];
       const reco::Candidate& candidate1(*r1);
       unsigned int I(0);
       if (same_) {I=i1+1;}
       for (unsigned int i2=I; i2!=n2; i2++) {
	 r2=coll2_[i2];
	 const reco::Candidate& candidate2(*r2);
	 if ( reco::deltaR(candidate1, candidate2) < minDR_ ) continue;
	 if ( fabs(candidate1.vz()-candidate2.vz()) > maxDZ_ ) continue;
	 n++;
	 filterproduct.addObject(Tid1,r1);
	 filterproduct.addObject(Tid2,r2);
       }
     }
     // filter decision
     accept = accept || (n>=min_N_);
   }

   return accept;
}