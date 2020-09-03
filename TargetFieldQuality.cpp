#include "TargetFieldQuality.h"
#include "QDebug"


TargetFieldQuality::TargetFieldQuality(Target form, QString targetName, DoseVector& doseDistribution ){

//    this->doseDistribution = doseDistribution;

//    target.targetName = targetName;
//    target.form = form;
//    form.setTargetIndicesList();
//    std::vector <int> indicesList = form.getTargetIndicesList();
//    volume = indicesList.size();
//    target.volume = volume;
//    target.indicesList = indicesList;



//    double targetDoseMaximum = -1000;
//    double targetDoseMinimum =  1000;
//    double targetIntegrDose =  0;
//    double targetMeanDose =  0;
//    double doseValue;

//    std::vector<int>::iterator itr = indicesList.begin();
//    int indx, N=0;
//    for (; itr != indicesList.end(); ++itr)
//    {
//        indx = *itr;
//        doseValue = doseDistribution.at(indx);
//        if (doseValue > targetDoseMaximum){
//            targetDoseMaximum = doseValue;
//        }

//        if (doseValue < targetDoseMinimum){
//            targetDoseMinimum = doseValue;
//        }
//        targetIntegrDose+= doseValue;
//        N++;
//    }
//    targetMeanDose = targetIntegrDose/N;
//    target.doseMaximum = targetDoseMaximum;
//    target.doseMinimum = targetDoseMinimum;
//    target.meanDose = targetMeanDose;

//    double deviation=0;
//    double stdDev;
//    itr = indicesList.begin();
//    for (; itr != indicesList.end(); ++itr)
//    {
//        indx = *itr;
//        doseValue = doseDistribution.at(indx);
//        deviation += (doseValue-targetMeanDose)*(doseValue-targetMeanDose);
//    }
//    stdDev = sqrt(deviation/N);
//    target.meanDoseDeviation =  stdDev;


//    this->targetName = targetName;
//    doseMaximum = targetDoseMaximum;
//    doseMinimum = targetDoseMinimum;
//    meanDose = targetMeanDose;
//    meanDoseDeviation =  stdDev;

//    setDVH();

//    if(targetName == "A3"){
//        qDebug()<< "!!";
//    }

//    target.dose50Vol = getDoseAtVolume(50.0);
//    deviation=0;
//    itr = indicesList.begin();
//    for (; itr != indicesList.end(); ++itr)
//    {
//        indx = *itr;
//        doseValue = doseDistribution.at(indx);
//        deviation += (doseValue-target.dose50Vol)*(doseValue-target.dose50Vol);
//    }
//    stdDev = sqrt(deviation/N);
//    target.dose50VolDeviation = stdDev;

//    target.HI = getHomogeneityIndex(target.dose50Vol, "D");


//    if(targetName == "A3"){
//        qDebug()<< "fin";
//    }

}

TargetFieldQuality::~TargetFieldQuality(){
    target.DVH.dose.clear();
    target.DVH.volume.clear();

    target.indicesList.clear();

    if(target.targetName=="A3"){
        qDebug()<<"destruct A3";
    }


//    target.//DVH.volume.clear();

};


TargetFieldQuality& TargetFieldQuality::operator=(const TargetFieldQuality& other){
    /*    struct TargetObj{
 *        std::string targetName;
 *        Target form;
 *        int volume;
 *        std::vector <int> indicesList;
 *        double doseMinimum;
 *        double doseMaximum;
 *        double meanDose;
 *        double meanDoseDeviation;
 *        double dose50Vol;
 *        double dose50VolDeviation;
 *        pairOfVectors DVH;
 *    };
*/

    this->target = other.target;

    //    this->target.targetName = other.target.targetName;
    //    this->target.form = other.target.form;
    //    this->target.volume = other.target.volume;
    //    this->target.indicesList = other.target.indicesList;
    //    this->target.doseMinimum = other.target.doseMinimum;
    //    this->target.doseMaximum = other.target.doseMaximum;
    //    this->target.meanDose = other.target.meanDose;
    //    this->target.meanDoseDeviation = other.target.meanDoseDeviation;

    //    this->target.DVH = other.target.DVH;
    //    this->target.HI = other.target.HI;

    return *this;
}

