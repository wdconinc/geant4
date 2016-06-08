//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: G4MassImportanceProcess.cc,v 1.3 2002/04/09 17:40:15 gcosmo Exp $
// GEANT4 tag $Name: geant4-04-01 $
//
// ----------------------------------------------------------------------
// GEANT 4 class source file
//
// G4MassImportanceProcess.cc
//
// ----------------------------------------------------------------------

#include "G4MassImportanceProcess.hh"
#include "G4VImportanceAlgorithm.hh"
#include "G4ImportanceFinder.hh"
#include "G4PTouchableKey.hh"

G4MassImportanceProcess::
G4MassImportanceProcess(const G4VImportanceAlgorithm &aImportanceAlgorithm,
			const G4VIStore &aIstore,
			const G4String &aName)
 : G4VProcess(aName),
   fImportanceAlgorithm(aImportanceAlgorithm),
   fImportanceFinder(new G4ImportanceFinder(aIstore))
{
  fParticleChange = new G4ParticleChange;
  G4VProcess::pParticleChange = fParticleChange;
}

G4MassImportanceProcess::~G4MassImportanceProcess()
{
  delete fImportanceFinder;
  delete fParticleChange;
}

G4double G4MassImportanceProcess::
PostStepGetPhysicalInteractionLength(const G4Track& aTrack,
				     G4double   previousStepSize,
				     G4ForceCondition* condition)
{
  *condition = Forced;
  return kInfinity;
}
  
G4VParticleChange *
G4MassImportanceProcess::PostStepDoIt(const G4Track &aTrack,
				      const G4Step &aStep)
{
  fParticleChange->Initialize(aTrack);
  if (aStep.GetPostStepPoint()->GetStepStatus() == fGeomBoundary
      && aStep.GetStepLength() > kCarTolerance) {
    if (aTrack.GetTrackStatus()==fStopAndKill) {
      G4cout << "G4MassImportanceProcess::PostStepDoIt StopAndKill" << G4endl;
    }

    G4StepPoint *prepoint = aStep.GetPreStepPoint();
    G4StepPoint *postpoint = aStep.GetPostStepPoint();
  

    G4PTouchableKey prekey(*(prepoint->GetPhysicalVolume()), 
			 prepoint->GetTouchable()->GetReplicaNumber());
    G4PTouchableKey postkey(*(postpoint->GetPhysicalVolume()), 
			  postpoint->GetTouchable()->GetReplicaNumber());

    G4Nsplit_Weight nw = fImportanceAlgorithm.
      Calculate(fImportanceFinder->
		GetIPre_over_IPost(prekey, postkey),
		aTrack.GetWeight());
    fImportancePostStepDoIt.DoIt(aTrack, fParticleChange, nw);
  }
  return fParticleChange;
}