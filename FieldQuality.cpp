#include "FieldQuality.h"

#include "math.h"

FieldQuality::FieldQuality(Target &target, DoseVector &doseField)
{
    this->target = target;
    this->doseField = doseField; // Поле без нормировки


    target.setTargetIndicesList();
    targetIndicesList = target.getTargetIndicesList();
    targetVolume = targetIndicesList.size();

    fieldDoseMax = doseField.getDoseMaxValue();
    std::tuple<int,int,int> fieldDoseMaxValPos = doseField.getMaxValPosition();
    fieldDoseMaxValPosX = std::get<0>(fieldDoseMaxValPos);
    fieldDoseMaxValPosY = std::get<1>(fieldDoseMaxValPos);
    fieldDoseMaxValPosZ = std::get<2>(fieldDoseMaxValPos);

    //fieldDoseAtPoint = fieldDoseMax;
    relDoseAtPoint = 100.0;
    relDosePointPosX = fieldDoseMaxValPosX;
    relDosePointPosY = fieldDoseMaxValPosY;
    relDosePointPosZ = fieldDoseMaxValPosZ;

    calibrateDoseField(relDoseAtPoint, relDosePointPosX, relDosePointPosY, relDosePointPosZ);
    //setTargetDoseRange();



}

FieldQuality& FieldQuality::operator= (const FieldQuality &other)
{

    this->target = other.target;
//    this->doseField = other.doseField;
//    //this->cDoseField = other.cDoseField;

//    this->fieldDoseMax = other.fieldDoseMax;

//    this->fieldDoseMaxValPosX = other.fieldDoseMaxValPosX;
//    this->fieldDoseMaxValPosY = other.fieldDoseMaxValPosY;
//    this->fieldDoseMaxValPosZ = other.fieldDoseMaxValPosZ;
//    this->fieldDoseMaxRelVal = other.fieldDoseMaxRelVal;

//    this->fieldDoseAtPoint = other.fieldDoseAtPoint;
//    this->relDoseAtPoint = other.relDoseAtPoint;

//    this->relDosePointPosX = other.relDosePointPosX;
//    this->relDosePointPosY = other.relDosePointPosY;
//    this->relDosePointPosZ = other.relDosePointPosZ;

//    this->calibrationUnit = other.calibrationUnit;

//    this->refIsodose = other.refIsodose;
//    this->refISOdoseVolume = other.refISOdoseVolume;

//    this->target = other.target;
//    target.setTargetIndicesList();
//    //this->targetIndicesList = other.targetIndicesList;

//    this->targetVolume = other.targetVolume;
//    this->tgtVolCoveredByRefDose = other.tgtVolCoveredByRefDose;

//    this->targetDoseMaximum = other.targetDoseMaximum;
//    this->targetDoseMinimum = other.targetDoseMinimum;


//    this->DVH = other.DVH;
//    this->doseStep = other.doseStep;


    return *this;
}

void FieldQuality::calibrateDoseField(double relDoseAtPoint, int relDosePointPosX, int relDosePointPosY, int relDosePointPosZ)
{
    relDoseAtPoint *=0.01;

    fieldDoseAtPoint = doseField.element(relDosePointPosX,relDosePointPosY,relDosePointPosZ);
    calibrationUnit = relDoseAtPoint/fieldDoseAtPoint;


//    std::vector<double> doseFieldVector = getDoseVector();
//    std::vector<double>::iterator doseIt = doseFieldVector.begin();

//    for (; doseIt != doseFieldVector.end(); ++doseIt)
//    {
//       // doseVal = calibrationUnit* (*doseIt);

//        if ((calibrationUnit* (*doseIt)) >= refDoseVal){
//            refISOdoseVolume++;
//        }
//    }





    setTargetDoseRange();
    setDVH();
}

///refDose in %
void FieldQuality::setPrescribedDose(double refIsodose)
{
    setTargetDoseRange();

    this->refIsodose = refIsodose;
    evalTgtVolCovered(refIsodose);
    evalISOdoseVolume(refIsodose);
}

void FieldQuality::setTargetDoseRange()
{

    targetDoseMaximum = -1000;
    targetDoseMinimum =  1000;

    std::vector<int>::iterator itr = targetIndicesList.begin();
    int indx;
    for (; itr != targetIndicesList.end(); ++itr)
    {
        indx = *itr;
        if (doseField.at(indx) > targetDoseMaximum){
           targetDoseMaximum = doseField.at(indx);
        }

        if (doseField.at(indx) < targetDoseMinimum){
            targetDoseMinimum = doseField.at(indx);
        }
    }
    targetDoseMaximum *=calibrationUnit;
    targetDoseMinimum *=calibrationUnit;

}

double FieldQuality::getMaxDoseAtTarget()
{
    return 100*targetDoseMaximum;
}

double FieldQuality::getMinDoseAtTarget(){
    return 100*targetDoseMinimum;
}

double FieldQuality::getMeanDoseAtTarget(){

    std::vector<int>::iterator itr = targetIndicesList.begin();
    int indx;
    double doseVal, meanDose = 0;

    for (; itr != targetIndicesList.end(); ++itr)
    {
        indx = *itr;
        doseVal = calibrationUnit*doseField.at(indx);

        meanDose += doseVal;
    }

    meanDose = meanDose/targetVolume;

    return 100*meanDose;
}

double FieldQuality::getCoverage()
{
    double TV;
    targetVolume == 0 ? TV = 0.0000001 : TV = targetVolume;

    double CO = (tgtVolCoveredByRefDose)/TV;
    return 100*CO;
}

double FieldQuality::getQualityOfCoverage() //RTOG
{
    //setTargetDoseRange();
    double qualityOfCO = (100*targetDoseMinimum)/refIsodose;
    return 100*qualityOfCO;
}

