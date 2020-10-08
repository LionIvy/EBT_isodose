#include "test.h"

test::test()
{

}
/*

    newCurve.clear();

    //Сканирование по изодозам
    for(int lvl=0; lvl<N_lvls;lvl++)
    {

        isoCurve.clear();
        //Поиск внешних контуров
        //newDoseLine = true;
        PenStyle.setColor(colorList[lvl]);
        isoCurve = MooreTracing(planeMtrx,fieldWidth,fieldHeight,isodoses[lvl]);

        if(isoCurve.traceNewObj(!traceCavities)){

            curveX.clear();
            curveY.clear();
            xData.clear();
            yData.clear();
            tData.clear();

            curveX = isoCurve.getNewTraceX();
            curveY = isoCurve.getNewTraceY();
            curveLength=curveX.size();

            if(curveX.empty()) continue;
            if(clearObjLessThen > curveLength) continue;

            newCurve.push_back(new QCPCurve(printArea->xAxis, printArea->yAxis));

            xData.resize(curveLength);
            yData.resize(curveLength);
            tData.resize(curveLength);

            for (int i=0; i<curveLength; i++)
            {
                xData[i] = curveX[i];
                yData[i] = curveY[i];
                tData[i] = i;
            }
            //======Цветной уровень==========================
            curveID++;
            newCurve[curveID]->setData(tData, xData, yData);
            newCurve[curveID]->setLineStyle(QCPCurve::LineStyle::lsLine);
            newCurve[curveID]->setPen(PenStyle);

            //if( newDoseLine){
            //    newDoseLine = false;
                doseLineName = QString::number(isodoses[lvl]);
                newCurve[curveID] -> setName(doseLineName);
            //}else{
            //    newCurve[curveID] -> removeFromLegend();
            //}

            //======Ч/Б уровень==========================
            curveID++;
            newCurve.push_back(new QCPCurve(printArea->xAxis, printArea->yAxis));
            newCurve[curveID]->setData(tData, xData, yData);
            newCurve[curveID]->setLineStyle(QCPCurve::LineStyle::lsLine);
            newCurve[curveID]->setPen(BlackPen);
            newCurve[curveID]->removeFromLegend();
        }

        while(isoCurve.traceNewObj(!traceCavities))
        {
            curveX.clear();
            curveY.clear();
            xData.clear();
            yData.clear();
            tData.clear();

            curveX = isoCurve.getNewTraceX();
            curveY = isoCurve.getNewTraceY();
            curveLength=curveX.size();

            if(curveX.empty()) continue;
            if(clearObjLessThen > curveLength) continue;

            newCurve.push_back(new QCPCurve(printArea->xAxis, printArea->yAxis));

            xData.resize(curveLength);
            yData.resize(curveLength);
            tData.resize(curveLength);

            for (int i=0; i<curveLength; i++)
            {
                xData[i] = curveX[i];
                yData[i] = curveY[i];
                tData[i] = i;
            }
//            //======Цветной уровень==========================
//            curveID++;
//            newCurve[curveID]->setData(tData, xData, yData);
//            newCurve[curveID]->setLineStyle(QCPCurve::LineStyle::lsLine);
//            newCurve[curveID]->setPen(PenStyle);

//            if( newDoseLine){
//                newDoseLine = false;
//                doseLineName = QString::number(isodoses[lvl]);
//                newCurve[curveID] -> setName(doseLineName);
//            }else{
//                newCurve[curveID] -> removeFromLegend();
//            }

            //======Ч/Б уровень==========================
            curveID++;
            newCurve.push_back(new QCPCurve(printArea->xAxis, printArea->yAxis));
            newCurve[curveID]->setData(tData, xData, yData);
            newCurve[curveID]->setLineStyle(QCPCurve::LineStyle::lsLine);
            newCurve[curveID]->setPen(BlackPen);
            newCurve[curveID]->removeFromLegend();
        }
        printArea->replot();


    }


*/
