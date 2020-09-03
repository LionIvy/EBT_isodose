#include "DoseFieldAnalize.h"

double analize::getDoseMinimum(intVector targetIndicesList, DoseVector& doseDistribution){

    //    form.setTargetIndicesList();
    //    intVector indicesList = form.getTargetIndicesList();

    double targetDoseMinimum;
    double doseValue;

    intVector::iterator itr = targetIndicesList.begin();
    targetDoseMinimum = doseDistribution.at(*itr);
    itr++;
    for (; itr != targetIndicesList.end(); ++itr)
    {
        doseValue = doseDistribution.at(*itr);
        if (doseValue < targetDoseMinimum){
            targetDoseMinimum = doseValue;
        }
    }
    return targetDoseMinimum;
}


double analize::getDoseMaximum(intVector targetIndicesList, DoseVector& doseDistribution){

    //    form.setTargetIndicesList();
    //    intVector indicesList = form.getTargetIndicesList();

    double targetDoseMaximum;
    double doseValue;

    intVector::iterator itr = targetIndicesList.begin();
    targetDoseMaximum = doseDistribution.at(*itr);
    itr++;
    for (; itr != targetIndicesList.end(); ++itr)
    {
        doseValue = doseDistribution.at(*itr);
        if (doseValue > targetDoseMaximum){
            targetDoseMaximum = doseValue;
        }
    }
    return targetDoseMaximum;
}


double analize::getMeanDose(intVector targetIndicesList, DoseVector& doseDistribution){

    //    form.setTargetIndicesList();
    //    intVector indicesList = form.getTargetIndicesList();

    double targetIntegrDose =  0;
    double targetMeanDose =  0;
    //double doseValue;

    intVector::iterator itr = targetIndicesList.begin();
    for (; itr != targetIndicesList.end(); ++itr)
    {
        targetIntegrDose += doseDistribution.at(*itr);
    }
    targetMeanDose = targetIntegrDose/targetIndicesList.size();
    return targetMeanDose;
}


double analize::getDoseValDeviation(intVector targetIndicesList, DoseVector& doseDistribution,double refValue){
    double deviation=0;
    double stdDev;

    double doseValue;
    intVector::iterator itr = targetIndicesList.begin();
    for (; itr != targetIndicesList.end(); ++itr)
    {
        doseValue = doseDistribution.at(*itr);
        deviation += (doseValue-refValue)*(doseValue-refValue);
    }
    stdDev = sqrt(deviation/targetIndicesList.size());
    return  stdDev;
}

/// pair <Dose, Volume>
vectorPair analize::getDVH(intVector targetIndicesList, DoseVector& doseDistribution){

    vectorPair DVH;
    //first Dose
    //second Volume

    dblVector doseHist;
    dblVector volumeHist;

    double doseMaximum = getDoseMaximum(targetIndicesList, doseDistribution);

    double doseStep = 0.01;//(target.doseMaximum-0)/Nsteps;
    int Nsteps =(doseMaximum-0)/ doseStep;

    DVH.resize(Nsteps+1);
    doseHist.resize(Nsteps+1);
    volumeHist.resize(Nsteps+1);

    double doseVal;
    for (int n=0;n<=Nsteps; n++){
        doseHist[n] = 0 + n*doseStep;
    }


    intVector::iterator itr = targetIndicesList.begin();
    int topIndex;
    int n;
    for (; itr != targetIndicesList.end(); ++itr)
    {
        doseVal = doseDistribution.at(*itr);
        topIndex = int(ceil(doseVal*Nsteps/doseMaximum));

        for (n = 0; n <= topIndex;n++)
        {
            volumeHist[n]++;
        }
    }


    int targetVolume = targetIndicesList.size();
    for (dblVector::iterator itr_Vol = volumeHist.begin(), itr_Dose = doseHist.begin();itr_Vol != volumeHist.end(); ++itr_Vol, ++itr_Dose)
    {
        (*itr_Vol) = 100. * (*itr_Vol) / targetVolume;
    }

    std::pair<double,double> test;
    for(unsigned int i=0; i<doseHist.size();i++){
        test = std::make_pair(doseHist[i],volumeHist[i]);
        DVH.push_back(test);
    }

    return DVH;
}

void analize::getDVH(intVector targetIndicesList, DoseVector& doseDistribution, dblVector &doseHist, dblVector &volumeHist){

    vectorPair DVH;
    //first Dose
    //second Volume

//    dblVector doseHist;
//    dblVector volumeHist;

    double doseMaximum = getDoseMaximum(targetIndicesList, doseDistribution);

    double doseStep = 0.01;//(target.doseMaximum-0)/Nsteps;
    int Nsteps =(doseMaximum-0)/ doseStep;

    DVH.resize(Nsteps+1);
    doseHist.resize(Nsteps+1);
    volumeHist.resize(Nsteps+1);

    double doseVal;
    for (int n=0;n<=Nsteps; n++){
        doseHist[n] = 0 + n*doseStep;
    }


    intVector::iterator itr = targetIndicesList.begin();
    int topIndex;
    int n;
    for (; itr != targetIndicesList.end(); ++itr)
    {
        doseVal = doseDistribution.at(*itr);
        topIndex = int(ceil(doseVal*Nsteps/doseMaximum));

        for (n = 0; n <= topIndex;n++)
        {
            volumeHist[n]++;
        }
    }


    int targetVolume = targetIndicesList.size();
    for (dblVector::iterator itr_Vol = volumeHist.begin(), itr_Dose = doseHist.begin();itr_Vol != volumeHist.end(); ++itr_Vol, ++itr_Dose)
    {
        (*itr_Vol) = 100. * (*itr_Vol) / targetVolume;
    }

//    std::pair<double,double> test;
//    for(unsigned int i=0; i<doseHist.size();i++){
//        test = std::make_pair(doseHist[i],volumeHist[i]);
//        DVH.push_back(test);
//    }


}

