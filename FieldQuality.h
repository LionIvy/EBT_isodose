#ifndef FIELDQUALITY_H
#define FIELDQUALITY_H

#include "DoseVector.h"
#include "Target.h"

class FieldQuality: public DoseVector
{
public:
    FieldQuality(){};
    FieldQuality(Target &target, DoseVector &doseField);

    FieldQuality& operator= (const FieldQuality &other);


    void calibrateDoseField(double relDoseAtPoint, int relDosePointPosX, int relDosePointPosY, int relDosePointPosZ);
    void setPrescribedDose(double refIsodose);

    double getMinDoseAtTarget();

    double getMaxDoseAtTarget();

    double getMeanDoseAtTarget();

    double getCoverage();

    double getQualityOfCoverage(); //RTOG

    double getHomogeneityIndex(QString Formula);  //RTOG

    double getConformityIndex();   //RTOG

    double getSelectivityIndex();

    double getPaddickIndex();

    std::vector <std::pair <double,double> > getDVH();

    double getDoseAtVolume(double VolumePercent);


private:
    DoseVector doseField;
    //DoseVector cDoseField;

    double fieldDoseMax;
    //std::tuple<int,int,int> fieldDoseMaxValPos;
    int fieldDoseMaxValPosX, fieldDoseMaxValPosY, fieldDoseMaxValPosZ;
    double fieldDoseMaxRelVal = 100.0;

    double fieldDoseAtPoint;
    double relDoseAtPoint = 100.0;
    //std::tuple<int,int,int> relDosePointPos;
    int relDosePointPosX, relDosePointPosY, relDosePointPosZ;

    double calibrationUnit;

    double refIsodose = 95;
    double refISOdoseVolume;

    void evalISOdoseVolume(double refDose);


    Target target;
    std::vector<int> targetIndicesList;

    double targetVolume;
    double tgtVolCoveredByRefDose;

    double targetDoseMaximum = -1000;
    double targetDoseMinimum =  1000;

    void setTargetDoseRange();
    void evalTgtVolCovered(double refDose);


    std::vector <std::pair <double,double> > DVH;
    double doseStep = 0.001;// 1 is maximum
    void setDVH();
//    std::vector <double> doseHist;
//    std::vector <double> volumeHist;




};

#endif // FIELDQUALITY_H
