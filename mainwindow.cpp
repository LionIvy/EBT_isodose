#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QMessageBox"

#include <tiffio.h>

#include "mooretracing.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //==================================================
    ///=============Настройка директорий================
    //==================================================
    ///
    {
        QString dirname = QDir::currentPath();
        rootFolder = dirname;

        QString shortcutFolder = dirname + "/shortcuts/";
        QString ebtFolder = dirname;
        QString imageFolder = dirname;


        ui -> statusbar -> showMessage(dirname);
        ui -> statusbar -> update();
    }

    //==================================================
    ///======Настройка взаимодействий с графиками=======
    //==================================================
    ///
    {

        ui -> axis4isodoses->axisRect()->setAutoMargins(QCP::msLeft | QCP::msTop | QCP::msBottom);
        ui -> axis4isodoses->axisRect()->setMargins(QMargins(0,0,150,0));
        ui -> axis4isodoses->axisRect()->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);
        ui -> axis4isodoses->axisRect()->insetLayout()->setInsetRect(0, QRectF(1.04,-0.025,0.1,0.1));

        ui -> axis4isodoses-> yAxis -> setRangeReversed(true);
        ui->axis4isodoses->axisRect()->setBackground(Qt::gray);
        bool ticksOn  = true;
        ui -> axis4isodoses -> xAxis ->setTickLabels(ticksOn);
        ui -> axis4isodoses -> yAxis ->setTickLabels(ticksOn);
        ui->axis4isodoses->replot();

        //ui -> axis4isodoses-> setInteractions(QCP::iSelectPlottables);
        ui -> axis4isodoses-> setInteractions(QCP::iRangeDrag | QCP::iRangeZoom );

        /* main
     * image
     * customGrid
     * probesArea
     * isodoses
     */
        ui -> axis4isodoses -> addLayer("image"      ,ui->axis4isodoses->layer("main")      , QCustomPlot::limAbove);
        ui -> axis4isodoses -> addLayer("customGrid" ,ui->axis4isodoses->layer("image")     , QCustomPlot::limAbove);
        ui -> axis4isodoses -> addLayer("probesArea" ,ui->axis4isodoses->layer("customGrid"), QCustomPlot::limAbove);
        ui -> axis4isodoses -> addLayer("isodoses"   ,ui->axis4isodoses->layer("probesArea"), QCustomPlot::limAbove);
        ui -> axis4isodoses -> addLayer("CMAP"       ,ui->axis4isodoses->layer("isodoses")  , QCustomPlot::limAbove);

        connect(ui->axis4isodoses, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(showCursor(QMouseEvent*)));
        //QMouseEvent* w;
       // mouseDoubleClickEvent()
        //mouseReleaseEvent()
    }




    //==================================================
    ///====Ограничения на значения вводимых значений====
    //==================================================
    ///
    {
        setLineValidators();
    }

    //==================================================
    ///===========Задание начальных значений============
    //==================================================
    ///
    {
        doseVectorIsSet = false;
        isodosesAreSet = false;

    }

    //==================================================
    ///================Коррекция цвета==================
    //==================================================
    ///
    {
        QString setBackgroundColor;
        setBackgroundColor="background-color: rgb( 255, 255, 255);";
        ui->filename_lbl->setAutoFillBackground(true);
        ui->filename_lbl->setStyleSheet(setBackgroundColor);

    }

    //==================================================
    ///===============Настройка кнопок==================
    //==================================================
    ///
    {
        QObject::connect(ui->loadDoseDistribution_btn, SIGNAL(clicked()),
                         this                        , SLOT  (loadEBTfile()));

        QObject::connect(ui->isodoseLines_edit       , SIGNAL(editingFinished()),
                         this                        , SLOT  (setIsodoseLines()));


        QObject::connect(ui->buildIsodose_btn        , SIGNAL(clicked()),
                         this                        , SLOT  (pushIsodoseButton()));

        QObject::connect(ui->buildIsoCMAP_btn        , SIGNAL(clicked()),
                         this                        , SLOT  (buildIsoCMAP()));

        QObject::connect(ui->gridOn_ChB              , SIGNAL(stateChanged(int)),
                         this                        , SLOT  (changeGridChBox(int)));



        QObject::connect(ui->gridStepX_edit          , SIGNAL(editingFinished()),
                         this                        , SLOT  (updateGrid()));
        QObject::connect(ui->gridStepX_edit          , SIGNAL(textEdited(QString)),
                         this                        , SLOT  (copyGridXStep2YStep(QString)));

        QObject::connect(ui->gridStepY_edit          , SIGNAL(editingFinished()),
                         this                        , SLOT  (updateGrid()));
        QObject::connect(ui->gridStartX_edit         , SIGNAL(editingFinished()),
                         this                        , SLOT  (updateGrid()));
        QObject::connect(ui->gridStartY_edit         , SIGNAL(editingFinished()),
                         this                        , SLOT  (updateGrid()));
        QObject::connect(ui->gridAngle_edit          , SIGNAL(editingFinished()),
                         this                        , SLOT  (updateGrid()));



    }


}

MainWindow::~MainWindow()
{
    delete ui;
}

//==================================================
///===========Настройки окна приложения=============
//==================================================
//

void MainWindow::setLineValidators()
{
    //==================================================
    ///====Ограничения на значения вводимых значений====
    //==================================================
    ///
    QDoubleValidator* validator = new QDoubleValidator(0, fieldWidth , 2, this);
    validator->setNotation(QDoubleValidator::StandardNotation);
    validator->setLocale(QLocale::English);


    ui->gridStepX_edit  -> setValidator(validator);
    ui->gridStepY_edit  -> setValidator(validator);
    ui->gridStartX_edit -> setValidator(validator);
    ui->gridStartY_edit -> setValidator(validator);
    ui->gridAngle_edit -> setValidator(new QDoubleValidator());


    //ui->MaxVal_pointX-> setValidator(new QIntValidator(0,sizeX-1,this));
}

