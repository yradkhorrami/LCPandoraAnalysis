/**
 *  @file   PandoraAnalysis/include/PandoraPFACalibrator.h
 * 
 *  @brief  Header file for pandora pfa calibrator class.
 * 
 *  $Log: $
 */

#ifndef PANDORA_PFA_CALIBRATOR_H
#define PANDORA_PFA_CALIBRATOR_H

#include "marlin/Processor.h"

#include <vector>

class TFile;
class TH1F;
class TH2F;

//------------------------------------------------------------------------------------------------------------------------------------------

/**
PandoraPFACalibrator is a processor to aid calibration of PandoraPFA.
<pre>
There are three sets of constants that need to be calibrated:
  i) CaloDigi raw hit to GeV
 ii) PandoraPFA energy in GeV -> MIP equivalent conversion
iii) PandoraPFA MIP equivalent -> PFO energy
One might expect that factor iii) is the reciprocal of factor ii). However,
due to the isolation cuts used in PandoraPFA this is not quite the case.

MC Samples:
===========
  o the calibration constants will depend on the Mokka ECAL and HCAL 
    drivers, the GEANT4 physics list, and potentially the version of 
    GEANT4 used to generate the events.
  o to calibrate PandoraPFA generate the following samples with the
    same version of Mokka/Geant4 used for the main event samples:
       o 10000 10 GeV photons with flat phi and flat cosine(theta) distributions
       o 10000 10 GeV K0L with flat phi and flat cosine(theta) distributions
       o 10000 10 GeV mu+/-

Calibration Procedure:
======================
i) Calibration of Mokka hits
  o Using the photon sample, tune the calibration parameters in digitisation processor
    e.g. <parameter name="CalibrECAL" type="FloatVec">XX YY</parameter>
    to so that the histogram fCalEnergy is centred on 10 GeV. It is recommended 
    that the calibration constants for the different layers should be in the ratio
    of the absorber thicknesses, i.e. 1:2 for ILD_o1_v05.
  o Repeat with the 10 GeV KL sample to fix 
    <parameter name="CalibrHCAL" type="FloatVec">ZZ</parameter>
ii) PandoraPFA MIP calibration
  o Using the muon sample, tune the parameters:
    <parameter name="ECALMIPcalibration" type="float">AA</parameter>
    <parameter name="HCALMIPcalibration" type="float">BB</parameter>
    so that fEcalMIPcorr and fHcalMIPcorr peak at approximately one. This
    set the scale for the energy deposited by a "minimum ionizing particle".
iii) PandoraPFA ECAL/HCAL calibration
  o Using the photon sample run PandoraPFA and PandoraPFACalibrator and determine
    <parameter name="ECalToEMGeVCalibration" type="float">XX</parameter>
    <parameter name="HCalToEMGeVCalibration" type="float">YY</parameter>
    so that fPFA peaks at 10 GeV. It is recommended to set the response to
    hadrons to be the same as that for EM showers, i.e. ECalToEMGeVCalibration = HCalToEMGeVCalibration
  o Using the K0L sample, examine the profile of a plot of ECALToHAD energy vs HCALToHAD energy,
    expecting the profile to be a straight line of gradient -1, intercepting both axes at 10GeV.
    However, it may be necessary, for optimal jet energy resolution, to deweight the ECALToHAD contribution.
    Set the parameters below accordingly:
    <parameter name="ECalToHadGeVCalibrationBarrel" type="float">AA</parameter>
    <parameter name="ECalToHadGeVCalibrationEndCap" type="float">BB</parameter>
    <parameter name="HCalToHadGeVCalibration" type="float">CC</parameter>
</pre>
*/
class PandoraPFACalibrator : public marlin::Processor
{
public:
    virtual Processor *newProcessor();
    PandoraPFACalibrator() ;

    /**
     *   @brief Called at the begin of the job before anything is read. Use to initialize the processor, e.g. book histograms.
     */
    virtual void init() ;

    /**
     *  @brief  Called for every run.
     */
    virtual void processRunHeader( LCRunHeader* run ) ;

    /**
     *  @brief  Called for every event - the working horse.
     */
    virtual void processEvent( LCEvent * evt ) ; 

    /**
     *  @brief  Check
     */
    virtual void check( LCEvent * evt ) ; 

    /**
     *  @brief  Called after data processing for clean up.
     */
    virtual void end() ;

private:
    /**
     *  @brief  
     * 
     *  @param  pLCEvent
     *  @param  collectionNames
     *  @param  cosTheta
     */
    void ReadMCParticles(LCEvent *pLCEvent, const LCStrVec &collectionNames, float &cosTheta) const;