double analize::getDoseAtVolume(double volumePercent, dblVector &doseHist, dblVector &volumeHist){

    double doseAtVolume = -999;
    double vol1 , vol2  = 0;
    double dose1, dose2 = 0;

    double tang, shift;

    dblVector::iterator itr_dose = doseHist.begin();
    dblVector::iterator itr_vol = volumeHist.begin();
    for (; itr_dose != (doseHist.end()-1); ++itr_dose, ++itr_vol)
    {
        vol1 = *itr_vol;
        vol2 = *(itr_vol+1);

        if(vol1 == volumePercent){
            doseAtVolume =  *itr_dose;
        }else
            if (((vol1 < volumePercent)&&(volumePercent < vol2)) ||
                    ((vol2 < volumePercent)&&(volumePercent < vol1))   )
            {
                dose1 = *itr_dose;
                dose2 = *(itr_dose+1);

                tang = (dose1-dose2)/(vol1-vol2);
                shift = dose1 - tang*vol1;

                doseAtVolume = tang*volumePercent+shift;
            }
    }

    return doseAtVolume;
}

double analize::getDoseAtVolume(double volumePercent, vectorPair DVH){

    double doseAtVolume = -999;
    double vol1 , vol2  = 0;
    double dose1, dose2 = 0;

    double tang, shift;

    vectorPair::iterator itr = DVH.begin();
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

double analize::getDoseAtVolume(intVector targetIndicesList, DoseVector& doseDistribution,double volumePercent){

    double doseAtVolume = -999;
    double vol1 , vol2  = 0;
    double dose1, dose2 = 0;

    double tang, shift;
    dblVector doseHist, volHist;
    //vectorPair DVH = getDVH(targetIndicesList, doseDistribution);
    getDVH(targetIndicesList, doseDistribution, doseHist, volHist);

        dblVector::iterator itr_dose = doseHist.begin();
        dblVector::iterator itr_vol = volHist.begin();
        for (; itr_dose != (doseHist.end()-1); ++itr_dose, ++itr_vol)
        {
            vol1 = *itr_vol;
            vol2 = *(itr_vol+1);

            if(vol1 == volumePercent){
                doseAtVolume =  *itr_dose;
            }else
                if (((vol1 < volumePercent)&&(volumePercent < vol2)) ||
                        ((vol2 < volumePercent)&&(volumePercent < vol1))   )
                {
                    dose1 = *itr_dose;
                    dose2 = *(itr_dose+1);

                    tang = (dose1-dose2)/(vol1-vol2);
                    shift = dose1 - tang*vol1;

                    doseAtVolume = tang*volumePercent+shift;
                }
        }

//    vectorPair::iterator itr = DVH.begin();
//    for (; itr != (DVH.end()-1); ++itr)
//    {
//        vol1 = (*itr).second;
//        vol2 = (*(itr+1)).second;

//        if(vol1 == volumePercent){
//            doseAtVolume =  (*itr).first;
//        }else
//            if (((vol1 < volumePercent)&&(volumePercent < vol2)) ||
//                    ((vol2 < volumePercent)&&(volumePercent < vol1))   )
//            {
//                dose1 = (*itr).first;
//                dose2 = (*(itr+1)).first;

//                tang = (dose1-dose2)/(vol1-vol2);
//                shift = dose1 - tang*vol1;

//                doseAtVolume = tang*volumePercent+shift;
//            }
//    }

    return doseAtVolume;
}

double analize::getHomogeneityIndex(intVector targetIndicesList, DoseVector& doseDistribution, double refIsodose, std::string Formula){



    double HI = 0; // HomogeneityIndex

    dblVector doseHist, volHist;
    //vectorPair DVH = getDVH(targetIndicesList, doseDistribution);
    getDVH(targetIndicesList, doseDistribution, doseHist, volHist);

    if(Formula == "RTOG"){
        double doseMaximum = getDoseMaximum(targetIndicesList, doseDistribution);
        HI =  doseMaximum/refIsodose;
    }else if(Formula == "A"){
        double D5  = getDoseAtVolume( 5.0, doseHist, volHist);
        double D95 = getDoseAtVolume(95.0, doseHist, volHist);

        HI = D5/D95;

    }else if(Formula == "B"){
        double Dmax = getDoseMaximum(targetIndicesList, doseDistribution);
        double Dmin = getDoseMinimum(targetIndicesList, doseDistribution);;

        HI = Dmax/Dmin;

    }else if(Formula == "C"){
        double D1  = getDoseAtVolume( 1.0, doseHist, volHist);
        double D98 = getDoseAtVolume(98.0, doseHist, volHist);

        HI = 100*(D1-D98)/refIsodose;

    }else if(Formula == "D"){
        double D5  = getDoseAtVolume( 5.0, doseHist, volHist);
        double D95 = getDoseAtVolume(95.0, doseHist, volHist);

        HI = 100*(D5-D95)/refIsodose;

    }else if(Formula == "E"){
        double Dmax = getDoseMaximum(targetIndicesList, doseDistribution);
        HI = Dmax/refIsodose;

    }else{
        double D5  = getDoseAtVolume( 5.0, doseHist, volHist);
        double D95 = getDoseAtVolume(95.0, doseHist, volHist);

        HI = 100*(D5-D95)/refIsodose;
    }

    return HI;

}