void MainWindow::pushIsodoseButton(){

    QString currentText = ui->buildIsodose_btn->text();

    if ((currentText == "Hide graph")){
        ui -> axis4isodoses -> setCurrentLayer("isodoses");
        ui -> axis4isodoses -> clearPlottables();
        ui -> axis4isodoses -> clearItems();
        ui -> axis4isodoses -> clearGraphs();

        ui->axis4isodoses->replot();

        //dosePrintLock = true;
        ui -> buildIsodose_btn -> setText("Print graph");

    }else if(currentText == "Print graph"){


        //dosePrintLock = false;
        //ui -> buildIsodose_btn -> setText("Hide graph");
        buildIsodoses();

    }


}

void MainWindow::copyGridXStep2YStep(QString txt){

    ui->gridStepY_edit->setText(txt);

}

//==================================================
///===============Работа с файлами==================
//==================================================

void MainWindow::loadEBTfile()
{
    QString locstr = ebtFolder;
    QString filter = "*.tif *.tiff";
    QString filename =   QFileDialog::getOpenFileName(0, "Set dose distribution", locstr, filter);//, 0, QFileDialog::DontUseNativeDialog) ;
    //QString filename =  "/home/ivan/work/EBT_isodose/B01.tif";

    if (filename == "")
        return;

    QChar* charArr = filename.data();
    int stringSize = filename.size()-1;
    int num= filename.size()-1;

    while(charArr[num] != '/')
    {   num--;
        if( num<0)
        {
            qDebug() << "Error while loading\n";
            return;
        }
    }

    num++;
    QChar charName[stringSize-num+2];
    for (int i=num;i<=stringSize;++i)
    {
        charName[i-num]= charArr[i];
    }

    QChar charfolderName[num+1];
    for (int i=0;i<num;i++)
    {
        charfolderName[i]= charArr[i];
    }

    loadTIFF(filename);

    QString file(charName);
    ui -> filename_lbl -> setText(file);

    setLineValidators();

    buildIsodoses();

}
void MainWindow::loadTIFF(QString filename)
{
    TIFF* tif = TIFFOpen(filename.toStdString().c_str(), "r");
    if (tif) {
        uint32 imageWidth, imageHeight;
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &imageWidth);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &imageHeight);

        fieldWidth = imageWidth;
        fieldHeight = imageHeight;

        //size_t npixels = imageWidth*imageHeight;
        tdata_t buf;
        uint32 row;
        uint32 config;

        doseVector.clear();
        doseVectorIsSet = false;

        double val;

        TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
        //uint32 lineSize = TIFFScanlineSize(tif);
        buf = _TIFFmalloc(TIFFScanlineSize(tif));

        uint16 sample, nsamples;
        uint16* data;


        TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &nsamples);
        for (sample = 0; sample < nsamples; sample++){
            for (row = 0; row < imageHeight; row++){

                TIFFReadScanline(tif, buf, row, sample);
                data = (uint16*)buf;

                for (uint32 i = 0; i < imageWidth; i++){
                    //doseVector.push_back(data[i]); mGy
                    val = 0.001* data[i];//Gy
                    doseVector.push_back(val);
                }
            }
        }
        _TIFFfree(buf);
        TIFFClose(tif);
        doseVectorIsSet = true;
    }
}

//==================================================
///===============Задание изодоз==================
//==================================================

void MainWindow::setIsodoseLines(){
    isodoses.clear();
    isodosesAreSet = false;

    //ui->isodoseLines_edit
    //ui->isod
    QString isodoseLines_edit = ui->isodoseLines_edit->text();
    int stringSize = isodoseLines_edit.size();

    int stPoint = 0;
    int finPoint = 0;

    while (stPoint < stringSize){
        search4Value(isodoseLines_edit, stPoint, finPoint, stringSize);
        //qDebug() << isodoses.back();
    }

    // Сортировка
    int vSize = isodoses.size();
    if (vSize > 1){
        double buff=0;bool notSorted;
        do{
            notSorted = false;
            /*
 *             for(int lvl=0;lvl<vSize-1;lvl++)
 *            {
 *                if(isodoses[lvl+1]>isodoses[lvl])
 *                {
 *                    buff=isodoses[lvl];
 *                    isodoses[lvl]=isodoses[lvl+1];
 *                    isodoses[lvl+1]=buff;
 *                    notSorted=true;
 *                }
 *             }
*/
            for (std::vector<double>::iterator it = isodoses.begin() ; it != (isodoses.end()-1); ++it){
                if( *(it+1) > *it)
                {
                    buff=*it;
                    *it=*(it+1);
                    *(it+1)=buff;
                    notSorted=true;
                }
            }
        }while(notSorted);
    }

    //isodoses.push_back(0);

    // Устранение дубликатов
    for (std::vector<double>::iterator it = isodoses.begin() ; it != (isodoses.end()-1); ++it){
        if (*(it+1) == *it){
            isodoses.erase(it);
        }
    }

    /*
 *    qDebug() << "================";
 *    qDebug() << "isodoses are:";
 *    for (unsigned int i = 0; i < isodoses.size(); i++){
 *        qDebug() << isodoses[i];
 *    }
 *    qDebug() << "================";
*/

    isodosesAreSet = true;

    buildIsodoses();
}

void MainWindow::search4Value(QString strLines, int &startPoint, int &finPoint, const int stringSize){
    //6, 7 , 8 9, 10, 11  12

    //"6, 7 , 8 9, 10, 11  12  17  "
    //std::vector <double> values;
    //QString strVal;
    //double value = -999;
    bool isFound = false;
    int checkPoint = startPoint;
    while (!isFound & (checkPoint < stringSize))
    {
        if      ((strLines[checkPoint] == ' ') || (strLines[checkPoint] == "/t"))
        {
            if ((checkPoint-1) >= 0){
                if((strLines[checkPoint-1] == ',') || (strLines[checkPoint-1] == ';'))
                {
                    startPoint++;
                }else{
                    isFound = true;
                    finPoint = checkPoint;
                    addToIsodoseList(strLines, startPoint, finPoint);
                }
            }
        }else if((strLines[checkPoint] == ',') || (strLines[checkPoint] == ';') )
        {
            isFound = true;
            finPoint = checkPoint;
            addToIsodoseList(strLines, startPoint, finPoint);
        }else if( strLines[checkPoint] == ':')
        {
            //qDebug() << ":";
            isFound = true;
            addRangeToIsodoseList(strLines, startPoint,finPoint, stringSize);
        }
        checkPoint++;
        if(checkPoint == stringSize)
        {
            isFound = true;
            finPoint = checkPoint;
            addToIsodoseList(strLines, startPoint, finPoint);
        }

    }
    startPoint = finPoint+1;
    finPoint = finPoint+1;
}