double FieldQuality::getHomogeneityIndex(QString Formula = "D")  //RTOG
{
    //setTargetDoseRange();
    double HI = 0; // HomogeneityIndex

    if(Formula == "RTOG"){
       HI =  targetDoseMaximum/refIsodose;
    }else if(Formula == "A"){
      double D5 = getDoseAtVolume(5.0);
      double D95 = getDoseAtVolume(95.0);

      HI = D5/D95;

    }else if(Formula == "B"){
        double Dmax = targetDoseMaximum;
        double Dmin = targetDoseMinimum;

        HI = Dmax/Dmin;

    }else if(Formula == "C"){
        double D1 = getDoseAtVolume(1.0);
        double D98 = getDoseAtVolume(98.0);

        HI = 100*(D1-D98)/refIsodose;

    }else if(Formula == "D"){
        double D5 = getDoseAtVolume(5.0);
        double D95 = getDoseAtVolume(95.0);

        HI = 100*(D5-D95)/refIsodose;

    }else if(Formula == "E"){
        double Dmax = targetDoseMaximum;

        HI = Dmax/refIsodose;

    }else{
        double D5 = getDoseAtVolume(5.0);
        double D95 = getDoseAtVolume(95.0);

        HI = 100*(D5-D95)/refIsodose;
    }

    return HI;
}

double FieldQuality::getConformityIndex()  //RTOG
{
    double CI;
    double TV;
    targetVolume == 0 ? TV = 0.0000001 : TV = targetVolume;

    CI = 100*refISOdoseVolume/TV;
    return CI;
}

double FieldQuality::getSelectivityIndex(){
     //evalISOdoseVolume(refIsodose);
     double SI;
     double volRI;
     refISOdoseVolume == 0 ? volRI = 0.0000001 : volRI = refISOdoseVolume;

     SI = tgtVolCoveredByRefDose/volRI;
     return SI;
}

double FieldQuality::getPaddickIndex(){

    double volRI;
    refISOdoseVolume == 0 ? volRI = 0.0000001 : volRI = refISOdoseVolume;

    double A = (tgtVolCoveredByRefDose / targetVolume);
    double B = (tgtVolCoveredByRefDose / volRI);

    double pCI = A * B;

    return pCI;
}

void FieldQuality::setDVH()
{

    DVH.clear();

    std::vector <double> doseHist;
    std::vector <double> volumeHist;

    double doseVal;
    //double doseStep = 0.001;
    //double doseStep = (targetDoseMaximum-0)/Nsteps;
    int Nsteps = (targetDoseMaximum-0) / doseStep;


    doseHist.resize(Nsteps+1);
    volumeHist.resize(Nsteps+1);

    for (int n=0;n<=Nsteps; n++){
        doseVal= 0 + n*doseStep;
        doseHist[n] = doseVal;
    }


    std::vector<int>::iterator itr = targetIndicesList.begin();
    int indx;
    int topIndex;
    int n;
    for (; itr != targetIndicesList.end(); ++itr)
    {
        indx = *itr;
        doseVal = calibrationUnit*doseField.at(indx);

        topIndex = int(ceil(doseVal*Nsteps/targetDoseMaximum));

        for (n = 0; n <= topIndex;n++)
        {
            volumeHist[n]++;
        }
    }

    for (unsigned int i = 0; i < volumeHist.size(); i++)
    {
            volumeHist[i] =100. * volumeHist[i] / targetVolume;
            doseHist[i]*=100.;
            DVH.push_back(std::make_pair(doseHist[i],volumeHist[i]));
    }


}

std::vector <std::pair <double,double> > FieldQuality::getDVH(){
    return DVH;
}

double FieldQuality::getDoseAtVolume(double volumePercent){


    double doseAtVolume = -999;
    double vol1 , vol2  = 0;
    double dose1, dose2 = 0;

    double tang, shift;


    std::vector<std::pair<double, double>>::iterator itr = DVH.begin();


    for (; itr != (DVH.end()-1); ++itr)
    {
        vol1 = (*itr).second;
        vol2 = (*(itr+1)).second;

        if(vol1 == volumePercent){
           doseAtVolume =  (*itr).first;
        }else
        if (((vol1 < volumePercent)&&(volumePercent < vol2)) ||
            ((vol2 < volumePercent)&&(volumePercent < vol1))   )
        {
            dose1 = (*itr).first;
            dose2 = (*(itr+1)).first;

            tang = (dose1-dose2)/(vol1-vol2);
            shift = dose1 - tang*vol1;

            doseAtVolume = tang*volumePercent+shift;
        }

    }

    return doseAtVolume;
}

///refDose in %
void FieldQuality::evalTgtVolCovered(double refDose){

    tgtVolCoveredByRefDose = 0;
    std::vector<int>::iterator itr = targetIndicesList.begin();
    int indx;
    double doseVal;
    double refDoseVal = refDose * 0.01;
    for (; itr != targetIndicesList.end(); ++itr)
    {
        indx = *itr;
        doseVal = calibrationUnit*doseField.at(indx);

        if (doseVal >= refDoseVal){
            tgtVolCoveredByRefDose++;
        }
    }
}

///refDose in %
void FieldQuality::evalISOdoseVolume(double refDose){
    std::vector<double> doseFieldVector = doseField.getDoseVector();
    std::vector<double>::iterator doseIt = doseFieldVector.begin();

    double doseVal;

    double refDoseVal = (refDose)*0.01;

    refISOdoseVolume = 0;
    for (; doseIt != doseFieldVector.end(); ++doseIt)
    {
        doseVal = calibrationUnit* (*doseIt);

        if (doseVal >= refDoseVal){
            refISOdoseVolume++;
        }
    }
    //refISOdoseVolume
}
