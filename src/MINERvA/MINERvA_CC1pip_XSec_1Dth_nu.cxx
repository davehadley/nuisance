// Copyright 2016 L. Pickering, P Stowell, R. Terri, C. Wilkinson, C. Wret

/*******************************************************************************
*    This file is part of NUISANCE.
*
*    NUISANCE is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    NUISANCE is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with NUISANCE.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#include "MINERvA_SignalDef.h"

#include "MINERvA_CC1pip_XSec_1Dth_nu.h"



//********************************************************************
MINERvA_CC1pip_XSec_1Dth_nu::MINERvA_CC1pip_XSec_1Dth_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "MINERvA_CC1pip_XSec_1Dth_nu sample. \n" \
                        "Target: CH \n" \
                        "Flux: MINERvA Forward Horn Current nue + nuebar \n" \
                        "Signal: Any event with 1 electron, any nucleons, and no other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("#theta_{#pi} (degrees)");
  fSettings.SetYTitle("d#sigma/d#theta_{#pi} (cm^{2}/degrees/nucleon)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/FULL");
  fSettings.SetEnuRange(1.5, 10.0);
  fSettings.DefineAllowedTargets("C,H");
  fSettings.DefineAllowedSpecies("numu");

  fFullPhaseSpace = !fSettings.Found("name", "_20deg");
  fFluxCorrection = fSettings.Found("name","fluxcorr");
  fSettings.SetTitle("MINERvA_CC1pip_XSec_1Dth_nu");
  fIsShape = fSettings.Found("type","shape");

  if (fFullPhaseSpace){
    if (fIsShape) {
      fSettings.SetDataInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pip/ccpip_theta_shape.csv");
      fSettings.SetCovarInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pip/ccpip_theta_cov_shape.csv");
    } else {
      fSettings.SetDataInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pip/ccpip_theta.csv");
      fSettings.SetCovarInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pip/ccpip_theta_cov.csv");
    }
  // Restricted Phase Space
  } else {
      if (fIsShape) {
        fSettings.SetDataInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pip/ccpip_theta_20_shape.csv");
        fSettings.SetCovarInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pip/ccpip_theta_20_cov_shape.csv");
    } else {
      fSettings.SetDataInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pip/ccpip_theta_20.csv");
      fSettings.SetCovarInput( GeneralUtils::GetTopLevelDir()+"/data/MINERvA/CC1pip/ccpip_theta_20_cov.csv");
    }
  }

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) / double(fNEvents) / TotalIntegratedFlux("width");

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  SetCorrelationFromTextFile( fSettings.GetCovarInput() );

  // Scale the MINERvA data to account for the flux difference
  if (fFluxCorrection) {
    for (int i = 0; i < fDataHist->GetNbinsX() + 1; i++) {
      fDataHist->SetBinContent(i + 1, fDataHist->GetBinContent(i + 1) * 1.11);
    }
  }

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};

void MINERvA_CC1pip_XSec_1Dth_nu::FillEventVariables(FitEvent *event) {

  if (event->NumFSParticle(PhysConst::pdg_charged_pions) == 0 ||
      event->NumFSParticle(13) == 0)
    return;

  TLorentzVector Pnu  = event->GetNeutrinoIn()->fP;
  TLorentzVector Ppip = event->GetHMFSParticle(PhysConst::pdg_charged_pions)->fP;
  TLorentzVector Pmu  = event->GetHMFSParticle(13)->fP;

  double hadMass = FitUtils::Wrec(Pnu, Pmu);
  double th      = -999;

  if (hadMass > 100 && hadMass < 1400)
    th = (180./M_PI)*FitUtils::th(Pnu, Ppip);

  fXVar = th;

  return;
};

//********************************************************************
bool MINERvA_CC1pip_XSec_1Dth_nu::isSignal(FitEvent *event) {
//********************************************************************
  // Last false refers to that this is NOT the restricted MINERvA phase space, in which only forward-going muons are accepted
  return SignalDef::isCC1pip_MINERvA(event, EnuMin, EnuMax, !fFullPhaseSpace);
}