bool MainWindow::addToIsodoseList(QString strLines, int startPoint,int finPoint)
{
    QString strVal;

    for (int i=startPoint; i < finPoint; i++)
    {
        if ( (strLines[i] == ' ') || (strLines[i] == "/t"))
        {
            qDebug() << "warning";
        }else if( strLines[i] == "."){
            strVal.push_back(strLines[i]);
            //               ((strLines[i].isLetter() & (strLines[i] != ".")) || (strLines[i] == "-") || (strLines[i] == "+") || (!strLines[i].isLetterOrNumber()))
        }else if( strLines[i].isLetter()  || (strLines[i] == "-") || (strLines[i] == "+") || (!strLines[i].isLetterOrNumber()))
        {
            //qDebug() << "warning";
            QMessageBox::critical(0,"Warning","Signs and letters are not allowed!");
            return false;
        }
        else if((strLines[i] != ',') & (strLines[i] != ';'))
        {
            strVal.push_back(strLines[i]);
        }
        //strVal.push_back()
    }

    double value = -999;
    bool isNumber;
    if (strVal.size() != 0){
        value = strVal.toDouble(&isNumber);
        if(!isNumber){
            QMessageBox::critical(0,"Warning","Could not convert string to number");
            return false;
        }
        isodoses.push_back(value);
        return true;
    }
    return false;
}

bool MainWindow::addRangeToIsodoseList(QString strLines, int startPoint,int &finPoint, const int stringSize)
{
    int colonNum = 0;
    int checkPoint = startPoint;
    int nPoint = 0;

    bool isFound = false;
    bool numberIsSet = false;
    bool isNumber;

    std::vector<int> checkPointList;

    while (!isFound)
    {
        if(strLines[checkPoint].isNumber()){
            numberIsSet = true;
        }else if(strLines[checkPoint] == ':')
        {
            colonNum++;
            checkPointList.push_back(checkPoint);
            numberIsSet = false;
        }else if((strLines[checkPoint] == ' ') || (strLines[checkPoint] == "/t"))
        {
            if (colonNum==0)        //Можно не рассматривать
            {
                /* " 2:3:9 15","  2:3:9 15", "2 :3:9 15"
             if(){

             }else if(){

             }else{
                 QMessageBox::critical(0, "Warning","unknown case!");
                 return;
             }*/
            }else if (colonNum==1)
            {
                /*"2: 3:9"  skip
                  "2:  3:9" skip
                  "2:3 :9"  skip */

                if (numberIsSet){
                    //"2:9 :15"  точка остановки считывания числа, но нужен поиск след :;
                    //"2:9 15"   полный выход isFound = true;
                    nPoint = checkPoint;
                    do {
                        nPoint++;
                        if (strLines[nPoint] != ':'){
                            isFound = true;
                            finPoint = checkPoint;
                        }else{
                            checkPoint = nPoint;

                            colonNum++;
                            checkPointList.push_back(checkPoint);
                        }
                    } while (( strLines[nPoint] == ' ') || ( strLines[nPoint] == "/t"));
                    numberIsSet = false;
                }
            }else if (colonNum==2)
            {
                // "2:3: 9" продолжение поиска
                if (numberIsSet){ //"2:3:9 15" выход из поиска

                    isFound = true;
                    finPoint = checkPoint;
                    numberIsSet = false;
                }
            }else{
                QMessageBox::critical(0,"Warning","To many colons! unknown sequence");
                return false;
            }
        }else if((strLines[checkPoint] == ',') || (strLines[checkPoint] == ';') )
        {
            isFound = true;
            finPoint = checkPoint;
            numberIsSet = false;
        }else if(strLines[checkPoint].isLetter() & (strLines[checkPoint] != ".")) {
            QMessageBox::critical(0,"Warning","Letters are not allowed!");
            return false;

        }

        checkPoint++;
        if(checkPoint == stringSize)
        {
            isFound = true;
            finPoint = checkPoint;

        }
    }

    QString strVal;
    double val_1 = -999, val_2 = -999, val_3 = -999;

    if (colonNum==1){

        strVal.clear();
        for (int i = startPoint; i < checkPointList[0]; i++)
        {
            strVal.push_back(strLines[i]);
        }
        val_1 = strVal.toDouble(&isNumber);
        if(!isNumber){
            QMessageBox::critical(0,"Warning","Could not convert string to number");
            return false;
        }

        strVal.clear();
        for (int i = checkPointList[0]+1; i < finPoint; i++)
        {
            strVal.push_back(strLines[i]);
        }
        val_2 = strVal.toDouble(&isNumber);
        if(!isNumber){
            QMessageBox::critical(0,"Warning","Could not convert string to number");
            return false;
        }

        if (val_1 > val_2){
            for(double value = val_2; value <= val_1; value++){
                isodoses.push_back(value);
            }
        }else if (val_2 >= val_1){
            for(double value = val_1; value <= val_2; value++){
                isodoses.push_back(value);
            }
        }

    }else if (colonNum==2){
        strVal.clear();
        for (int i = startPoint; i < checkPointList[0]; i++)
        {
            strVal.push_back(strLines[i]);
        }
        val_1 = strVal.toDouble(&isNumber);
        if(!isNumber){
            QMessageBox::critical(0,"Warning","Could not convert string to number");
            return false;
        }

        strVal.clear();
        for (int i = checkPointList[0]+1; i < checkPointList[1]; i++)
        {
            strVal.push_back(strLines[i]);
        }
        val_2 = strVal.toDouble(&isNumber);
        if(!isNumber){
            QMessageBox::critical(0,"Warning","Could not convert string to number");
            return false;
        }

        strVal.clear();
        for (int i = checkPointList[1]+1; i < finPoint; i++)
        {
            strVal.push_back(strLines[i]);
        }
        val_3 = strVal.toDouble(&isNumber);
        if(!isNumber){
            QMessageBox::critical(0,"Warning","Could not convert string to number");
            return false;
        }

        if (val_1 > val_3){ //"6:-2:1"
            if(val_2>=0)    //"6: 2:1"
            {
                QMessageBox::critical(0,"Warning","Wrong range sequence!");
                return false;
            }
            for(double value = val_1; value >= val_3; value+=val_2){
                isodoses.push_back(value);
            }
        }else if (val_3 >= val_1){ //"3: 2:6"
            if(val_2<=0){   //"3:-2:6"
                QMessageBox::critical(0,"Warning","Wrong range sequence!");
                return false;
            }
            for(double value = val_1; value <= val_3; value+=val_2){
                isodoses.push_back(value);
            }
        }

    }else{
        QMessageBox::critical(0,"Warning","To many colons! unknown sequence");
        return false;
    }

    return true;
}

