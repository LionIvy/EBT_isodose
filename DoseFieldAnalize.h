#ifndef DOSEFIELDANALIZE_H
#define DOSEFIELDANALIZE_H

#include "Target.h"
#include "DoseVector.h"

#include "QString"

typedef std::vector<int> intVector;
typedef std::vector<double> dblVector;
typedef std::vector<std::pair<double,double>> vectorPair;

namespace analize {
    double getDoseMinimum(intVector targetIndicesList, DoseVector& doseDistribution);

    double getDoseMaximum(intVector targetIndicesList, DoseVector& doseDistribution);

    double getMeanDose(intVector targetIndicesList, DoseVector& doseDistribution);

    double getDoseValDeviation(intVector targetIndicesList, DoseVector& doseDistribution,double doseValue);

    vectorPair getDVH(intVector targetIndicesList, DoseVector& doseDistribution);

    void getDVH(intVector targetIndicesList, DoseVector& doseDistribution, dblVector &doseHist, dblVector &volumeHist);

    double getDoseAtVolume(double volumePercent, dblVector &doseHist, dblVector &volumeHist);

    double getDoseAtVolume(double volumePercent, vectorPair DVH);

    double getDoseAtVolume(intVector targetIndicesList, DoseVector& doseDistribution,double volumePercent);

    double getHomogeneityIndex(intVector targetIndicesList, DoseVector& doseDistribution, double refIsodose, std::string Formula = "D");

}








#endif // DOSEFIELDANALIZE_H
