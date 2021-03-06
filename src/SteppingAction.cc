//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file hadronic/Hadr03/src/SteppingAction.cc
/// \brief Implementation of the SteppingAction class
//
// $Id: SteppingAction.cc 94619 2015-11-26 13:57:32Z gcosmo $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "SteppingAction.hh"
#include "Run.hh"
#include "HistoManager.hh"

#include "G4ParticleTypes.hh"
#include "G4RunManager.hh"
#include "G4HadronicProcess.hh"
                           
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction()
: G4UserSteppingAction()
{ 
    fHistoManager = NULL;    
}

SteppingAction::SteppingAction(HistoManager * histo) 
: G4UserSteppingAction()
{
    fHistoManager = histo;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
 Run* run 
   = static_cast<Run*>(G4RunManager::GetRunManager()->GetNonConstCurrentRun());
 
  fHistoManager->clear(); // clear the vectors in the histomanager
        
  // count processes
  // 
  const G4StepPoint* endPoint = aStep->GetPostStepPoint();
  G4VProcess* process   = 
                   const_cast<G4VProcess*>(endPoint->GetProcessDefinedStep());
  run->CountProcesses(process);
  
  // check that an real interaction occured (eg. not a transportation)
  G4StepStatus stepStatus = endPoint->GetStepStatus();
  G4bool transmit = (stepStatus==fGeomBoundary || stepStatus==fWorldBoundary);
  if (transmit) return;
                      
  //real processes : sum track length
  //
  G4double stepLength = aStep->GetStepLength();
  run->SumTrack(stepLength);
  
  //energy-momentum balance initialisation
  //
  const G4StepPoint* prePoint = aStep->GetPreStepPoint();
  G4double Q             = - prePoint->GetKineticEnergy();
  G4ThreeVector Pbalance = - prePoint->GetMomentum();
  
  //initialisation of the nuclear channel identification
  //
  G4ParticleDefinition* particle = aStep->GetTrack()->GetDefinition();
  G4String partName = particle->GetParticleName();
  G4String nuclearChannel = partName;
  G4HadronicProcess* hproc = dynamic_cast<G4HadronicProcess*>(process);
  G4String hproc_name = hproc->GetProcessName(); 
  const G4Isotope* target = NULL;
  if (hproc) target = hproc->GetTargetIsotope();
  G4String targetName = "XXXX";  
  if (target) targetName = target->GetName();
  nuclearChannel += " + " + targetName + " --> ";
  if (targetName == "XXXX") run->SetTargetXXX(true);
    
  //scattered primary particle (if any)
  //
  G4AnalysisManager* analysis = G4AnalysisManager::Instance();
  G4int ih = 1;
  if (aStep->GetTrack()->GetTrackStatus() == fAlive) {
    G4double energy = endPoint->GetKineticEnergy();      
    analysis->FillH1(ih,energy);
    //
    G4ThreeVector momentum = endPoint->GetMomentum();
    Q        += energy;
    Pbalance += momentum;
    //
    nuclearChannel += partName + " + ";
  
    int pNumber = IdentifyParticle(partName);
    fHistoManager->push_back(pNumber,energy);
  }  
 
  // for the ejectile ...

  //secondaries
  //
  const std::vector<const G4Track*>* secondary 
                                    = aStep->GetSecondaryInCurrentStep();  
  for (size_t lp=0; lp<(*secondary).size(); lp++) {
    particle = (*secondary)[lp]->GetDefinition(); 
    G4String name   = particle->GetParticleName();
    G4String type   = particle->GetParticleType();      
    G4double energy = (*secondary)[lp]->GetKineticEnergy();
    run->ParticleCount(name,energy);
    //energy spectrum
    ih = 0; 
         if (particle == G4Gamma::Gamma())       ih = 2;
    else if (particle == G4Neutron::Neutron())   ih = 3;
    else if (particle == G4Proton::Proton())     ih = 4;
    else if (particle == G4Deuteron::Deuteron()) ih = 5;
    else if (particle == G4Alpha::Alpha())       ih = 6;       
    else if (type == "nucleus")                  ih = 7;
    else if (type == "meson")                    ih = 8;
    else if (type == "baryon")                   ih = 9;        
    if (ih > 0) analysis->FillH1(ih,energy);
    //atomic mass
    if (type == "nucleus") {
      G4int A = particle->GetAtomicMass();
      analysis->FillH1(12, A);
    }
    
    //energy-momentum balance
    G4ThreeVector momentum = (*secondary)[lp]->GetMomentum();
    Q        += energy;
    Pbalance += momentum;
    //particle flag
    fParticleFlag[particle]++;
 
    int pNumber = IdentifyParticle(name);
    fHistoManager->push_back(pNumber,energy);  
    //int nucleus = 0;
    //if(type == "nucleus") {
    //    nucleus = 1;
    //    if(name == "neutron")                       ih = 3;
    //    else if(name == "proton")                   ih = 4;
    //    else if(name == "deuteron")                 ih = 5;
    //    else if(name == "C12" || name == "C13")     ih = 10;
    //}
    //G4int processNumber;
    //if(hproc_name == "hadElastic") processNumber = 0;
    //else if(hproc_name == "neutronInelastic") processNumber = 1;
    //else if(hproc_name == "nCapture") processNumber = 2;
    //else if(hproc_name == "nKiller") processNumber = 3;
    //else processNumber = -1;
    //G4int channelNumber = run->GetNuclChannelNumber(nuclearChannel);
    //analysis->FillNtupleIColumn(1,0,processNumber);
    //analysis->FillNtupleIColumn(1,1,channelNumber);
    //analysis->FillNtupleIColumn(1,2,ih);
    //analysis->FillNtupleDColumn(1,3,energy);
    //analysis->FillNtupleIColumn(1,4,nucleus);
    //analysis->AddNtupleRow(1);

    //if(type == "nucleus") G4cout << "nucleus = " << particle->GetParticleName() << G4endl;

  }
  
  
  if(fParticleFlag[G4Neutron::Neutron()] == 2 && fParticleFlag[G4Proton::Proton()] == 1) {
    analysis->FillH1(13,Q);
  }
  else if(hproc_name == "neutronInelastic" || hproc_name == "nCapture") {
    analysis->FillH1(14,Q);
  }
  else {
    analysis->FillH1(15,Q);
  }

  if(hproc_name == "hadElastic") {
    analysis->FillH1(16,Q);
  }
  else {
    analysis->FillH1(17,Q);
  }

  //energy-momentum balance
  G4double Pbal = Pbalance.mag();
  run->Balance(Pbal);
  ih = 10;
  analysis->FillH1(ih,Q);
  ih = 11;
  analysis->FillH1(ih,Pbal);  
  
  // nuclear channel
  const G4int kMax = 16;  
  const G4String conver[] = {"0 ","","2 ","3 ","4 ","5 ","6 ","7 ","8 ","9 ",
                             "10 ","11 ","12 ","13 ","14 ","15 ","16 "};
  std::map<G4ParticleDefinition*,G4int>::iterator ip;               
  for (ip = fParticleFlag.begin(); ip != fParticleFlag.end(); ip++) {
    particle = ip->first;
    G4String name = particle->GetParticleName();      
    G4int nb = ip->second;
    if (nb > kMax) nb = kMax;   
    G4String Nb = conver[nb];    
    if (particle == G4Gamma::Gamma()) {
     run->CountGamma(nb);
     Nb = "N ";
    } 
    if (ip != fParticleFlag.begin()) nuclearChannel += " + ";
    nuclearChannel += Nb + name;
  }
  
  run->RegisterProcessType(nuclearChannel,hproc_name);

  ///G4cout << "\n nuclear channel: " << nuclearChannel << G4endl;
  run->CountNuclearChannel(nuclearChannel, Q);
    
  fParticleFlag.clear();
  
  // filling the tree
  //G4int channelNumber = run->GetNuclChannelNumber(nuclearChannel);
  G4int channelNumber = IdentifyChannel(nuclearChannel);
  analysis->FillNtupleIColumn(1,0,channelNumber); // nuclear channel number (should be idenfified when EndOfRunAction() does its thing
  analysis->FillNtupleDColumn(1,1,Q); // Q value
  analysis->AddNtupleRow(1);
            
  // kill event after first interaction
  //
  G4RunManager::GetRunManager()->AbortEvent();  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int SteppingAction::IdentifyParticle(std::string name) {
    int num = -1;
    if(name == "gamma")         num = 2;
    else if(name == "neutron")  num = 3;
    else if(name == "proton")   num = 4;
    else if(name == "deuteron") num = 5;
    else if(name == "alpha")    num = 6;
    else if(name == "C12" || name == "C13")     num = 7;
    else if(name == "Be9" || name == "Be10")    num = 8;

    return num;
}

int SteppingAction::IdentifyChannel(std::string channel) { // wow this is gross...
    int num = -1;
         if (channel == "neutron + H1 --> neutron + proton + 0 neutron")            num = 0; 
    else if (channel == "neutron + H1 --> N gamma + 0 neutron + deuteron")          num = 1; 
    else if (channel == "neutron + H2 --> neutron + 0 neutron + deuteron")          num = 2; 
    else if (channel == "neutron + H2 --> proton + 2 neutron")                      num = 3;     
    else if (channel == "neutron + H2 --> N gamma + proton + 2 neutron")            num = 4; 
    else if (channel == "neutron + H2_isotope --> neutron + 0 neutron + deuteron")  num = 5;        
    else if (channel == "neutron + H2_isotope --> proton + 2 neutron")              num = 6;            
    else if (channel == "neutron + H2_isotope --> N gamma + proton + 2 neutron")    num = 7;         
    else if (channel == "neutron + H2_isotope --> N gamma + 0 neutron + triton")    num = 8;    
    else if (channel == "neutron + C12 --> neutron + 0 neutron + C12")              num = 9; 
    else if (channel == "neutron + C12 --> N gamma + neutron + C12")                num = 10; 
    else if (channel == "neutron + C12 --> 0 neutron + alpha + Be9")                num = 11;  
    else if (channel == "neutron + C12 --> N gamma + 0 neutron + alpha + Be9")      num = 12; 
    else if (channel == "neutron + C12 --> N gamma + 0 neutron + C13")              num = 13; 
    else if (channel == "neutron + C13 --> neutron + 0 neutron + C13")              num = 14; 
    else if (channel == "neutron + C13 --> N gamma + neutron + C13")                num = 15; 
    else if (channel == "neutron + C13 --> 0 neutron + alpha + Be10")               num = 16; 
    else if (channel == "neutron + C13 --> N gamma + 0 neutron + alpha + Be10")     num = 17; 
    else if (channel == "neutron + C13 --> N gamma + 0 neutron + C14")              num = 18; 

    return num;
}

