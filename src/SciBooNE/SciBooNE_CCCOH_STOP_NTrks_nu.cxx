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

#include "SciBooNE_CCCOH_STOP_NTrks_nu.h"

SciBooNE_CCCOH_STOP_NTrks_nu::SciBooNE_CCCOH_STOP_NTrks_nu(nuiskey samplekey){

  // Sample overview
  std::string descrip = "SciBooNE CC-coherent N. tracks.\n" \
    "Target: CH \n"                                                     \
    "Flux: SciBooNE FHC numu \n";

  // Common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("Q^{2} (GeV^{2})");
  fSettings.SetYTitle("Entries/0.05 (GeV^{2})");
  this->SetFitOptions("NOWIDTH");
  fSettings.SetEnuRange(0.0, 10.0);
  fSettings.DefineAllowedTargets("C,H");

  fSettings.SetTitle("SciBooNE CCCOH proton");
  fSettings.SetDataInput(  FitPar::GetDataBase()+"/SciBooNE/SB_COH_Fig7_CVs.csv");
  fSettings.SetHasExtraHistograms(true);
  fSettings.DefineAllowedSpecies("numu");

  SetDataFromTextFile(fSettings.GetDataInput());
  FinaliseSampleSettings();

  // Setup Plots
  this->muonStopEff = PlotUtils::GetTH2DFromRootFile(FitPar::GetDataBase()+"/SciBooNE/SciBooNE_stopped_muon_eff_nu.root", "stopped_muon_eff");

  this->fMCStack  = new SciBooNEUtils::ModeStack(fSettings.Name() + "_Stack",
                                                 "Mode breakdown" + fSettings.PlotTitles(),
                                                 PlotUtils::GetTH1DFromFile(fSettings.GetDataInput(), fSettings.GetName()));
  SetAutoProcessTH1(fMCStack);

  double nTargets = 10.6E6/13.*6.022E23;
  this->fScaleFactor = GetEventHistogram()->Integral()*13.*1E-38/double(fNEvents)*nTargets;

  FinaliseMeasurement();

};

void SciBooNE_CCCOH_STOP_NTrks_nu::FillEventVariables(FitEvent *event){

  nTrks = 0;
  this->mainIndex = SciBooNEUtils::GetMainTrack(event, this->muonStopEff, this->mainTrack, this->Weight);
  SciBooNEUtils::GetOtherTrackInfo(event, this->mainIndex, this->nProtons, this->nPiMus, this->nVertex, this->secondTrack);

  nTrks = nProtons + nPiMus;
  if (this->mainTrack) nTrks += 1;

  // Set X Variables
  fXVar = nTrks;
  return;
};


bool SciBooNE_CCCOH_STOP_NTrks_nu::isSignal(FitEvent *event){

  if (!this->mainTrack) return false;
  return true;
};

void SciBooNE_CCCOH_STOP_NTrks_nu::FillExtraHistograms(MeasurementVariableBox* vars, double weight){

  if (Signal) fMCStack->Fill(Mode, fXVar, weight);
  return;
};