void MainWindow::setLinesColorStyle(){

    if (!isodosesAreSet) return;

    QCPColorGradient jetCM;
    jetCM.loadPreset(QCPColorGradient::gpJet);
    jetCM.setLevelCount(1000);

    colorList.clear();

    QString setBackgroundColor;

    double value;

    double doseMin = isodoses.front();
    double doseMax = isodoses.back();
    double temp = doseMax;
    doseMin > doseMax ? doseMax = doseMin , doseMin = temp : false;


    doseMin = 0;
    for (std::vector<double>::iterator it = isodoses.begin() ; it != isodoses.end(); ++it){
        value = 0.9*(*it - doseMin)/doseMax;
        colorList.push_back(jetCM.color(value,QCPRange(0, 1), false));
    }
    colorList.push_back(jetCM.color(0,QCPRange(0, 1), false));

}

void MainWindow::buildIsodoses()
{
    if (!(isodosesAreSet & doseVectorIsSet)) return;

    ui -> axis4isodoses -> setCurrentLayer("isodoses");

    //ui->axis4isodoses-
    ui -> axis4isodoses -> clearPlottables();
    ui -> axis4isodoses -> clearItems();
    ui -> axis4isodoses -> clearGraphs();
    ui-> axis4isodoses -> legend->clear();


    //if(dosePrintLock) return;

    setLinesColorStyle();

    // Общий стиль линий
    QPen PenStyle;
    PenStyle.setWidth(3);

    QVector<double> xData, yData, tData;

    int curveLength;
    int clearObjLessThen = 2;

    bool traceCavities =  true;//false;//

    // Вектор всех кривых на графике
    QVector<QCPCurve*> newCurve;
    // Считчик контуров
    int curveID = -1;


    //==============================================================
    ///        Настройка легенды
    //==============================================================
    ///
    ui -> axis4isodoses -> legend ->clearItems();
    //ui -> axis4isodoses -> legend -> setInteraction(QCP::iRangeDrag, true);
    ui -> axis4isodoses -> setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    ui -> axis4isodoses -> legend->setVisible(false);
    bool newDoseLine = true;
    QString doseLineName;

    //==============================================================
    ///        Задание матрицы текущего среза
    //==============================================================
    ///
    // Инициализация
    std::vector<std::vector<double>> planeMtrx (fieldWidth, std::vector<double>(fieldHeight, 0.0));

    // Присвоение элементов среза
    for (int x=0; x<fieldWidth; x++)
    {
        for (int y=0; y<fieldHeight; y++)
        {
            planeMtrx[x][y]=doseVector[doseIndex(x,y)];
        }
    }

    int ctrlDose = 5;
       printDoseFieldInFile(ctrlDose, planeMtrx, "Cut_B04_5Gy.tgt");

    //==============================================================
    ///        Поиск изодозных кривых
    //==============================================================
    ///

    int N_lvls = isodoses.size();

    //Инициализация поиска
    QVector<MooreTracing> isoCurve(N_lvls+1);

    bool reverseSearchingOn;// = false;
    bool plusReverse = true;//false;

    //Сканирование по изодозам
    for(int lvl=0; lvl<N_lvls;lvl++)
    {
        newDoseLine = true;
        isoCurve[lvl] = MooreTracing(planeMtrx,fieldWidth,fieldHeight,isodoses[lvl]);

        PenStyle.setColor(colorList[lvl]);
        //Поиск всех контуров
        while(isoCurve[lvl].traceNewObj(!traceCavities))
        {
            curveX.clear();
            curveY.clear();
            xData.clear();
            yData.clear();
            tData.clear();

            curveX = isoCurve[lvl].getNewTraceX();
            curveY = isoCurve[lvl].getNewTraceY();
            curveLength=curveX.size();

            if(curveX.empty()) continue;
            if(clearObjLessThen > curveLength) continue;

            curveID++;
            //ContourCounter++;
            //qDebug() << newCurve.size() <<'\n';

            newCurve.push_back(new QCPCurve(ui -> axis4isodoses ->xAxis, ui -> axis4isodoses ->yAxis));

            xData.resize(curveLength);
            yData.resize(curveLength);
            tData.resize(curveLength);

            for (int i=0; i<curveLength; i++)
            {
                xData[i] = curveX[i];
                yData[i] = curveY[i];
                tData[i] = i;
            }

            newCurve[curveID]->setData(tData, xData, yData);
            newCurve[curveID]->setLineStyle(QCPCurve::LineStyle::lsLine);
            newCurve[curveID]->setPen(PenStyle);

            if( newDoseLine){
                newDoseLine = false;
                doseLineName = QString::number(isodoses[lvl]);
                newCurve[curveID] -> setName(doseLineName);
            }else{
                //                doseLineName = QString::number(isodoses[lvl]) + " Gy";
                //                newCurve[curveID] -> setName(doseLineName);
                newCurve[curveID] -> removeFromLegend();
            }
            ui->axis4isodoses->replot();
        }

        if (plusReverse){
            reverseSearchingOn = true;
            isoCurve[lvl] = MooreTracing(planeMtrx,fieldWidth,fieldHeight,isodoses[lvl],reverseSearchingOn);
            PenStyle.setColor(colorList[lvl+1]);
            while(isoCurve[lvl].traceNewObj(!traceCavities))
            {
                curveX.clear();
                curveY.clear();
                xData.clear();
                yData.clear();
                tData.clear();

                curveX = isoCurve[lvl].getNewTraceX();
                curveY = isoCurve[lvl].getNewTraceY();
                curveLength=curveX.size();

                if(curveX.empty()) continue;
                if(clearObjLessThen > curveLength) continue;

                curveID++;
                //ContourCounter++;
                //qDebug() << newCurve.size() <<'\n';

                newCurve.push_back(new QCPCurve(ui -> axis4isodoses ->xAxis, ui -> axis4isodoses ->yAxis));

                xData.resize(curveLength);
                yData.resize(curveLength);
                tData.resize(curveLength);

                for (int i=0; i<curveLength; i++)
                {
                    xData[i] = curveX[i];
                    yData[i] = curveY[i];
                    tData[i] = i;
                }

                newCurve[curveID]->setData(tData, xData, yData);
                newCurve[curveID]->setLineStyle(QCPCurve::LineStyle::lsLine);
                newCurve[curveID]->setPen(PenStyle);

                if( newDoseLine){
                    newDoseLine = false;
                    doseLineName = QString::number(isodoses[lvl]);
                    newCurve[curveID] -> setName(doseLineName);
                }else{
                    //                doseLineName = QString::number(isodoses[lvl]) + " Gy";
                    //                newCurve[curveID] -> setName(doseLineName);
                    newCurve[curveID] -> removeFromLegend();
                }
                ui->axis4isodoses->replot();
            }
        }

    }

    // Поиск границ изодоз не вошедших в нижний предел
    reverseSearchingOn = true;
    newDoseLine = true;
    isoCurve[N_lvls] = MooreTracing(planeMtrx,fieldWidth,fieldHeight,isodoses[N_lvls-1],reverseSearchingOn);

    PenStyle.setColor(colorList[N_lvls]);
    //Поиск всех контуров
    while(isoCurve[N_lvls].traceNewObj(true))
    {
        curveX.clear();
        curveY.clear();
        xData.clear();
        yData.clear();
        tData.clear();

        curveX = isoCurve[N_lvls].getNewTraceX();
        curveY = isoCurve[N_lvls].getNewTraceY();
        curveLength=curveX.size();

        if(curveX.empty()) continue;
        if(clearObjLessThen > curveLength) continue;

        curveID++;

        newCurve.push_back(new QCPCurve(ui -> axis4isodoses ->xAxis, ui -> axis4isodoses ->yAxis));

        xData.resize(curveLength);
        yData.resize(curveLength);
        tData.resize(curveLength);

        for (int i=0; i<curveLength; i++)
        {
            xData[i] = curveX[i];
            yData[i] = curveY[i];
            tData[i] = i;
        }

        newCurve[curveID]->setData(tData, xData, yData);
        newCurve[curveID]->setLineStyle(QCPCurve::LineStyle::lsLine);
        newCurve[curveID]->setPen(PenStyle);

        if( newDoseLine){
            newDoseLine = false;
            doseLineName = "< " + QString::number(isodoses[N_lvls-1]);
            newCurve[curveID] -> setName(doseLineName);
        }else{
            //                doseLineName = QString::number(isodoses[lvl]) + " Gy";
            //                newCurve[curveID] -> setName(doseLineName);
            newCurve[curveID] -> removeFromLegend();

        }

        ui->axis4isodoses->replot();
    }






    planeMtrx.clear();

    //==============================================================
    ///        Настройка осей графика
    //==============================================================
    ///
    int MaxInd;
    double xShift = 0 , yShift = 0;

    // Организация области отображения. Можно переделать, с учетом растяжения графика
    if(fieldWidth > fieldHeight)
    {
        MaxInd=fieldWidth;
        yShift = 0.5*(fieldWidth - fieldHeight);
    }else
    {
        MaxInd=fieldHeight;
        xShift = 0.5*(fieldHeight - fieldWidth);
    }
    ui->axis4isodoses->xAxis->setRange(0-xShift,MaxInd-xShift);
    ui->axis4isodoses->yAxis->setRange(0-yShift,MaxInd-yShift);

    //Задание подписи осей
    QFont labelFont = font();
    labelFont.setPointSize(14);
    ui -> axis4isodoses -> xAxis -> setLabelFont(labelFont);
    ui -> axis4isodoses -> yAxis -> setLabelFont(labelFont);

//    ui -> axis4isodoses -> xAxis ->setTickLabels(false);
//    ui -> axis4isodoses -> yAxis ->setTickLabels(false);


    ui -> axis4isodoses -> legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(14); // and make a bit smaller for legend
    ui -> axis4isodoses -> legend->setFont(legendFont);
    ui -> axis4isodoses -> legend->setBrush(QBrush(QColor(255,255,255,230)));

    QPoint tw;
    tw.setX(0);
    tw.setY(0);
    ui -> axis4isodoses-> axisRect()->insetLayout()->outerRect().setTopRight(tw);

    // create and prepare a text layout element:
    QCPTextElement *legendTitle = new QCPTextElement(ui -> axis4isodoses);
    legendTitle->setLayer(ui -> axis4isodoses->legend->layer()); // place text element on same layer as legend, or it ends up below legend
    legendTitle->setText("Dose, Gy");
    legendTitle->setFont(QFont(legendFont));
    // then we add it to the QCPLegend (which is a subclass of QCPLayoutGrid):
    if (ui -> axis4isodoses->legend->hasElement(0, 0)) // if top cell isn't empty, insert an empty row at top
        ui -> axis4isodoses->legend->insertRow(0);
    ui -> axis4isodoses->legend->addElement(0, 0, legendTitle); // place the text element into the empty cell


    ui->axis4isodoses->axisRect()->setBackground(Qt::gray);
    ui->axis4isodoses->replot();


    ui -> buildIsodose_btn -> setText("Hide graph");
}

