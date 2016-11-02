#include "T2K_CC1pip_H2O_XSec_1DEnuMB_nu.h"

// The constructor
T2K_CC1pip_H2O_XSec_1DEnuMB_nu::T2K_CC1pip_H2O_XSec_1DEnuMB_nu(std::string inputfile, FitWeight *rw, std::string  type, std::string fakeDataFile){

  measurementName = "T2K_CC1pip_H2O_XSec_1DEnuMB_nu";
  plotTitles = "; E_{#nu} (GeV); #sigma(E_{#nu}) (cm^{2}/nucleon)";
  EnuMin = 0.;
  EnuMax = 10.;
  isDiag = false;
  Measurement1D::SetupMeasurement(inputfile, type, rw, fakeDataFile);

  this->SetDataValues(std::string(std::getenv("EXT_FIT"))+"/data/T2K/CC1pip/H2O/T2K_CC1pi_FGD2_water3.root");
  this->SetCovarMatrix(std::string(std::getenv("EXT_FIT"))+"/data/T2K/CC1pip/H2O/T2K_CC1pi_FGD2_water3.root");

  this->SetupDefaultHist();

  this->scaleFactor = (this->eventHist->Integral("width")*1E-38)/double(nevents);
};

void T2K_CC1pip_H2O_XSec_1DEnuMB_nu::SetDataValues(std::string fileLocation) {
  std::cout << "Reading: " << this->measurementName << "\nData: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  dataHist = (TH1D*)(dataFile->Get("EnuRec_MB/hResultTot"))->Clone();

  dataHist->SetDirectory(0); //should disassociate dataHist with dataFile
  dataHist->SetNameTitle((measurementName+"_data").c_str(), (measurementName+"_MC"+plotTitles).c_str());

  dataFile->Close();
};

void T2K_CC1pip_H2O_XSec_1DEnuMB_nu::SetCovarMatrix(std::string fileLocation) {
  std::cout << "Covariance: " << fileLocation.c_str() << std::endl;
  TFile *dataFile = new TFile(fileLocation.c_str()); //truly great .root file!

  TH2D *covarMatrix = (TH2D*)(dataFile->Get("EnuRec_MB/TotalCovariance"))->Clone();

  int nBinsX = covarMatrix->GetXaxis()->GetNbins();
  int nBinsY = covarMatrix->GetYaxis()->GetNbins();

  if ((nBinsX != nBinsY)) std::cerr << "covariance matrix not square!" << std::endl;

  this->covar = new TMatrixDSym(nBinsX);

  for (int i = 1; i < nBinsX+1; i++) {
    for (int j = 1; j < nBinsY+1; j++) {
      (*this->covar)(i-1, j-1) = covarMatrix->GetBinContent(i, j)*1E75;
    }
  } //should now have set covariance, I hope

  TDecompChol tempMat = TDecompChol(*this->covar);
  this->covar = new TMatrixDSym(nBinsX, tempMat.Invert().GetMatrixArray(), "");
  *this->covar *= 1E75*1E-76; // 1E-76 comes from StatUtils::GetChi2FromCov

  return;
};

void T2K_CC1pip_H2O_XSec_1DEnuMB_nu::FillEventVariables(FitEvent *event) {

  TLorentzVector Pnu = (event->PartInfo(0))->fP;
  TLorentzVector Ppip;
  TLorentzVector Pmu;

  // Loop over the particle stack
  for (int j = 2; j < event->Npart(); ++j){
    if (!(event->PartInfo(j))->fIsAlive && (event->PartInfo(j))->fStatus != 0) continue;
    int PID = (event->PartInfo(j))->fPID;
    if (PID == 211) {
      Ppip = event->PartInfo(j)->fP;
    } else if (PID == 13) {
      Pmu = (event->PartInfo(j))->fP;  
    }
  }

  double Enu = FitUtils::EnuCC1piprec(Pnu, Pmu, Ppip);

  this->X_VAR = Enu;

  return;
};


//******************************************************************** 
bool T2K_CC1pip_H2O_XSec_1DEnuMB_nu::isSignal(FitEvent *event) {
//******************************************************************** 
// Warning: The H2O analysis has different signal definition to the CH analysis!
  return SignalDef::isCC1pip_T2K_H2O(event, EnuMin, EnuMax);
}