TargetObj& TargetFieldQuality::getTargetObject(){
    return target;
}

void TargetFieldQuality::setDVH()
{


    qDebug() << targetName;


    target.DVH.dose.clear();
    target.DVH.volume.clear();


    double doseVal;
    //double doseStep = 0.001;

    //int Nsteps = 1000;//(targetDoseMaximum-0) / doseStep;
    double doseStep = 0.01;//(target.doseMaximum-0)/Nsteps;
    int Nsteps =(target.doseMaximum-0)/ doseStep; //1000;//


    target.DVH.dose.resize(Nsteps+1);
    target.DVH.volume.resize(Nsteps+1);

    if (targetName == "A3"){

        qDebug()<<"!!";
    }
    for (int n=0;n<=Nsteps; n++){
        doseVal= 0 + n*doseStep;
        // doseHist[n] = doseVal;
        target.DVH.dose[n] = doseVal;
    }


    std::vector<int>::iterator itr = target.indicesList.begin();
    int indx;
    int topIndex;
    int n;
    for (; itr != target.indicesList.end(); ++itr)
    {
        indx = *itr;
        doseVal = doseDistribution.at(indx);

        topIndex = int(ceil(doseVal*Nsteps/target.doseMaximum));

        for (n = 0; n <= topIndex;n++)
        {
            target.DVH.volume[n]++;
        }
    }

    for (std::vector <double>::iterator  it_VH= target.DVH.volume.begin(); it_VH!=target.DVH.volume.end(); ++it_VH)
    {
        (*it_VH)=100. * (*it_VH)/volume;
    }


}

double TargetFieldQuality::getDoseAtVolume(double volumePercent){


    double doseAtVolume = -999;
    double vol1 , vol2  = 0;
    double dose1, dose2 = 0;

    double tang, shift;


    //std::vector<std::pair<double, double>>::iterator itr = target.DVH.begin();
    std::vector<double>::iterator itr_vol = target.DVH.volume.begin();
    std::vector<double>::iterator itr_dose = target.DVH.dose.begin();

    for (; itr_vol != (target.DVH.volume.end()-1); ++itr_vol, ++itr_dose)
    {
        //        vol1 = (*itr).second;
        //        vol2 = (*(itr+1)).second;
        vol1 = *itr_vol;
        vol2 = *(itr_vol+1);

        if(vol1 == volumePercent){
            //doseAtVolume =  (*itr).first;
            doseAtVolume =  *itr_dose;
        }else
            if (((vol1 < volumePercent)&&(volumePercent < vol2)) ||
                    ((vol2 < volumePercent)&&(volumePercent < vol1))   )
            {
                //                dose1 = (*itr).first;
                //                dose2 = (*(itr+1)).first;
                dose1 = *itr_dose;
                dose2 = *(itr_dose+1);

                tang = (dose1-dose2)/(vol1-vol2);
                shift = dose1 - tang*vol1;

                doseAtVolume = tang*volumePercent+shift;
            }

    }

    return doseAtVolume;
}

double TargetFieldQuality::getHomogeneityIndex(double refIsodose, std::string Formula)  //RTOG
{
    //setTargetDoseRange();
    double HI = 0; // HomogeneityIndex

    if(Formula == "RTOG"){
        HI =  target.doseMaximum/refIsodose;
    }else if(Formula == "A"){
        double D5 = getDoseAtVolume(5.0);
        double D95 = getDoseAtVolume(95.0);

        HI = D5/D95;

    }else if(Formula == "B"){
        double Dmax = target.doseMaximum;
        double Dmin = target.doseMinimum;

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
        double Dmax = target.doseMaximum;

        HI = Dmax/refIsodose;

    }else{
        double D5 = getDoseAtVolume(5.0);
        double D95 = getDoseAtVolume(95.0);

        HI = 100*(D5-D95)/refIsodose;
    }

    return HI;
}