void MainWindow::buildIsoCMAP(){

    if (!(isodosesAreSet & doseVectorIsSet)) return;


    QString currentText = ui->buildIsodose_btn->text();


    ui -> buildIsodose_btn -> setText("Print graph");


    ui -> axis4isodoses -> setCurrentLayer("CMAP");
    ui -> axis4isodoses -> clearPlottables();
    ui -> axis4isodoses -> clearItems();
    ui -> axis4isodoses -> clearGraphs();
    ui-> axis4isodoses -> legend->clear();


    //ui -> axis4isodoses->layer("CMAP")->
    //ui -> axis4isodoses->layer("isodoses")->setVisible(false);

    QCPColorMap *colorMap = new QCPColorMap(ui->axis4isodoses->xAxis, ui->axis4isodoses->yAxis);

    QString xAxlabel="", yAxlabel = "";


    //double value , v2;
    double doseMin = isodoses.front();
    double doseMax = isodoses.back();
    double temp = doseMax;
    doseMin > doseMax ? doseMax = doseMin , doseMin = temp : false;

    doseMin = 0;
    std::vector<double> altIsodoses;
    for (unsigned int i=0;i<isodoses.size();i++){
        //value = 0.9*(isodoses[i] - doseMin)/doseMax;
        altIsodoses.push_back(isodoses[i]/doseMax);
    }


    bool pointIsSet; // указание что точка округлена до ближайшего уровня изодозы
    unsigned long lvl;//текущий уровень изодозы
    double doseVal;
    int numOfEmptyColors = 0;
    // int n = altIsodoses.size();

    colorMap->data()->setSize(fieldWidth, fieldHeight);
    colorMap->data()->setRange(QCPRange(0, fieldWidth-1),
                               QCPRange(0, fieldHeight-1));
    for (int x=0; x < fieldWidth; ++x)
    {
        for (int y=0; y < fieldHeight; ++y)
        {
            doseVal=doseVector[doseIndex(x,y)]/doseMax;// нормированное значение дозы в точке
            lvl=0;//текущий уровень изодозы
            do{
                pointIsSet=false;
                if(doseVal > altIsodoses[lvl])
                {
                    doseVal = 0.9*altIsodoses[lvl];
                    pointIsSet=true;
                }else{
                    if(lvl >= altIsodoses.size()-1-numOfEmptyColors)
                    {// Случай, если дозы меньше нижнего уровня
                        doseVal=0;
                        pointIsSet=true;
                    }
                    lvl++;
                }
            }while(!pointIsSet);
            colorMap->data()->setCell(x, y, doseVal);
        }
    }

    int MaxInd=0, xShift=0, yShift=0;
    if( fieldWidth > fieldHeight)
    {
        MaxInd= fieldWidth - 1;
        yShift = 0.5*(fieldWidth - fieldHeight);
    }else
    {
        MaxInd = fieldHeight - 1;
        xShift = 0.5*(fieldHeight - fieldWidth);
    }
    ui->axis4isodoses->xAxis->setRange(0-xShift,MaxInd-xShift);
    ui->axis4isodoses->yAxis->setRange(0-yShift,MaxInd-yShift);

    QCPColorGradient jetCM;
    jetCM.loadPreset(QCPColorGradient::gpJet);
    jetCM.setLevelCount(1000);

    colorMap->setGradient(jetCM);
    colorMap->setDataRange(QCPRange(0, 1));
    colorMap->setInterpolate(false);

    QFont labelFont = font();
    labelFont.setPointSize(14);
    ui -> axis4isodoses -> xAxis->setLabelFont(labelFont);
    ui -> axis4isodoses -> yAxis->setLabelFont(labelFont);

    ui -> axis4isodoses -> xAxis->setTickLabelFont(labelFont);
    ui -> axis4isodoses -> yAxis->setTickLabelFont(labelFont);

    ui -> axis4isodoses -> xAxis->setLabel(xAxlabel);
    ui -> axis4isodoses -> yAxis->setLabel(yAxlabel);

    ui -> axis4isodoses -> replot();


    //==============================================================
    ///        Построение изодоз поверх карты
    //==============================================================
    ///

    //ui -> axis4isodoses -> setCurrentLayer("isodoses");

    // Общий стиль линий
    QPen PenStyle;
    PenStyle.setWidth(3);
    PenStyle.setStyle(Qt::DashLine);
    //PenStyle.setBrush(Qt::black);

    QPen BlackPen;
    BlackPen.setWidth(3);
    BlackPen.setStyle(Qt::DashLine);
    BlackPen.setBrush(Qt::black);

    QVector<double> xData, yData, tData;

    int curveLength;
    int clearObjLessThen = 2;

    bool traceCavities =  true;//false;//

    // Вектор всех кривых на графике
    QVector<QCPCurve*> newCurve;
    // Считчик контуров
    int curveID = -1;

    //==============================================================
    ///        Настройка легенды
    //==============================================================
    ///
    ui -> axis4isodoses -> legend ->clearItems();
    //ui -> axis4isodoses -> legend -> setInteraction(QCP::iRangeDrag, true);
    ui -> axis4isodoses -> setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    ui -> axis4isodoses -> legend->setVisible(false);
    bool newDoseLine = true;
    QString doseLineName;

    //==============================================================
    ///        Задание матрицы текущего среза
    //==============================================================
    ///
    // Инициализация
    std::vector<std::vector<double>> planeMtrx (fieldWidth, std::vector<double>(fieldHeight, 0.0));

    // Присвоение элементов среза
    for (int x=0; x<fieldWidth; x++)
    {
        for (int y=0; y<fieldHeight; y++)
        {
            planeMtrx[x][y]=doseVector[doseIndex(x,y)];
        }
    }

    //==============================================================
    ///        Поиск изодозных кривых
    //==============================================================
    ///

    int N_lvls = isodoses.size();

    //Инициализация поиска
    QVector<MooreTracing> isoCurve(N_lvls);
    bool reverseSearchingOn;// = true;
    bool plusReverse = true;

    //Сканирование по изодозам
    for(int lvl=0; lvl<N_lvls;lvl++)
    {
        //Поиск внешних контуров
        newDoseLine = true;
        PenStyle.setColor(colorList[lvl]);
        isoCurve[lvl] = MooreTracing(planeMtrx,fieldWidth,fieldHeight,isodoses[lvl]);

        while(isoCurve[lvl].traceNewObj(!traceCavities))
        {
            curveX.clear();
            curveY.clear();
            xData.clear();
            yData.clear();
            tData.clear();

            curveX = isoCurve[lvl].getNewTraceX();
            curveY = isoCurve[lvl].getNewTraceY();
            curveLength=curveX.size();

            if(curveX.empty()) continue;
            if(clearObjLessThen > curveLength) continue;

            curveID++;
            //ContourCounter++;
            //qDebug() << newCurve.size() <<'\n';

            newCurve.push_back(new QCPCurve(ui -> axis4isodoses ->xAxis, ui -> axis4isodoses ->yAxis));

            xData.resize(curveLength);
            yData.resize(curveLength);
            tData.resize(curveLength);

            for (int i=0; i<curveLength; i++)
            {
                xData[i] = curveX[i];
                yData[i] = curveY[i];
                tData[i] = i;
            }

            newCurve[curveID]->setData(tData, xData, yData);
            newCurve[curveID]->setLineStyle(QCPCurve::LineStyle::lsLine);
            newCurve[curveID]->setPen(PenStyle);

            if( newDoseLine){
                newDoseLine = false;
                doseLineName = QString::number(isodoses[lvl]);
                newCurve[curveID] -> setName(doseLineName);
            }else{
                newCurve[curveID] -> removeFromLegend();
            }

            curveID++;
            newCurve.push_back(new QCPCurve(ui -> axis4isodoses ->xAxis, ui -> axis4isodoses ->yAxis));
            newCurve[curveID]->setData(tData, xData, yData);
            newCurve[curveID]->setLineStyle(QCPCurve::LineStyle::lsLine);
            newCurve[curveID]->setPen(BlackPen);
            newCurve[curveID] -> removeFromLegend();

            ui->axis4isodoses->replot();


        }

        if (plusReverse){
            //Поиск внешних контуров для значений меньше порога
            reverseSearchingOn = true;
            isoCurve[lvl] = MooreTracing(planeMtrx,fieldWidth,fieldHeight,isodoses[lvl],reverseSearchingOn);
            PenStyle.setColor(colorList[lvl+1]);
            while(isoCurve[lvl].traceNewObj(!traceCavities))
            {
                curveX.clear();
                curveY.clear();
                xData.clear();
                yData.clear();
                tData.clear();

                curveX = isoCurve[lvl].getNewTraceX();
                curveY = isoCurve[lvl].getNewTraceY();
                curveLength=curveX.size();

                if(curveX.empty()) continue;
                if(clearObjLessThen > curveLength) continue;

                curveID++;
                //ContourCounter++;
                //qDebug() << newCurve.size() <<'\n';

                newCurve.push_back(new QCPCurve(ui -> axis4isodoses ->xAxis, ui -> axis4isodoses ->yAxis));

                xData.resize(curveLength);
                yData.resize(curveLength);
                tData.resize(curveLength);

                for (int i=0; i<curveLength; i++)
                {
                    xData[i] = curveX[i];
                    yData[i] = curveY[i];
                    tData[i] = i;
                }

                newCurve[curveID]->setData(tData, xData, yData);
                newCurve[curveID]->setLineStyle(QCPCurve::LineStyle::lsLine);
                newCurve[curveID]->setPen(PenStyle);

                if( newDoseLine){
                    newDoseLine = false;
                    doseLineName = QString::number(isodoses[lvl]);
                    newCurve[curveID] -> setName(doseLineName);
                }else{
                    //                doseLineName = QString::number(isodoses[lvl]) + " Gy";
                    //                newCurve[curveID] -> setName(doseLineName);
                    newCurve[curveID] -> removeFromLegend();
                }

                curveID++;
                newCurve.push_back(new QCPCurve(ui -> axis4isodoses ->xAxis, ui -> axis4isodoses ->yAxis));
                newCurve[curveID]->setData(tData, xData, yData);
                newCurve[curveID]->setLineStyle(QCPCurve::LineStyle::lsLine);
                newCurve[curveID]->setPen(BlackPen);
                newCurve[curveID] -> removeFromLegend();

                ui->axis4isodoses->replot();
            }

        }
    }


    planeMtrx.clear();

    //==============================================================
    ///        Настройка осей графика
    //==============================================================
    ///

    // Организация области отображения. Можно переделать, с учетом растяжения графика
    if(fieldWidth > fieldHeight)
    {
        MaxInd=fieldWidth;
        yShift = 0.5*(fieldWidth - fieldHeight);
    }else
    {
        MaxInd=fieldHeight;
        xShift = 0.5*(fieldHeight - fieldWidth);
    }
    ui->axis4isodoses->xAxis->setRange(0-xShift,MaxInd-xShift);
    ui->axis4isodoses->yAxis->setRange(0-yShift,MaxInd-yShift);

    //Задание подписи осей
    ui -> axis4isodoses -> xAxis -> setLabelFont(labelFont);
    ui -> axis4isodoses -> yAxis -> setLabelFont(labelFont);

 //   ui -> axis4isodoses -> xAxis ->setTickLabels(false);
 //   ui -> axis4isodoses -> yAxis ->setTickLabels(false);


    ui -> axis4isodoses -> legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(14); // and make a bit smaller for legend
    ui -> axis4isodoses -> legend->setFont(legendFont);
    ui -> axis4isodoses -> legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    // ui -> axis4isodoses -> axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignLeft);

    QPoint tw;
    tw.setX(0);
    tw.setY(0);
    ui -> axis4isodoses-> axisRect()->insetLayout()->outerRect().setTopRight(tw);

    // create and prepare a text layout element:
    QCPTextElement *legendTitle = new QCPTextElement(ui -> axis4isodoses);
    legendTitle->setLayer(ui -> axis4isodoses->legend->layer()); // place text element on same layer as legend, or it ends up below legend
    legendTitle->setText("Dose, Gy");
    legendTitle->setFont(QFont(legendFont));
    // then we add it to the QCPLegend (which is a subclass of QCPLayoutGrid):
    if (ui -> axis4isodoses->legend->hasElement(0, 0)) // if top cell isn't empty, insert an empty row at top
        ui -> axis4isodoses->legend->insertRow(0);
    ui -> axis4isodoses->legend->addElement(0, 0, legendTitle); // place the text element into the empty cell


    ui->axis4isodoses->axisRect()->setBackground(Qt::gray);
    ui->axis4isodoses->replot();

}