    /**
     *  @brief  
     * 
     *  @param  pLCEvent
     *  @param  collectionNames
     *  @param  hitEnergySum
     *  @param  mipConstant
     *  @param  pMipPlot
     *  @param  pMipPlotCorrected
     *  @param  pLayerEncoding
     *  @param  pEnergyByLayerPlot
     */
    void ReadHitEnergies(LCEvent *pLCEvent, const LCStrVec &collectionNames, float &hitEnergySum, const float mipConstant = -1.f,
        TH1F *const pMipPlot = NULL, TH1F *const pMipPlotCorrected = NULL, const char *const pLayerEncoding = NULL, TH1F *const pEnergyByLayerPlot = NULL) const;

    /**
     *  @brief  
     * 
     *  @param  pLCEvent
     *  @param  collectionNames
     *  @param  pfoEnergySum
     *  @param  cosTheta
     */
    void ReadPfoCollections(LCEvent *pLCEvent, const LCStrVec &collectionNames, float &pfoEnergySum, float &cosTheta) const;

    LCStrVec        m_inputMCParticleCollections;                   ///< Legacy parameter only
    std::string     m_particleCollectionName;                       ///< Legacy parameter only
    LCStrVec        m_mcPfoCollections;                             ///< New, preferred input parameter
    LCStrVec        m_recoPfoCollections;                           ///< New, preferred input parameter

    LCStrVec        m_ecalBarrelCollections;                        ///< 
    LCStrVec        m_ecalEndCapCollections;                        ///< 
    LCStrVec        m_hcalCollections;                              ///< 
    LCStrVec        m_muonCollections;                              ///< 
    LCStrVec        m_lcalCollections;                              ///< 
    LCStrVec        m_bcalCollections;                              ///< 
    LCStrVec        m_lhcalCollections;                             ///< 

    int             m_nRun;                                         ///< 
    int             m_nEvt;                                         ///< 

    float           m_ecalToMIP;                                    ///< 
    float           m_hcalToMIP;                                    ///< 
    float           m_muonToMIP;                                    ///< 
    float           m_ecalToEMGeVCalibration;                       ///< 
    float           m_hcalToHadGeVCalibration;                      ///< 
    float           m_ecalToHadGeVCalibrationBarrel;                ///< 
    float           m_ecalToHadGeVCalibrationEndCap;                ///< 
    float           m_hcalToEMGeVCalibration;                       ///< 
    float           m_maxHCalHitHadronicEnergy;                     ///< 

    float           m_zOfEndCap;                                    ///< 

    TFile          *m_pTFile;                                       ///< 
    std::string     m_rootFile;                                     ///< 

    TH1F           *m_PFA;                                          ///< 
    TH1F           *m_PFAB;                                         ///< 
    TH2F           *m_PFAVsCosTheta;                                ///< 
    TH2F           *m_PFAVsCosThetaR;                               ///< 
    TH2F           *m_PFAVsCosThetaF;                               ///< 
    TH1F           *m_PFAE;                                         ///< 
    TH1F           *m_PFAH;                                         ///< 
    TH1F           *m_PFAM;                                         ///< 

    TH1F           *m_EcalEnergy;                                   ///< 
    TH1F           *m_HcalEnergy;                                   ///< 
    TH1F           *m_MuonEnergy;                                   ///< 
    TH1F           *m_LcalEnergy;                                   ///< 
    TH1F           *m_CalEnergy;                                    ///< 
    TH2F           *m_EcalHcalEnergyEM;                             ///< 
    TH2F           *m_EcalHcalEnergyHAD;                            ///< 
    TH2F           *m_EcalBarrelHcalEnergyEM;                       ///< 
    TH2F           *m_EcalEndCapHcalEnergyEM;                       ///< 
    TH2F           *m_EcalBarrelHcalEnergyHAD;                      ///< 
    TH2F           *m_EcalEndCapHcalEnergyHAD;                      ///< 
    TH1F           *m_CalEnergyE;                                   ///< 
    TH1F           *m_CalEnergyH;                                   ///< 
    TH1F           *m_CalEnergyM;                                   ///< 
    TH2F           *m_CalEnergyVsCosTheta;                          ///< 
    TH2F           *m_CalEnergyVsCosThetaR;                         ///< 
    TH1F           *m_EcalBarrelEnergyByLayer;                      ///< 
    TH1F           *m_EcalEndCapEnergyByLayer;                      ///< 

    TH1F           *m_EcalBarrelMIP;                                ///< 
    TH1F           *m_EcalEndCapMIP;                                ///< 
    TH1F           *m_HcalMIP;                                      ///< 
    TH1F           *m_MuonMIP;                                      ///< 
    TH1F           *m_EcalBarrelMIPcorr;                            ///< 
    TH1F           *m_EcalEndCapMIPcorr;                            ///< 
    TH1F           *m_HcalMIPcorr;                                  ///< 
    TH1F           *m_MuonMIPcorr;                                  ///< 
};

//------------------------------------------------------------------------------------------------------------------------------------------

inline marlin::Processor *PandoraPFACalibrator::newProcessor()
{
    return new PandoraPFACalibrator;
}

#endif // #ifndef PANDORA_PFA_CALIBRATOR_H
