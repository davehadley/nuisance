// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NuFiX.
*
*    NuFiX is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NuFiX is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NuFiX.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "BEBC_CC1npim_XSec_1DEnu_antinu.h"

// The constructor
BEBC_CC1npim_XSec_1DEnu_antinu::BEBC_CC1npim_XSec_1DEnu_antinu(std::string inputfile, FitWeight *rw, std::string type, std::string fakeDataFile){

  measurementName = "BEBC_CC1npim_XSec_1DEnu_antinu";
  plotTitles = "; E_{#nu} (GeV); #sigma(E_{#nu}) (cm^{2}/neutron)";
  EnuMin = 5.;
  EnuMax = 200.;
  isDiag = true;
  normError = 0.20;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/BEBC/theses/BEBC_theses_ANU_CC1pi-_nFin_W14.txt");
  this->SetupDefaultHist();

  fullcovar = StatUtils::MakeDiagonalCovarMatrix(dataHist);
  covar     = StatUtils::GetInvert(fullcovar);

  this->scaleFactor = this->eventHist->Integral("width")*double(1E-38)/double(nevents)*(16./8.);
};


void BEBC_CC1npim_XSec_1DEnu_antinu::FillEventVariables(FitEvent *event) {
    
  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Pn;
  TLorentzVector Ppim;
  TLorentzVector Pmu;

  // Loop over the particle stack
  for (int j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == -211) {
      Ppim = event->PartInfo(j)->fP;
    } else if (PID == 2112) {
      Pn = event->PartInfo(j)->fP;
    } else if (PID == -13) {
      Pmu = (event->PartInfo(j))->fP;  
    }
  }

  double hadMass = FitUtils::MpPi(Pn, Ppim);
  double Enu;

  if (hadMass < 1400) {
    Enu = FitUtils::EnuCC1piprec(Pnu, Pmu, Ppim);
  } else {
    Enu = -1.0;
  }

  this->X_VAR = Enu;

  return;
};




bool BEBC_CC1npim_XSec_1DEnu_antinu::isSignal(FitEvent *event) {

  if ((event->PartInfo(0))->fPID != -14) return false;

  if (((event->PartInfo(0))->fP.E() < this->EnuMin*1000.) || ((event->PartInfo(0))->fP.E() > this->EnuMax*1000.)) return false; 

  if (((event->PartInfo(2))->fPID != -13) && ((event->PartInfo(3))->fPID != -13)) return false; 

  int pimCnt = 0;
  int lepCnt = 0;
  int neutronCnt = 0;

  for (int j = 2; j < event->Npart(); j++) {
    if (!((event->PartInfo(j))->fIsAlive) && (event->PartInfo(j))->fStatus != 0) continue; //move to next particle if NOT ALIVE and NOT NORMAL
    int PID = (event->PartInfo(j))->fPID;
    if (PID == -13) {
      lepCnt++;
    } else if (PID == -211) {
      pimCnt++;
    } else if (PID == 2112) {
      neutronCnt++;
    } else {
      return false; // require only three prong events! (allow photons?)
    }
  }

  // don't think there's away of implementing spectator proton cuts in NEUT?
  // 100 MeV or larger protons

  if (pimCnt != 1) return false;
  if (lepCnt != 1) return false;
  if (neutronCnt != 1) return false;

  return true;
}