//==================================================
///===============Задание сетки==================
//==================================================

void MainWindow::updateGrid(){

    if(ui->gridOn_ChB->isChecked()){
        setCustomGrid();
    }
}

void MainWindow::changeGridChBox(int boxValue){

    qDebug() << boxValue;
    if(boxValue==0){
        qDebug() << "now off";
        ui->axis4isodoses->xAxis->grid()->setVisible(true);
        ui->axis4isodoses->yAxis->grid()->setVisible(true);

    }else if(boxValue==2){
        qDebug() << "now on";
        setCustomGrid();
        ui->axis4isodoses->xAxis->grid()->setVisible(false);
        ui->axis4isodoses->yAxis->grid()->setVisible(false);
    }
}

void MainWindow::setCustomGrid(){

    double dX  = ui->gridStepX_edit  -> text().toDouble();
    double dY  = ui->gridStepY_edit  -> text().toDouble();
    double X0  = ui->gridStartX_edit -> text().toDouble();
    double Y0  = ui->gridStartY_edit -> text().toDouble();
    double deg  = ui->gridAngle_edit -> text().toDouble();


    double angle = 3.14*deg/180;
    double tangence = tan(angle);
    //double tangence = tan(angle);

    int Nx,Ny;

    Nx = 1+ceil((fieldWidth -X0)/dX);
    Ny = 1+ceil((fieldHeight-Y0)/dY);

    ui -> axis4isodoses -> setCurrentLayer("customGrid");
    ui -> axis4isodoses->clearGraphs();
    int graphCounter=-1;

    QPen PenStyle;
    PenStyle.setWidth(2);
    PenStyle.setColor(Qt::white);
    PenStyle.setStyle(Qt::DashLine);

    QVector<double> x(2), y(2);

    for(int i=-1;i<Nx;i++){

        x[0] = X0 + i*dX;
        y[0] = 0;

        x[1] = X0*(1+tangence) + i*dX;
        y[1] = fieldHeight;

        graphCounter++;
        ui -> axis4isodoses -> addGraph();
        //ui -> axis4isodoses -> graph(graphCounter)->setName("D(z)");
        // Построение гафика #M
        ui -> axis4isodoses -> graph(graphCounter)->setData(x, y);
        ui -> axis4isodoses -> graph(graphCounter)->setPen(PenStyle);
        ui -> axis4isodoses -> graph(graphCounter)->setLineStyle(QCPGraph::lsLine);
        ui -> axis4isodoses -> graph(graphCounter)-> removeFromLegend();
        // ui -> axis4isodoses->replot();
    }

    for(int i=-1;i<Ny;i++){

        y[0] = Y0 + i*dY;
        x[0] = 0;

        y[1] = Y0*(1-tangence) + i*dY;
        x[1] = fieldWidth;

        graphCounter++;
        ui -> axis4isodoses -> addGraph();
        //ui -> axis4isodoses -> graph(graphCounter)->setName("D(z)");
        // Построение гафика #M
        ui -> axis4isodoses -> graph(graphCounter)->setData(x, y);
        ui -> axis4isodoses -> graph(graphCounter)->setPen(PenStyle);
        ui -> axis4isodoses -> graph(graphCounter)->setLineStyle(QCPGraph::lsLine);
        ui -> axis4isodoses -> graph(graphCounter)-> removeFromLegend();
    }


    ui -> axis4isodoses->replot();
}


