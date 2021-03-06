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

#include "T2K_SignalDef.h"

#include "T2K_CC0pinp_STV_XSec_1Ddpt_nu.h"


//********************************************************************
T2K_CC0pinp_STV_XSec_1Ddpt_nu::T2K_CC0pinp_STV_XSec_1Ddpt_nu(nuiskey samplekey) {
//********************************************************************

  // Sample overview ---------------------------------------------------
  std::string descrip = "T2K_CC0pinp_STV_XSec_1Ddpt_nu sample. \n" \
                        "Target: CH \n" \
                        "Flux: MINERvA Forward Horn Current nue + nuebar \n" \
                        "Signal: Any event with 1 electron, any nucleons, and no other FS particles \n";

  // Setup common settings
  fSettings = LoadSampleSettings(samplekey);
  fSettings.SetDescription(descrip);
  fSettings.SetXTitle("#delta#it{p}_{T} (GeV c^{-1})");
  fSettings.SetYTitle("#frac{d#sigma}{d#delta#it{p}_{T}} (cm^{2} neutron^{-1} GeV^{-1} c)");
  fSettings.SetAllowedTypes("FIX,FREE,SHAPE/DIAG,FULL/NORM/MASK", "FIX/DIAG");
  fSettings.SetEnuRange(0.0, 50.0);
  fSettings.DefineAllowedTargets("C,H");

  // CCQELike plot information
  fSettings.SetTitle("T2K_CC0pinp_STV_XSec_1Ddpt_nu");
  fSettings.SetDataInput(  GeneralUtils::GetTopLevelDir() + "/data/T2K/T2K_CC0pinp_STV_XSec_1Ddpt_nu.dat");
  fSettings.DefineAllowedSpecies("numu");

  FinaliseSampleSettings();

  // Scaling Setup ---------------------------------------------------
  // ScaleFactor automatically setup for DiffXSec/cm2/Nucleon
  fScaleFactor = GetEventHistogram()->Integral("width") * double(1E-38) *
                 (13.0 / 6.0 /*Data is /neutron */) /
                 (double(fNEvents) * TotalIntegratedFlux("width"));

  // Plot Setup -------------------------------------------------------
  SetDataFromTextFile( fSettings.GetDataInput() );
  ScaleData(1E-38);
  SetCovarFromDiagonal();

  // Final setup  ---------------------------------------------------
  FinaliseMeasurement();

};




void T2K_CC0pinp_STV_XSec_1Ddpt_nu::FillEventVariables(FitEvent *event) {
  fXVar = FitUtils::Get_STV_dpt(event, 14, true) / 1000.0;
  return;
};

//********************************************************************
bool T2K_CC0pinp_STV_XSec_1Ddpt_nu::isSignal(FitEvent *event)
//********************************************************************
{
  return SignalDef::isT2K_CC0pi_STV(event, EnuMin, EnuMax);
}
