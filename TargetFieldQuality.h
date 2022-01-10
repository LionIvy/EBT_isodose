#ifndef TARGETFIELDQUALITY_H
#define TARGETFIELDQUALITY_H

#include "Target.h"
#include "DoseVector.h"

#include "QString"

typedef std::vector <std::pair <double,double> > pairOfVectors;

struct DoseVolHyst{
    std::vector <double> dose;
    std::vector <double> volume;
};

//struct TargetObj{
//    QString targetName;
//    Target form;
//    int volume;
//    std::vector <int> indicesList;

//    double doseMinimum;
//    double doseMaximum;

//    double meanDose;
//    double meanDoseDeviation;

//    double dose50Vol;
//    double dose50VolDeviation;

//    //std::vector <std::pair <double,double> > DVH
//    DoseVolHyst DVH;
//    double HI;



//};



class TargetFieldQuality
{
public:
    TargetFieldQuality(){};
    TargetFieldQuality(Target form, QString targetName, DoseVector& doseDistribution );
    ~TargetFieldQuality();

    //DoseVector& operator=(const DoseVector &other);
    TargetFieldQuality& operator=(const TargetFieldQuality& other);

    //TargetObj& getTargetObject();
    double getDoseAtVolume(double volumePercent);
    double getHomogeneityIndex(double refIsodose, std::string Formula = "D");

private:
    DoseVector doseDistribution;
    //TargetObj target;

    QString targetName;
    double doseMinimum;
    double doseMaximum;

    double meanDose;
    double meanDoseDeviation;
    int volume;


    //pairOfVectors DVH;
//    DoseVolHyst DVH;
//    std::vector <double> doseHist;
//    std::vector <double> volumeHist;

    void setDVH();


};

#endif // TARGETFIELDQUALITY_H