//==================================================
///===============   Служебные    ==================
//==================================================

void MainWindow::printDoseFieldInFile(double lvl, std::vector<std::vector<double>> planeMtrx,QString fName){

    QFile newFile(fName);
    newFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
    QTextStream newData(&newFile);
    newData << "1" << '\t' << fieldWidth <<'\t' << fieldHeight << '\n';
    for (int j=0; j < fieldHeight; j++)
    {
        for (int i=0; i < fieldWidth; i++)
        {
            if(planeMtrx[i][j] >= lvl){
                newData << 1 << "  ";
            }else{
                newData << 0 << "  ";
            }

            //newData << planeMtrx[i][j] << "  ";
        }
        newData << '\n';
    }
    newFile.close();

}




void MainWindow::showCursor(QMouseEvent *event)
{
    int mouseX = floor(ui->axis4isodoses->xAxis->pixelToCoord(event->pos().x()));
    int mouseY = floor(ui->axis4isodoses->yAxis->pixelToCoord(event->pos().y()));

   // qDebug() << "Xpos = " << mouseX;
   // qDebug() << "Ypos = " << mouseY;

    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->setTitle("Dose at");
   // menu->setAttribute(Qt::WA_D)

    QString strX = "X = " +  QString::number(mouseX);
    QString strY = "Y = " +  QString::number(mouseY);



    QLabel* label1 = new QLabel(strX, this);
    label1->setAlignment(Qt::AlignLeft);
    QLabel* label2 = new QLabel(strY, this);
    label2->setAlignment(Qt::AlignLeft);


    QWidgetAction* line1 = new QWidgetAction(menu);
    QWidgetAction* line2 = new QWidgetAction(menu);

    line1->setDefaultWidget(label1);
    line2->setDefaultWidget(label2);


    menu -> addAction(line1);
    menu -> addAction(line2);

    if (doseVectorIsSet && !((mouseX < 0)|| (mouseX > fieldWidth)||(mouseY < 0)||(mouseY > fieldHeight)))
    {
            double doseValue = 0.01*round(100*doseVector[doseIndex(mouseX, mouseY)]);
            QString strD = "Dose = " +  QString::number(doseValue) + " Gy";
            QLabel* label3 = new QLabel(strD, this);
            label1->setAlignment(Qt::AlignLeft);
            QWidgetAction* line3 = new QWidgetAction(menu);
            line3->setDefaultWidget(label3);
            menu -> addAction(line3);
    }


    menu->popup(ui->axis4isodoses->mapToGlobal(event->pos()));



}
