#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QMessageBox"

#include <tiffio.h>

#include "mooretracing.h"
#include "QString"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    menuBar()->setNativeMenuBar(false);
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
        isodoseArea = ui->axis4isodoses;

        isodoseArea->axisRect()->setAutoMargins(QCP::msLeft | QCP::msTop | QCP::msBottom);
        isodoseArea->axisRect()->setMargins(QMargins(0,0,150,0));
        isodoseArea->axisRect()->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);
        //isodoseArea->axisRect()->insetLayout()->setInsetRect(0, QRectF(1.04,-0.025,0.1,0.1));
        isodoseArea->axisRect()->insetLayout()->setInsetRect(0, QRectF(1.0,0,0.1,0.1));
        isodoseArea->axisRect()->insetLayout()->setMargins(QMargins(5,0,0,0));

        isodoseArea->yAxis -> setRangeReversed(true);
        isodoseArea->axisRect()->setBackground(QBrush(QColor(211,211,211,255)));//setBackground(Qt::gray);
        bool ticksOn  = false;
        isodoseArea->xAxis ->setTickLabels(ticksOn);
        isodoseArea->yAxis ->setTickLabels(ticksOn);
        isodoseArea->replot();

        //isodoseArea->setInteractions(QCP::iSelectPlottables);
        isodoseArea->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom );

        /* main
     * image
     * customGrid
     * probesArea
     * isodoses
     */
        isodoseArea->addLayer("image"      ,isodoseArea->layer("main")      , QCustomPlot::limAbove);
        isodoseArea->addLayer("CMAP"       ,isodoseArea->layer("image")  , QCustomPlot::limAbove);
        isodoseArea->addLayer("customGrid" ,isodoseArea->layer("CMAP")     , QCustomPlot::limAbove);
        isodoseArea->addLayer("probesArea" ,isodoseArea->layer("customGrid"), QCustomPlot::limAbove);
        isodoseArea->addLayer("isodoses"   ,isodoseArea->layer("probesArea"), QCustomPlot::limAbove);

        connect(isodoseArea, SIGNAL(mouseDoubleClick(QMouseEvent*)), this, SLOT(showCursor(QMouseEvent*)));

        ui -> axisDVH->setInteraction(QCP::iSelectLegend, QCP::iSelectPlottables);
        ui -> axisDVH->axisRect()->setAutoMargins(QCP::msLeft | QCP::msTop | QCP::msBottom);
        ui -> axisDVH->axisRect()->setMargins(QMargins(0,0,150,0));
        ui -> axisDVH->axisRect()->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);
        ui -> axisDVH->axisRect()->insetLayout()->setInsetRect(0, QRectF(1.04,-0.025,0.1,0.1));


        connect(isodoseArea,   SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
        connect(ui -> axisDVH, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));
        connect(ui -> axisDVH, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    }






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
        targetListIsSet = false;

        ui->mainMenu->setCurrentIndex(0);

        resizeTable();

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
                         this                        , SLOT  (pushIsoCMAP()));

        QObject::connect(ui->gridOn_ChB              , SIGNAL(stateChanged(int)),
                         this                        , SLOT  (changeGridChBox(int)));


        QObject::connect(ui->rename_btn              , SIGNAL(clicked()),
                         this                        , SLOT  (renameTargets()));




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


        QObject::connect(ui->targetNumX_edit         , SIGNAL(editingFinished()),
                         this                        , SLOT  (resizeTable()));
        QObject::connect(ui->targetNumY_edit         , SIGNAL(editingFinished()),
                         this                        , SLOT  (resizeTable()));


        QObject::connect(ui->targetOn_ChB            , SIGNAL(stateChanged(int)),
                         this                        , SLOT  (changeTargetChBox(int)));

        QObject::connect(ui->targetNumX_edit         , SIGNAL(editingFinished()),
                         this                        , SLOT  (setTargetList()));
        QObject::connect(ui->targetNumY_edit         , SIGNAL(editingFinished()),
                         this                        , SLOT  (setTargetList()));
        QObject::connect(ui->targetRx_edit           , SIGNAL(editingFinished()),
                         this                        , SLOT  (setTargetList()));
        QObject::connect(ui->targetRy_edit           , SIGNAL(editingFinished()),
                         this                        , SLOT  (setTargetList()));

        QObject::connect(ui->gridStepX_edit          , SIGNAL(textEdited(QString)),
                         this                        , SLOT  (copyGridXStep2Radii(QString)));
        QObject::connect(ui->targetRx_edit           , SIGNAL(textEdited(QString)),
                         this                        , SLOT  (copyRxVal2Ry(QString)));




        QObject::connect(ui->dvh_Dmin_edit           , SIGNAL(editingFinished()),
                         this                        , SLOT  (setDoseRange4DVH()));
        QObject::connect(ui->dvh_Dmax_edit           , SIGNAL(editingFinished()),
                         this                        , SLOT  (setDoseRange4DVH()));

        //        QObject::connect(ui->table_feature_cBox      , SIGNAL(activated(int)),
        //                         this                        , SLOT  (printTargetStats()));

        QObject::connect(ui->openGraph_btn           , SIGNAL(clicked()),
                         this                        , SLOT  (openGraph()));



        QObject::connect(ui->actionSave_table         ,SIGNAL(triggered()),
                         this                         , SLOT(saveTables()));

        QObject::connect(ui->actionSaveDVH, SIGNAL(triggered()),
                         this, SLOT(saveDVH()));

        QObject::connect(ui->actionSaveIsodoses, SIGNAL(triggered()),
                         this, SLOT(saveIsodose()));

        QObject::connect(ui->action90_left, SIGNAL(triggered()),
                         this, SLOT(rotate90degLeft()));
        QObject::connect(ui->action90_right, SIGNAL(triggered()),
                         this, SLOT(rotate90degRight()));
        QObject::connect(ui->action_hFlip, SIGNAL(triggered()),
                         this, SLOT(flipHorizontally()));
        QObject::connect(ui->action_vFlip, SIGNAL(triggered()),
                         this, SLOT(flipVertically()));


        QObject::connect(ui->addRect_btn             , SIGNAL(clicked()),
                         this                        , SLOT  (addToRectangleTarget()));
        QObject::connect(ui->remRect_btn             , SIGNAL(clicked()),
                         this                        , SLOT  (remFromRectangleTarget()));

        QObject::connect(ui->clearRect_btn           , SIGNAL(clicked()),
                         this                        , SLOT  (clearRectangleTarget()));



    }



    //==================================================
    ///===================ОТЛАДКА=======================
    //==================================================
    ///
    {
        bool preset = true;

        if(preset){
           setLineValidators();

           ui->gridStartX_edit->setText("3");
           ui->gridStartY_edit->setText("3");
           ui->gridStepX_edit->setText("71");
           ui->gridStepY_edit->setText("71");
           ui->gridOn_ChB->setChecked(true);//setCheckState(Qt::CheckState(checked));

           ui->targetRx_edit->setText("35.5");
           ui->targetRy_edit->setText("35.5");
           ui->targetOn_ChB->setChecked(true);

        }

        bool t1 = false;//true;//
        bool t2 = false;//true;//
        bool t3 = false;//true;//
        bool t4 = true;//true;//
        if (t1)
        {

            loadTIFF("/home/ivan/work/EBT/RTA/220х330-D-001-OD-dose.tif");
            //loadTIFF("/home/ivan/work/EBT/films/B02.tif");
            setLineValidators();

            //isodoses.push_back(1);

            ui->isodoseLines_edit->setText("7:0.5:8, 4:0.5:5, 2:0.5:3, 0.1:0.2:1.1");
            //ui->isodoseLines_edit->setText("11:-0.5:9, 6.5:-0.5:5.5, 4:-0.5:0.5");
            //ui->isodoseLines_edit->setText("5.5, 5:3");
            setIsodoseLines();
            isodosesAreSet = true;

            buildIsodoses(isodoseArea);

            ui->gridStartX_edit->setText("20");
            ui->gridStartY_edit->setText("13");
            ui->gridStepX_edit->setText("54");
            ui->gridStepY_edit->setText("54");
            ui->gridOn_ChB->setChecked(false);//setCheckState(Qt::CheckState(checked));

            ui->targetRx_edit->setText("26");
            ui->targetRy_edit->setText("26");
            ui->targetOn_ChB->setChecked(false);

            ui->openGraph_btn->clicked();

        }
        if (t2)
        {

            loadTIFF("/home/ivan/work/EBT/RTA/test190820/330х220-E-001-OD-dose.tif");
            flipHorizontally();

            setLineValidators();

            //isodoses.push_back(1);

            ui->isodoseLines_edit->setText("2:0.5:5");

            setIsodoseLines();
            isodosesAreSet = true;

            buildIsodoses(isodoseArea);

            ui->gridStartX_edit->setText("46");//44");//
            ui->gridStartY_edit->setText("52");//58");//
            ui->gridStepX_edit->setText("56.5");
            ui->gridStepY_edit->setText("56.5");
            ui->gridAngle_edit->setText("0.5");
            ui->gridOn_ChB->setChecked(true);//setCheckState(Qt::CheckState(checked));

            ui->targetNumX_edit->setText("5");
            ui->targetNumY_edit->setText("3");
            ui->targetRx_edit->setText("22");
            ui->targetRy_edit->setText("22");
            ui->targetOn_ChB->setChecked(true);





            ui->openGraph_btn->clicked();

        }
        if (t3)
        {

            loadTIFF("/home/ivan/work/EBT/RTA/220х330-D-005-OD-dose.tif");
            //rotate90degLeft();
            flipHorizontally();

            setLineValidators();

            //isodoses.push_back(1);

            ui->isodoseLines_edit->setText("4:0.25:4.75");

            setIsodoseLines();
            isodosesAreSet = true;

            buildIsodoses(isodoseArea);

            ui->gridStartX_edit->setText("36.5");//44");//
            ui->gridStartY_edit->setText("135.5");//58");//
            ui->gridStepX_edit->setText("27");
            ui->gridStepY_edit->setText("27");
//            ui->gridAngle_edit->setText("0.5");
          //  ui->gridOn_ChB->setChecked(true);//setCheckState(Qt::CheckState(checked));

            ui->targetNumX_edit->setText("4");
            ui->targetNumY_edit->setText("2");
            ui->targetRx_edit->setText("9");
            ui->targetRy_edit->setText("9");

            ui->targetNameCol_edit->setText("2");
            ui->targetNameRow_edit->setText("F");

          //  ui->targetOn_ChB->setChecked(true);





//            ui->openGraph_btn->clicked();

        }
        if (t4)
        {

            loadTIFF("/home/ivy/work/EBT_films/data/PT_2021-04/1.0_inch/DOSE/002-OD-dose.tif");

            setLineValidators();

            ui->isodoseLines_edit->setText("14:22");

            setIsodoseLines();
            isodosesAreSet = true;

            buildIsodoses(isodoseArea);

        }

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

    QDoubleValidator* validator1 = new QDoubleValidator(-fieldWidth, fieldWidth , 2, this);
    validator1->setNotation(QDoubleValidator::StandardNotation);
    validator1->setLocale(QLocale::English);

    ui->gridStartX_edit -> setValidator(validator1);
    ui->gridStartY_edit -> setValidator(validator1);

    QIntValidator* validator2 = new QIntValidator(0,100,this);
    validator2->setLocale(QLocale::English);
    ui->targetNumX_edit->setValidator(validator2);
    ui->targetNumY_edit->setValidator(validator2);

    QDoubleValidator* validator3 = new QDoubleValidator(0, 100 , 2, this);//(0, fieldWidth , 2, this);
    validator3->setLocale(QLocale::English);
    ui->targetRx_edit->setValidator(validator3);
    ui->targetRy_edit->setValidator(validator3);
    //ui->MaxVal_pointX-> setValidator(new QIntValidator(0,sizeX-1,this));

    QDoubleValidator* validator4 = new QDoubleValidator(-180, 180 , 4, this);//(0, fieldWidth , 2, this);
    validator4->setLocale(QLocale::English);
    ui->gridAngle_edit -> setValidator(validator4);

    //QValidator* validator5 = new QValidator(this);
    ui->targetNameRow_edit->setMaxLength(1);

     ui->targetNameCol_edit->setMaxLength(2);
     ui->targetNameCol_edit->setValidator(validator2);

     int maxSize;
     fieldHeight > fieldWidth ? maxSize = fieldHeight : maxSize = fieldWidth;
     QDoubleValidator* validator5 = new QDoubleValidator(0, maxSize, 2, this);
     validator5 -> setLocale(QLocale::English);
     ui -> rectHeigth_edit ->setValidator(validator5);
     ui -> rectWidth_edit ->setValidator(validator5);
     ui -> rectCx_edit ->setValidator(validator5);
     ui -> rectCy_edit ->setValidator(validator5);


}

void MainWindow::pushIsodoseButton(){

    QString currentText = ui->buildIsodose_btn->text();

    if ((currentText == "Hide graph")){
        isodoseArea->setCurrentLayer("isodoses");
        isodoseArea->clearPlottables();
        isodoseArea->clearItems();
        isodoseArea->clearGraphs();

        isodoseArea->replot();

        //dosePrintLock = true;
        ui -> buildIsodose_btn -> setText("Print graph");

    }else if(currentText == "Print graph"){


        //dosePrintLock = false;
        //ui -> buildIsodose_btn -> setText("Hide graph");
        buildIsodoses(isodoseArea);

    }


}

void MainWindow::copyGridXStep2YStep(QString txt){

    ui->gridStepY_edit->setText(txt);

}

void MainWindow::copyGridXStep2Radii(QString txt){
    ui->targetRx_edit->setText(QString::number(0.5*txt.toDouble()));
    ui->targetRy_edit->setText(QString::number(0.5*txt.toDouble()));
}
void MainWindow::copyRxVal2Ry(QString txt){
    ui->targetRy_edit->setText(txt);
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

    buildIsodoses(isodoseArea);

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
        doseV.setAs(doseVector,fieldWidth,fieldHeight,1);
        double doseMax = 0.1*round(10*doseV.getDoseMaxValue());
        ui->dvh_Dmax_edit->setText(QString::number(doseMax));//

        setEmtyRectangleTarget();
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
    if (stringSize==0) return;

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

    buildIsodoses(isodoseArea);
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
        if      ((strLines[checkPoint] == ' ') || (strLines[checkPoint] == '\t'))
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

        if ( (strLines[i] == ' ') || (strLines[i] == '\t'))
        {
            qDebug() << "warning";
        }else if( strLines[i] == '.'){
            strVal.push_back(strLines[i]);
            //               ((strLines[i].isLetter() & (strLines[i] != ".")) || (strLines[i] == "-") || (strLines[i] == "+") || (!strLines[i].isLetterOrNumber()))
        }else if( strLines[i].isLetter()  || (strLines[i] == '-') || (strLines[i] == '+') || (!strLines[i].isLetterOrNumber()))
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
        }else if((strLines[checkPoint] == ' ') || (strLines[checkPoint] == '\t'))
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
                    } while (( strLines[nPoint] == ' ') || ( strLines[nPoint] == '\t'));
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
        }else if(strLines[checkPoint].isLetter() & (strLines[checkPoint] != '.')) {
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


    //doseMin = 0;
    for (std::vector<double>::iterator it = isodoses.begin() ; it != isodoses.end(); ++it){
        value = colorModifier*(*it - doseMin)/(doseMax-doseMin);
        colorList.push_back(jetCM.color(value,QCPRange(0, 1), false));
    }
    colorList.push_back(jetCM.color(0,QCPRange(0, 1), false));

}

void MainWindow::buildIsodoses(QCustomPlot* printArea)
{
    if (!(isodosesAreSet & doseVectorIsSet)) return;
    currentMode = "isodoses";//CMAP

    QCPLayer* layer = isodoseArea->layer("isodoses");
    isodoseArea->setCurrentLayer(layer);

    printArea->clearPlottables();
    printArea->clearItems();
    printArea->clearGraphs();


    printArea->legend->clear();

    //isodoses.clear();
    curveX.clear();
    curveY.clear();


    //if(dosePrintLock) return;

    setLinesColorStyle();

    // Общий стиль линий
    QPen PenStyle;
    PenStyle.setWidth(3);

    QVector<double> xData, yData, tData;

    int curveLength;
    int clearObjLessThen = 1;

    bool traceCavities =  true;//false;//

    // Вектор всех кривых на графике
    QVector<QCPCurve*> newCurve;
    // Считчик контуров
    int curveID = -1;


    //==============================================================
    ///        Настройка легенды
    //==============================================================
    ///
    printArea->legend ->clearItems();
    //printArea->legend -> setInteraction(QCP::iRangeDrag, true);
    printArea->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    printArea->legend->setVisible(false);
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

    //    double ctrlDose = 15;
    //       printDoseFieldInFile(ctrlDose, planeMtrx, "Cut_B12_15.5Gy.tgt");
    //       printRevDoseFieldInFile(ctrlDose, planeMtrx, "Cut_B12_l15.5Gy.tgt");

    //==============================================================
    ///        Поиск изодозных кривых
    //==============================================================
    ///

    int N_lvls = isodoses.size();

    //Инициализация поиска
    MooreTracing isoCurve;

    // Поиск границ изодоз не вошедших в нижний предел
    bool reverseSearchingOn = true;
    newDoseLine = true;
    isoCurve.clear();
    isoCurve = MooreTracing(planeMtrx,fieldWidth,fieldHeight,isodoses[N_lvls-1],reverseSearchingOn);
    PenStyle.setColor(colorList[N_lvls]);
    while(isoCurve.traceNewObj(true))
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

        curveID++;

        newCurve.push_back(new QCPCurve(printArea ->xAxis, printArea ->yAxis));

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

        printArea->replot();
    }

    //reverseSearchingOn;// = false;
    bool plusReverse = false;//true;//

    //Сканирование по изодозам
    //  for(int lvl=0; lvl<N_lvls;lvl++)
    for(int lvl=N_lvls-1; lvl>-1;lvl--)
    {
        isoCurve.clear();
        reverseSearchingOn = false;
        newDoseLine = true;
        isoCurve = MooreTracing(planeMtrx,fieldWidth,fieldHeight,isodoses[lvl],reverseSearchingOn);

        PenStyle.setColor(colorList[lvl]);
//        if (lvl==2){
//            qDebug()<< "ID 106";
//        }
        //Поиск всех контуров
        while(isoCurve.traceNewObj(!traceCavities))
        {
            curveX.clear();
            curveY.clear();
            xData.clear();
            yData.clear();
            tData.clear();

//            if (curveID==100){
//                qDebug()<< "ID 106";
//            }
            curveX = isoCurve.getNewTraceX();
            curveY = isoCurve.getNewTraceY();
            curveLength=curveX.size();

            if(curveX.empty()) continue;
            if(clearObjLessThen > curveLength) continue;

            curveID++;

            //if (curveID==44)
//            {
//                qDebug()<< "ID " + QString::number(curveID);
//            }
            //ContourCounter++;
            //qDebug() << newCurve.size() <<'\n';

            newCurve.push_back(new QCPCurve(printArea ->xAxis, printArea ->yAxis));

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
            //printArea->replot();
        }

        if (plusReverse){
            isoCurve.clear();
            reverseSearchingOn = true;

            isoCurve = MooreTracing(planeMtrx,fieldWidth,fieldHeight,isodoses[lvl],reverseSearchingOn);
            PenStyle.setColor(colorList[lvl+1]);
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

                curveID++;

                newCurve.push_back(new QCPCurve(printArea ->xAxis, printArea ->yAxis));

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
                //printArea->replot();
            }
        }
        printArea->replot();
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
    printArea->xAxis->setRange(0-xShift,MaxInd-xShift);
    printArea->yAxis->setRange(0-yShift,MaxInd-yShift);

    //Задание подписи осей
    QFont labelFont = font();
    labelFont.setPointSize(14);
    printArea->xAxis -> setLabelFont(labelFont);
    printArea->yAxis -> setLabelFont(labelFont);

    //    printArea->xAxis ->setTickLabels(false);
    //    printArea->yAxis ->setTickLabels(false);


    printArea->legend->setVisible(true);
    QFont legendFont = font();
    legendFont.setPointSize(14);
    printArea->legend->setFont(legendFont);
    printArea->legend->setBrush(QBrush(QColor(255,255,255,230)));

    //    QPoint tw;
    //    tw.setX(0);
    //    tw.setY(0);
    //printArea-> axisRect()->insetLayout()->outerRect().setTopRight(tw);

    int rowCounter = printArea->legend->rowCount();
    QVector<QCPLayoutElement*> legendList;
    for (int row = 0; row < rowCounter; row++){
        legendList.push_back(printArea->legend->element(row, 0));
    }
    for (int row = 0; row < rowCounter; row++){
        printArea->legend->insertRow(0);
        printArea->legend->addElement(0, 0, legendList[row]);
    }

    // create and prepare a text layout element:
    QCPTextElement *legendTitle = new QCPTextElement(printArea);
    legendTitle->setLayer(printArea->legend->layer()); // place text element on same layer as legend, or it ends up below legend
    legendTitle->setText("Dose, Gy");
    legendTitle->setFont(QFont(legendFont));
    // then we add it to the QCPLegend (which is a subclass of QCPLayoutGrid):
    if (printArea->legend->hasElement(0, 0)) // if top cell isn't empty, insert an empty row at top
        printArea->legend->insertRow(0);
    printArea->legend->addElement(0, 0, legendTitle); // place the text element into the empty cell


    printArea->axisRect()->setBackground(QBrush(QColor(211,211,211,255)));//setBackground(Qt::gray);
    printArea->replot();


    //ui -> buildIsodose_btn -> setText("Hide graph");

    if(ui->gridOn_ChB->isChecked())
        setCustomGrid(printArea);

    lastTargetColor = Qt::black;
    if(ui->targetOn_ChB->isChecked())
        showTargets(printArea);
}

void MainWindow::pushIsoCMAP(){
    if (!(isodosesAreSet & doseVectorIsSet)) return;

    QString currentText = ui->buildIsodose_btn->text();

    ui -> buildIsodose_btn -> setText("Print graph");
    buildIsoCMAP(isodoseArea);

}

void MainWindow::buildIsoCMAP(QCustomPlot* printArea){

    if (!(isodosesAreSet & doseVectorIsSet)) return;

    currentMode = "CMAP";

    QCPLayer* layer = printArea->layer("CMAP");
    printArea->setCurrentLayer(layer);
    printArea->clearPlottables();
    printArea->clearItems();
    printArea->clearGraphs();
    printArea->legend->clear();

    //printArea->layer("CMAP")->
    //printArea->layer("isodoses")->setVisible(false);

    QCPColorMap *colorMap = new QCPColorMap(printArea->xAxis, printArea->yAxis);
    QString xAxlabel="", yAxlabel = "";

    //double value , v2;
    double doseMin = isodoses.front();
    double doseMax = isodoses.back();
    double temp = doseMax;
    doseMin > doseMax ? doseMax = doseMin , doseMin = temp : false;

    //doseMin = 0;
    std::vector<double> altIsodoses;
    double value;
    for (unsigned int i=0;i<isodoses.size();i++){
        //value = colorModifier*(isodoses[i] - doseMin)/(doseMax - doseMin);
        altIsodoses.push_back((isodoses[i]-doseMin)/(doseMax-doseMin));
        //altIsodoses.push_back(value);
    }
//    QCPColorGradient jetCM;
//    jetCM.loadPreset(QCPColorGradient::gpJet);
//    jetCM.setLevelCount(1000);
//    for (std::vector<double>::iterator it = isodoses.begin() ; it != isodoses.end(); ++it){
//        value = colorModifier*(*it - doseMin)/(doseMax-doseMin);
//        altIsodoses.push_back(jetCM.color(value,QCPRange(0, 1), false));
//    }

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
            doseVal=(doseVector[doseIndex(x,y)]-doseMin)/(doseMax-doseMin);// нормированное значение дозы в точке
            lvl=0;//текущий уровень изодозы
            do{
                pointIsSet=false;
                if(doseVal > altIsodoses[lvl])
                {
                    doseVal = colorModifier*altIsodoses[lvl];
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
    printArea->xAxis->setRange(0-xShift,MaxInd-xShift);
    printArea->yAxis->setRange(0-yShift,MaxInd-yShift);

    QCPColorGradient jetCM;
    jetCM.loadPreset(QCPColorGradient::gpJet);
    jetCM.setLevelCount(1000);

    colorMap->setGradient(jetCM);
    colorMap->setDataRange(QCPRange(0, 1));
    colorMap->setInterpolate(false);

    QFont labelFont = font();
    labelFont.setPointSize(14);
    printArea->xAxis->setLabelFont(labelFont);
    printArea->yAxis->setLabelFont(labelFont);

    printArea->xAxis->setTickLabelFont(labelFont);
    printArea->yAxis->setTickLabelFont(labelFont);

    printArea->xAxis->setLabel(xAxlabel);
    printArea->yAxis->setLabel(yAxlabel);

    printArea->replot();


    //==============================================================
    ///        Построение изодоз поверх карты
    //==============================================================
    ///

    //printArea->setCurrentLayer("isodoses");

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
    printArea->legend ->clearItems();
    //printArea->legend -> setInteraction(QCP::iRangeDrag, true);
    printArea->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    printArea->legend->setVisible(false);
    //bool newDoseLine = true;
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
    MooreTracing isoCurve;
  //  bool reverseSearchingOn;// = true;
  //  bool plusReverse = false;//true;

    //Сканирование по изодозам
    for(int lvl=0; lvl<N_lvls;lvl++)
    {
        isoCurve.clear();
        //Поиск внешних контуров
        //newDoseLine = true;
        PenStyle.setColor(
                    colorList[lvl]);
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
            curveID++;
            newCurve[curveID]->setData(tData, xData, yData);
            newCurve[curveID]->setLineStyle(QCPCurve::LineStyle::lsLine);
            newCurve[curveID]->setPen(PenStyle);
            doseLineName = QString::number(isodoses[lvl]);
            newCurve[curveID] -> setName(doseLineName);


            curveID++;
            newCurve.push_back(new QCPCurve(printArea->xAxis, printArea->yAxis));
            newCurve[curveID]->setData(tData, xData, yData);
            newCurve[curveID]->setLineStyle(QCPCurve::LineStyle::lsLine);
            newCurve[curveID]->setPen(BlackPen);
            newCurve[curveID] -> removeFromLegend();
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
            curveID++;
            newCurve[curveID]->setData(tData, xData, yData);
            newCurve[curveID]->setLineStyle(QCPCurve::LineStyle::lsLine);
            newCurve[curveID]->setPen(BlackPen);
            newCurve[curveID] -> removeFromLegend();
        }
        printArea->replot();
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
    printArea->xAxis->setRange(0-xShift,MaxInd-xShift);
    printArea->yAxis->setRange(0-yShift,MaxInd-yShift);

    //Задание подписи осей
    printArea->xAxis -> setLabelFont(labelFont);
    printArea->yAxis -> setLabelFont(labelFont);

    //   printArea->xAxis ->setTickLabels(false);
    //   printArea->yAxis ->setTickLabels(false);


    printArea->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(14); // and make a bit smaller for legend
    printArea->legend->setFont(legendFont);
    printArea->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    // printArea->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignLeft);

    QPoint tw;
    tw.setX(0);
    tw.setY(0);
    printArea->axisRect()->insetLayout()->outerRect().setTopRight(tw);

    // create and prepare a text layout element:
    QCPTextElement *legendTitle = new QCPTextElement(printArea);
    legendTitle->setLayer(printArea->legend->layer()); // place text element on same layer as legend, or it ends up below legend
    legendTitle->setText("Dose, Gy");
    legendTitle->setFont(QFont(legendFont));
    // then we add it to the QCPLegend (which is a subclass of QCPLayoutGrid):
    if (printArea->legend->hasElement(0, 0)) // if top cell isn't empty, insert an empty row at top
        printArea->legend->insertRow(0);
    printArea->legend->addElement(0, 0, legendTitle); // place the text element into the empty cell

    // QBrush shadeOfGray = QBrush(QColor(211,211,211,255));
    printArea->axisRect()->setBackground(QBrush(QColor(211,211,211,255)));//Qt::gray);
    printArea->replot();


    if(ui->gridOn_ChB->isChecked())
        setCustomGrid(printArea);

    lastTargetColor = Qt::white;
    if(ui->targetOn_ChB->isChecked())
        showTargets(printArea, Qt::white);

}



//==================================================
///===============Задание сетки==================
//==================================================

void MainWindow::updateGrid(){

    if(ui->gridOn_ChB->isChecked()){
        setCustomGrid(isodoseArea);
    }
    if(ui->targetOn_ChB->isChecked()){
        setTargetList();
    }
}

void MainWindow::changeGridChBox(int boxValue){

    // qDebug() << boxValue;
    if(boxValue==0){
        //qDebug() << "now off";
        isodoseArea->xAxis->grid()->setVisible(true);
        isodoseArea->yAxis->grid()->setVisible(true);
        QCPLayer* layer = isodoseArea->layer("customGrid");
        foreach (QCPLayerable* l, layer->children())
            if(qobject_cast<QCPAbstractPlottable*>(l))
                isodoseArea->removePlottable((QCPAbstractPlottable*)l);
            else if(qobject_cast<QCPAbstractItem*>(l))
                isodoseArea->removeItem((QCPAbstractItem*)l);
        isodoseArea->replot();

    }else if(boxValue==2){
        //qDebug() << "now on";
        setCustomGrid(isodoseArea);
        isodoseArea->xAxis->grid()->setVisible(false);
        isodoseArea->yAxis->grid()->setVisible(false);
    }
}

void MainWindow::setCustomGrid(QCustomPlot* printArea){

    double dX  = ui->gridStepX_edit  -> text().toDouble();
    double dY  = ui->gridStepY_edit  -> text().toDouble();
    double X0  = ui->gridStartX_edit -> text().toDouble();
    double Y0  = ui->gridStartY_edit -> text().toDouble();
    double deg  = ui->gridAngle_edit -> text().toDouble();


    double angle = 3.14*deg/180;
    double tanA = tan(angle);
    double cosA = cos(angle);

    int Nx,Ny;

    Nx = 1+ceil((fieldWidth -X0)/dX);
    Ny = 1+ceil((fieldHeight-Y0)/dY);

    printArea->setCurrentLayer("customGrid");
    printArea->clearGraphs();
    int graphCounter=-1;

    QPen PenStyle;
    PenStyle.setWidth(2);
    PenStyle.setColor(Qt::white);
    PenStyle.setStyle(Qt::DashLine);

    QVector<double> x(2), y(2);

    std::vector<double> cX, cY;
    double Xmin, Xmax;
    double Ymin, Ymax;
    //y=tanA*x + shift;
    double shift;

    for(int j=-9;j<Ny+10;j++){

        //shift = (Y0+j*dY)-tanA*X0;//для горизонтальных
        shift = Y0+(j*dY/cosA)-tanA*X0;

        Ymin = tanA*0 + shift;
        if (Ymin < 0){
            tanA==0? x[0] = 0: x[0] = -shift/tanA;
            y[0] = 0;
        }else if(Ymin>fieldHeight){
            tanA==0? x[0] = 0: x[0] = (fieldHeight-shift)/tanA;

            y[0] = fieldHeight;
        }else{
            x[0] = 0;
            y[0] = Ymin;
        }

        Ymax = tanA*fieldWidth + shift;
        if (Ymax > fieldHeight){
            ((tanA==0)||(tanA==-0))? x[1] = fieldWidth : x[1] = (fieldHeight-shift)/tanA;
            y[1] = fieldHeight;
        }else if(Ymax<0){
            ((tanA==0)||(tanA==-0))? x[1] = 0 :  x[1] = (0-shift)/tanA;;

            y[1] = 0;
        }else{
            x[1] = fieldWidth;
            y[1] = Ymax;
        }

        //if (x[0]>fieldWidth || x[1]<0 ) continue;

        graphCounter++;
        printArea->addGraph();
        printArea->graph(graphCounter)->setData(x, y);
        printArea->graph(graphCounter)->setPen(PenStyle);
        printArea->graph(graphCounter)->setLineStyle(QCPGraph::lsLine);
        printArea->graph(graphCounter)-> removeFromLegend();
        //printArea->replot();
    }

    tanA=-tanA;
    for(int i=-9;i<Nx+10;i++)
    {
        // shift = (X0+i*dX)-tanA*Y0;
        shift = X0+(i*dX/cosA)-tanA*Y0;

        Xmin = tanA*0 + shift;
        if (Xmin < 0){
            x[0] = 0;
            tanA==0? y[0] = 0: y[0] = -shift/tanA;
        }else if(Xmin>fieldWidth){
            x[0] = fieldWidth;
            y[0] = (fieldWidth-shift)/tanA;
        }else{
            x[0] = Xmin;
            y[0] = 0;
        }

        Xmax = tanA*fieldHeight + shift;
        if (Xmax > fieldWidth){
            x[1] = fieldWidth;
            tanA==0? y[1] = fieldHeight : y[1] = (fieldWidth-shift)/tanA;
        }else if(Xmax<0){
            x[1] = 0;
            y[1] = (0-shift)/tanA;
        }else{
            x[1] = Xmax;
            y[1] = fieldHeight;
        }

        graphCounter++;
        printArea->addGraph();
        printArea->graph(graphCounter)->setData(x, y);
        printArea->graph(graphCounter)->setPen(PenStyle);
        printArea->graph(graphCounter)->setLineStyle(QCPGraph::lsLine);
        printArea->graph(graphCounter)-> removeFromLegend();
        //printArea->replot();
    }
    printArea->replot();
}

//==================================================
///==============Построение мишеней=================
//==================================================

void MainWindow::updateTargetGrid(){

    if(ui->targetOn_ChB->isChecked()){
        setTargetList();
    }
}

void MainWindow::changeTargetChBox(int boxValue){

    if(boxValue==0){
        QCPLayer* layer = isodoseArea->layer("probesArea");
        foreach (QCPLayerable* l, layer->children())
            if(qobject_cast<QCPAbstractPlottable*>(l))
                isodoseArea->removePlottable((QCPAbstractPlottable*)l);
            else if(qobject_cast<QCPAbstractItem*>(l))
                isodoseArea->removeItem((QCPAbstractItem*)l);
        isodoseArea->replot();

    }else if(boxValue==2){
        //showTargets();
        setTargetList();
    }
}

void MainWindow::showTargets(QCustomPlot* printArea, QColor tgtColor){

    if(!targetListIsSet) return;


    QCPLayer* layer = printArea->layer("probesArea");
    printArea->setCurrentLayer(layer);
    foreach (QCPLayerable* l, layer->children())
        if(qobject_cast<QCPAbstractPlottable*>(l))
            printArea->removePlottable((QCPAbstractPlottable*)l);
        else if(qobject_cast<QCPAbstractItem*>(l))
            printArea->removeItem((QCPAbstractItem*)l);

    QPen PenStyle;
    PenStyle.setWidth(2);
    PenStyle.setColor(lastTargetColor); //tgtColor
    PenStyle.setStyle(Qt::DashLine);

    std::vector<std::vector<double>> planeMtrx (fieldWidth, std::vector<double>(fieldHeight, 0.0));

    //Инициализация поиска
    MooreTracing tgtContour;
    int clearObjLessThen=2;
    bool traceCavities = true;
    std::vector<int> curveX;
    std::vector<int> curveY;
    QVector<double> xData, yData, tData;
    int curveLength;
    QVector<QCPCurve*> newCurve;

    int traceID = -1;


    for(std::vector<TargetObj>::iterator pTgt = targetList.begin() ; pTgt != targetList.end(); ++pTgt){


        for (int x=0; x<fieldWidth; x++) {
            for (int y=0; y<fieldHeight; y++) {
                planeMtrx[x][y]=((*pTgt).form.element(x,y,0));
            }
        }

        tgtContour = MooreTracing(planeMtrx,fieldWidth,fieldHeight,0.95);

        //Поиск контуров
        while(tgtContour.traceNewObj(!traceCavities))
        {
            curveX.clear();
            curveY.clear();
            xData.clear();
            yData.clear();
            tData.clear();

            curveX = tgtContour.getNewTraceX();
            curveY = tgtContour.getNewTraceY();
            curveLength=curveX.size();
            if(curveX.empty()) continue;
            if(clearObjLessThen > curveLength) continue;

            traceID++;

            newCurve.push_back(new QCPCurve(printArea->xAxis, printArea->yAxis));

            xData.resize(curveLength);
            yData.resize(curveLength);
            tData.resize(curveLength);

            for (int i=0; i<curveLength; i++)
            {
                xData[i] = curveX[i]; //+ 0.5
                yData[i] = curveY[i];// + 0.5;
                tData[i] = i;
            }

            newCurve[traceID]->setData(tData, xData, yData);
            newCurve[traceID]->setPen(PenStyle);
            newCurve[traceID] -> removeFromLegend();
        }
    }
    printArea->replot();
}

void MainWindow::setTargetList(){
    targetList.clear();

    int Nx =ui->targetNumX_edit->text().toInt();
    int Ny =ui->targetNumY_edit->text().toInt();

    double Rx =ui->targetRx_edit->text().toDouble();
    double Ry =ui->targetRy_edit->text().toDouble();

    double dX  = ui->gridStepX_edit  -> text().toDouble();
    double dY  = ui->gridStepY_edit  -> text().toDouble();
    double X0  = ui->gridStartX_edit -> text().toDouble();
    double Y0  = ui->gridStartY_edit -> text().toDouble();
    double deg  = ui->gridAngle_edit -> text().toDouble();
    double angle = 3.14*deg/180;
    //double tanA = tan(angle);
    double cosA = cos(angle);
    double sinA = sin(angle);

    std::vector<double> Xc;
    std::vector<double> Yc;

    TargetObj tgtObj;
    Target tgt;
    double Cx,Cy;
    double H =1;
    double Cz = 0;
    //TargetFieldQuality tfq;


    int colStart = ui->targetNameCol_edit->text().toInt();

    QString cS=ui->targetNameRow_edit->text();
    int rowStart = (int) (cS[0].unicode());

    QString rowName,colName,tgtName;
//
    for(int j=0;j<Ny;j++){
        for(int i=0;i<Nx;i++){
            tgt = Target(fieldWidth,fieldHeight,1);
            Cx = X0 + (i+0.5)*dX*cosA - (j+0.5)*dY*sinA;
            Cy = Y0 + (i+0.5)*dX*sinA + (j+0.5)*dY*cosA;
            tgt.setTube(Cx,Cy,Cz,Rx,Ry,H,"Z","add");


            rowName = (char)(j+rowStart);//65+
            colName = QString::number(i+1+colStart-1);
            tgtName=rowName+colName;

            tgtObj.name = tgtName;
            tgtObj.row = j;
            tgtObj.col = i;
            tgtObj.form = tgt;
            tgtObj.form.setTargetIndicesList();
            if (tgtObj.form.getTargetIndicesList().size()<1) continue;
            targetList.push_back(tgtObj);
        }
    }

    targetListIsSet = true;

    printDVH();
    printTargetStats();


    if(ui->targetOn_ChB->isChecked()) showTargets(isodoseArea);
}

void MainWindow::evalFieldQuality(){
    if(!targetListIsSet) return;

}

void MainWindow::printDVH(){
    if(!targetListIsSet) return;
    int tgtAmount = targetList.size();


    ui -> axisDVH ->clearGraphs();
    ui -> axisDVH -> legend ->clear();
    ui -> axisDVH -> setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator

    QPen LinePen;
    LinePen.setWidth(5);
    LinePen.setWidthF(3);
    QVector<QRgb> dvh_colorList;
    QCPColorGradient jetCM;
    jetCM.loadPreset(QCPColorGradient::gpJet);
    jetCM.setLevelCount(1000);
    double value;
    for (int i=0;i<tgtAmount;i++){
        value = (double)i/tgtAmount;
        dvh_colorList.push_back(jetCM.color(value,QCPRange(0, 1), false));
    }

    double axisXmin=0;
    double axisXmax=0;
    double axisYmin=0;
    double axisYmax=105;

    double axislXmin=0;
    double axislXmax=0;
    //    double axislYmin=0;
    //    double axislYmax=105;


    QString rowName, colName, tgtName;
    //std::vector <std::pair <double,double> > DVH;
    //DoseVolHyst DVH;
    //vectorPair DVH;
    //dblVector doseHyst,volumeHyst;
    intVector indList ;
    QVector<double> dose, volume;
    //   int size;

    int graphID = -1;

    for(std::vector<TargetObj>::iterator pTgt = targetList.begin() ; pTgt != targetList.end(); ++pTgt)
    {
//        dose.clear();
//        volume.clear();


  //      doseHyst.clear();volumeHyst.clear();

        indList.clear();
        indList = (*pTgt).form.getTargetIndicesList();

        tgtName=(*pTgt).name;
        analize::getDVH(indList, doseV, dose,volume);


//        for(unsigned int i = 0; i < doseHyst.size(); ++i){
//            dose.push_back( doseHyst[i]);
//            volume.push_back(volumeHyst[i]);
//        }

        graphID++;
        ui -> axisDVH -> addGraph();
        ui -> axisDVH -> graph(graphID)->setName(tgtName);
        // Построение гафика #M
        ui -> axisDVH -> graph(graphID)->setData(dose, volume);
        LinePen.setColor(dvh_colorList[graphID]);
        ui -> axisDVH -> graph(graphID)->setPen(LinePen);
        ui -> axisDVH -> graph(graphID)->setLineStyle(QCPGraph::lsStepLeft);  // Задание типа линии (lsStepCenter - ступенчатый)

        axislXmin = 0;//


        analize::getDoseMinimum(indList, doseV);
        axislXmax = 1.05*analize::getDoseAtVolume(1., dose,volume);

        axislXmin = 0.01*round(100*axislXmin);
        axislXmax = 0.01*round(100*axislXmax);

        if (axisXmin > axislXmin)
            axisXmin = axislXmin;
        axisXmin < 0 ? axisXmin = 0 : false;

        if (axisXmax < axislXmax)
            axisXmax = axislXmax;

    }
    ui ->dvh_Dmin_edit->setText(QString::number(axisXmin));
    ui ->dvh_Dmax_edit->setText(QString::number(axisXmax));

    //        axisXmin = ui ->dvh_Dmin_edit->text().toDouble();
    //        axisXmax = ui ->dvh_Dmax_edit->text().toDouble();

    ui -> axisDVH -> xAxis->setRange(axisXmin, axisXmax);
    ui -> axisDVH -> yAxis->setRange(axisYmin, axisYmax);

    ui -> axisDVH -> yAxis->ticker()->setTickCount(10);

    QString xAxlabel="Dose, Gy";
    QString yAxlabel="Volume, %";

    QFont labelFont = font();
    labelFont.setPointSize(14);
    ui -> axisDVH -> xAxis->setLabelFont(labelFont);
    ui -> axisDVH -> yAxis->setLabelFont(labelFont);

    ui -> axisDVH -> xAxis->setTickLabelFont(labelFont);
    ui -> axisDVH -> yAxis->setTickLabelFont(labelFont);

    ui -> axisDVH -> xAxis->setLabel(xAxlabel);
    ui -> axisDVH -> yAxis->setLabel(yAxlabel);

    ui -> axisDVH -> legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(14); // and make a bit smaller for legend
    ui -> axisDVH -> legend->setFont(legendFont);
    ui -> axisDVH -> legend->setBrush(QBrush(QColor(255,255,255,230)));

    QPoint tw;
    tw.setX(0);
    tw.setY(0);
    ui -> axisDVH-> axisRect()->insetLayout()->outerRect().setTopRight(tw);

    ui -> axisDVH -> replot();

}

void MainWindow::setDoseRange4DVH(){

    double axisXmin = ui ->dvh_Dmin_edit->text().toDouble();
    double axisXmax = ui ->dvh_Dmax_edit->text().toDouble();

    ui -> axisDVH -> xAxis->setRange(axisXmin, axisXmax);
    ui -> axisDVH -> yAxis->setRange(0, 105);
    ui -> axisDVH -> yAxis->ticker()->setTickCount(10);

    ui -> axisDVH -> replot();

}

void MainWindow::renameTargets(){
    if(!targetListIsSet) return;



    int colStart = ui->targetNameCol_edit->text().toInt();

    QString cS=ui->targetNameRow_edit->text();
    int rowStart = (int) (cS[0].unicode());

    QString  rowName, colName;


    int Nx =ui->targetNumX_edit->text().toInt();
    int Ny =ui->targetNumY_edit->text().toInt();
    std::vector<TargetObj>::iterator pTgt=targetList.begin();

    for(int j=0;j<Ny;j++){
        for(int i=0;i<Nx;i++){
            rowName = (char)(j+rowStart);//65+
            colName = QString::number(i+1+colStart-1);
            (*pTgt).name = rowName+colName;
            pTgt++;
        }
    }
    printDVH();
    printTargetStats();

}

void MainWindow::setEmtyRectangleTarget(){
    Target tgt;
//    double Cx = ui -> rectCx_edit -> text().toDouble();
//    double Cy = ui -> rectCy_edit -> text().toDouble();

//    double Width = ui -> rectWidth_edit -> text().toDouble();
//    double Heigth = ui -> rectHeigth_edit -> text().toDouble();

//    double H =1;
//    double Cz = 0;

    tgt = Target(fieldWidth,fieldHeight,1);

    rectArea.name = "Film";
    rectArea.row = 1;
    rectArea.col = 1;
    rectArea.form = tgt;
}

void MainWindow::addToRectangleTarget(){
    double Cx = ui -> rectCx_edit -> text().toDouble();
    double Cy = ui -> rectCy_edit -> text().toDouble();

    double Width = ui -> rectWidth_edit -> text().toDouble();
    double Heigth = ui -> rectHeigth_edit -> text().toDouble();

    double H =1;
    double Cz = 0;

    rectArea.form.setCube(Cx,Cy, Cz, Width, Heigth, H, "add");
    rectArea.form.setTargetIndicesList();
    targetList.clear();
    if (rectArea.form.getTargetIndicesList().size()<1) return;
    targetList.push_back(rectArea);
    showTargets(isodoseArea, Qt::white);
    printDVH();
    printTargetStats();
}

void MainWindow::remFromRectangleTarget(){
    double Cx = ui -> rectCx_edit -> text().toDouble();
    double Cy = ui -> rectCy_edit -> text().toDouble();

    double Width = ui -> rectWidth_edit -> text().toDouble();
    double Heigth = ui -> rectHeigth_edit -> text().toDouble();

    double H =1;
    double Cz = 0;

    if (!targetList.empty()) rectArea = targetList[0];

    rectArea.form.setCube(Cx,Cy, Cz, Width, Heigth, H, "remove");
    rectArea.form.setTargetIndicesList();
    targetList.clear();
    if (rectArea.form.getTargetIndicesList().size()<1) return;
    targetList.push_back(rectArea);
    showTargets(isodoseArea, Qt::white);
    printDVH();
    printTargetStats();
}

void MainWindow::clearRectangleTarget(){
    targetList.clear();
    setEmtyRectangleTarget();
    rectArea.form.setTargetIndicesList();
    targetList.clear();
    //if (rectArea.form.getTargetIndicesList().size()<1) return;

    targetList.push_back(rectArea);
    showTargets(isodoseArea, Qt::white);
    //printDVH();
    //printTargetStats();
}
//==================================================
///===============Работа с таблицей=================
//==================================================

void MainWindow::resizeTable(){

    int Nx =ui->targetNumX_edit->text().toInt();
    int Ny =ui->targetNumY_edit->text().toInt();

    int N = Nx*Ny;

    //    ui->results_table->setRowCount(Ny);
    //    ui->results_table->setColumnCount(Nx);

    //    ui->results_table->setShowGrid(true);

    //    QStringList col_names;
    //    QString col_name;
    //    for(int i=0;i<Nx;i++){
    //        col_name = (char)(65+i);
    //        col_names.push_back(col_name);
    //    }
    //    ui->results_table->setHorizontalHeaderLabels(col_names);

    ui->results_table->setRowCount(N);
    ui->results_table->setColumnCount(7);

    QStringList col_names;

    col_names.push_back("Target");
    col_names.push_back("Dmin, Gy");
    col_names.push_back("Dmean, Gy");
    col_names.push_back("Dmax, Gy");
    col_names.push_back("D50");
    col_names.push_back("HI, %");
    col_names.push_back("Integral, Gy");

    ui->results_table->setHorizontalHeaderLabels(col_names);

}

void MainWindow::printTargetStats(){
    if(!targetListIsSet) return;
    resizeTable();

    double value;
    QString item;

    QTableWidgetItem* cell;

    /*
    int row, col;

        int feature = ui->table_feature_cBox->currentIndex();
        switch (feature) {
        case 0: //Dmin
            for(std::vector<TargetObj>::iterator pTgt=targetList.begin(); pTgt!=targetList.end();++pTgt){
                value = analize::getDoseMinimum((*pTgt).form.getTargetIndicesList(), doseV);
                item = QString::number(0.01*round(100*value));
                cell = new QTableWidgetItem(item);
                cell->setTextAlignment(Qt::AlignCenter);
                ui->results_table->setItem((*pTgt).row, (*pTgt).col, cell);
            }
            break;
        case 1: //Dmax
            for(std::vector<TargetObj>::iterator pTgt=targetList.begin(); pTgt!=targetList.end();++pTgt){
                value = analize::getDoseMaximum((*pTgt).form.getTargetIndicesList(), doseV);
                item = QString::number(0.01*round(100*value));
                cell = new QTableWidgetItem(item);
                cell->setTextAlignment(Qt::AlignCenter);
                ui->results_table->setItem((*pTgt).row, (*pTgt).col, cell);
            }
            break;
        case 2: //Dmean
            for(std::vector<TargetObj>::iterator pTgt=targetList.begin(); pTgt!=targetList.end();++pTgt){
                value = analize::getMeanDose((*pTgt).form.getTargetIndicesList(), doseV);
                item = QString::number(0.01*round(100*value));

                value = analize::getDoseValDeviation((*pTgt).form.getTargetIndicesList(), doseV, value);
                item += ", \u03C3 = " + QString::number(0.01*round(100*value));

                cell = new QTableWidgetItem(item);
                cell->setTextAlignment(Qt::AlignCenter);
                ui->results_table->setItem((*pTgt).row, (*pTgt).col, cell);
            }
            break;
        case 3: //D50
            for(std::vector<TargetObj>::iterator pTgt=targetList.begin(); pTgt!=targetList.end();++pTgt){
                value = analize::getDoseAtVolume((*pTgt).form.getTargetIndicesList(), doseV, 50.0);
                item = QString::number(0.01*round(100*value));

                value = analize::getDoseValDeviation((*pTgt).form.getTargetIndicesList(), doseV, value);
                item += ", \u03C3 = " + QString::number(0.01*round(100*value));

                cell = new QTableWidgetItem(item);
                cell->setTextAlignment(Qt::AlignCenter);
                ui->results_table->setItem((*pTgt).row, (*pTgt).col, cell);
            }
            break;
        case 4: //HI
            for(std::vector<TargetObj>::iterator pTgt=targetList.begin(); pTgt!=targetList.end();++pTgt){
                value = analize::getDoseAtVolume((*pTgt).form.getTargetIndicesList(), doseV, 50.0);
                value = analize::getHomogeneityIndex((*pTgt).form.getTargetIndicesList(), doseV, value, "D");
                item = QString::number(0.01*round(100*value))+"%";

                cell = new QTableWidgetItem(item);
                cell->setTextAlignment(Qt::AlignCenter);
                ui->results_table->setItem((*pTgt).row, (*pTgt).col, cell);
            }
            break;
        default:
            ui->results_table->clear();
            break;
        }
*/

    double accuracy = 0.1;
    double revAcc = 10;// = 1/accuracy;
    //QTableWidgetItem* cell;

    int row = -1;
    for(std::vector<TargetObj>::iterator pTgt=targetList.begin(); pTgt!=targetList.end();++pTgt){
        row++;
        //Name
        item =  (*pTgt).name;
        cell = new QTableWidgetItem(item);
        cell->setTextAlignment(Qt::AlignCenter);
        ui->results_table->setItem(row, 0, cell);

        //Dmin
        value = analize::getDoseMinimum((*pTgt).form.getTargetIndicesList(), doseV);
        item = QString::number(accuracy*round(revAcc*value));
        cell = new QTableWidgetItem(item);
        cell->setTextAlignment(Qt::AlignCenter);
        ui->results_table->setItem(row, 1, cell);

        //Dmean
        value = analize::getMeanDose((*pTgt).form.getTargetIndicesList(), doseV);
        item = QString::number(accuracy*round(revAcc*value));
        value = analize::getDoseValDeviation((*pTgt).form.getTargetIndicesList(), doseV, value);
        item += ", \u03C3 = " + QString::number(0.1*accuracy*round(10*revAcc*value));
        cell = new QTableWidgetItem(item);
        cell->setTextAlignment(Qt::AlignCenter);
        ui->results_table->setItem(row, 2, cell);

        //Dmax
        value = analize::getDoseMaximum((*pTgt).form.getTargetIndicesList(), doseV);
        item = QString::number(accuracy*round(revAcc*value));
        cell = new QTableWidgetItem(item);
        cell->setTextAlignment(Qt::AlignCenter);
        ui->results_table->setItem(row, 3, cell);

        //D50
        value = analize::getDoseAtVolume((*pTgt).form.getTargetIndicesList(), doseV, 50.0);
        item = QString::number(accuracy*round(revAcc*value));
        value = analize::getDoseValDeviation((*pTgt).form.getTargetIndicesList(), doseV, value);
        item += ", \u03C3 = " + QString::number(0.1*accuracy*round(10*revAcc*value));
        cell = new QTableWidgetItem(item);
        cell->setTextAlignment(Qt::AlignCenter);
        ui->results_table->setItem(row, 4, cell);

        //HI%
        value = analize::getDoseAtVolume((*pTgt).form.getTargetIndicesList(), doseV, 50.0);
        value = analize::getHomogeneityIndex((*pTgt).form.getTargetIndicesList(), doseV, value, "D");
        item = QString::number(accuracy*round(revAcc*value));
        cell = new QTableWidgetItem(item);
        cell->setTextAlignment(Qt::AlignCenter);
        ui->results_table->setItem(row, 5, cell);


        //Integral dose
        value = analize::getIntegralDose((*pTgt).form.getTargetIndicesList(), doseV);
        item = QString::number(accuracy*round(revAcc*value));
        cell = new QTableWidgetItem(item);
        cell->setTextAlignment(Qt::AlignCenter);
        ui->results_table->setItem(row, 6, cell);


    }

}
/*
void MainWindow::getTableData(QStringList& cellDataSet){
    int Nrow = ui->results_table->rowCount();
    int Ncol = ui->results_table->columnCount();

    QTableWidgetItem* item;
    QString cellData;
    //QStringList cellDataSet;
    cellDataSet.clear();

    for(int row = 0; row < Nrow; row++){
        for(int col = 0; col < Ncol; col++){
            cellData = ui->results_table->takeItem(row,col)->text();
            cellDataSet.push_back(cellData);
        }
    }
}
*/

void MainWindow::saveTables(){

    QString locstr = ui -> statusbar->currentMessage();
    QString filter = "*.txt";
    QString fName = QFileDialog::getSaveFileName(0, "Save irradiation data", locstr, filter);

    if (fName.isEmpty()) return;
    else{
        QFile newFile(fName);
        newFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
        QTextStream newData(&newFile);
        //newData << "Target \t | \tDmin, Gy  \t | \tDmean, Gy  \t\t | \tDmax, Gy  \t | \tD50  \t\t | \tHI% | \tIntegral \n";
        newData << "Target\t | \tDmin, Gy\t | \tDmean, Gy\t | \tDmax, Gy\t | \tD50  \t | \tHI% | \tIntegral\n";

        int Nrow = ui->results_table->rowCount();
        int Ncol = ui->results_table->columnCount();

        QString cellData;
        for(int row = 0; row < Nrow; row++){
            for(int col = 0; col < Ncol-1; col++){
                cellData = ui->results_table->takeItem(row,col)->text();
                newData << cellData <<  "\t | \t";
            }
            cellData = ui->results_table->takeItem(row,Ncol-1)->text();
            newData << cellData <<  "\n";
        }

        /*
        double value;
        QString item;

        double accuracy = 0.1;
        double revAcc = 10;// = 1/accuracy;

        for(std::vector<TargetObj>::iterator pTgt=targetList.begin(); pTgt!=targetList.end();++pTgt){
            //Name
            newData << (*pTgt).name << "\t | \t";

            //Dmin
            value = analize::getDoseMinimum((*pTgt).form.getTargetIndicesList(), doseV);
            item = QString::number(accuracy*round(revAcc*value));
            newData << item <<  "\t | \t";

            //Dmean
            value = analize::getMeanDose((*pTgt).form.getTargetIndicesList(), doseV);
            item = QString::number(accuracy*round(revAcc*value));
            value = analize::getDoseValDeviation((*pTgt).form.getTargetIndicesList(), doseV, value);
            item += ", \u03C3 = " + QString::number(accuracy*round(revAcc*value));
            newData << item << "\t | \t";

            //Dmax
            value = analize::getDoseMaximum((*pTgt).form.getTargetIndicesList(), doseV);
            item = QString::number(accuracy*round(revAcc*value));
            newData << item << "\t | \t";

            //D50
            value = analize::getDoseAtVolume((*pTgt).form.getTargetIndicesList(), doseV, 50.0);
            item = QString::number(accuracy*round(revAcc*value));
            value = analize::getDoseValDeviation((*pTgt).form.getTargetIndicesList(), doseV, value);
            item += ", \u03C3 = " + QString::number(accuracy*round(revAcc*value));
            newData << item << "\t | \t";

            //HI%
            value = analize::getDoseAtVolume((*pTgt).form.getTargetIndicesList(), doseV, 50.0);
            value = analize::getHomogeneityIndex((*pTgt).form.getTargetIndicesList(), doseV, value, "D");
            item = QString::number(accuracy*round(revAcc*value))+"%";

            newData << item << "\n";
        }
*/
        newFile.close();

        printTargetStats();
    }

}


/*
void MainWindow::openTableInNewTab(){

    QString locstr = ui -> statusbar->currentMessage();
    QString filter = "tab.txt";
    QString fName = locstr + filter;

    if (fName.isEmpty()) return;
    else{
        QFile newFile(fName);
        newFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
        QTextStream newData(&newFile);
        newData << "Target \t | \tDmin  \t | \tDmean  \t\t | \tDmax  \t | \tD50  \t\t | \tHI%\n";

        int Nrow = ui->results_table->rowCount();
        int Ncol = ui->results_table->columnCount();

        QString cellData;
        for(int row = 0; row < Nrow; row++){
            for(int col = 0; col < Ncol-1; col++){
                cellData = ui->results_table->takeItem(row,col)->text();
                newData << cellData <<  "\t | \t";
            }
            cellData = ui->results_table->takeItem(row,Ncol-1)->text();
            newData << cellData <<  "\n";
        }
        newFile.close();
    }


}
*/

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

void MainWindow::printRevDoseFieldInFile(double lvl, std::vector<std::vector<double>> planeMtrx,QString fName){

    QFile newFile(fName);
    newFile.open(QIODevice::ReadWrite | QIODevice::Truncate | QIODevice::Text);
    QTextStream newData(&newFile);
    newData << "1" << '\t' << fieldWidth <<'\t' << fieldHeight << '\n';
    for (int j=0; j < fieldHeight; j++)
    {
        for (int i=0; i < fieldWidth; i++)
        {
            if(planeMtrx[i][j] < lvl){
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

void MainWindow::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
    QCustomPlot* axis = legend->parentPlot();
    // Rename a graph by double clicking on its legend item
    Q_UNUSED(legend)
    if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
    {
        QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
        bool ok;
        QString newName = QInputDialog::getText(this, "Reset name", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
        if (ok)
        {
            plItem->plottable()->setName(newName);
            axis->replot();
        }
    }
}

void MainWindow::showCursor(QMouseEvent *event)
{
    int mouseX = floor(isodoseArea->xAxis->pixelToCoord(event->pos().x()));
    int mouseY = floor(isodoseArea->yAxis->pixelToCoord(event->pos().y()));

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


    menu->popup(isodoseArea->mapToGlobal(event->pos()));



}

void MainWindow::selectionChanged(){

//    QCustomPlot* plotArea = ui->axisDVH;

//    // synchronize selection of graphs with selection of corresponding legend items:
//    for (int i=0; i<plotArea->graphCount(); ++i)
//    {
//      QCPGraph *graph = plotArea->graph(i);
//      QCPPlottableLegendItem *item = plotArea->legend->itemWithPlottable(graph);
//      if (item->selected() || graph->selected())
//      {

//        item->setSelected(true);
//        graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
//      }
//    }
}



void MainWindow::openGraph(){

    QWidget* tab = new QWidget();
    double scaling = 2.5;
    int marginWidth = 140;
    tab->setGeometry(0,0,scaling*fieldWidth+marginWidth+12,scaling*fieldHeight+12);
    tab->setWindowTitle("Dose distribution");

    tab->setMaximumSize(scaling*fieldWidth+marginWidth+12,scaling*fieldHeight+12);

    //tab->layout()->setSizeConstraint(QLayout::SetFixedSize);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::white);
    tab -> setAutoFillBackground(true);
    tab -> setPalette(pal);

    QGridLayout* boxLayout = new QGridLayout(tab);
    boxLayout->setGeometry(QRect(0,0,scaling*fieldWidth+marginWidth,scaling*fieldHeight));
    boxLayout->setContentsMargins(0,0,0,0);
    boxLayout->setSpacing(0);
    // boxLayout->setAlignment(Qt::AlignTop);

    QMenuBar* menuBar = new QMenuBar();
    QMenu* saveMenu = new QMenu("file");
    QAction* save_act = new QAction("Save", tab);

    QIcon saveIcon;
    saveIcon.addFile("ico/download.png");
    save_act->setIcon(saveIcon);
    QObject::connect(save_act, SIGNAL(triggered()), this, SLOT(saveTabIsodose()));

    menuBar->addMenu(saveMenu);
    saveMenu->addAction(save_act);
    tab->layout()->setMenuBar(saveMenu);
    menuBar->setNativeMenuBar(false);

    QSizePolicy spolicy;
    spolicy.Fixed;

    QCustomPlot* qcustomplot;
    qcustomplot = new QCustomPlot(tab);
    qcustomplot->setGeometry(0,0,scaling*fieldWidth+marginWidth,scaling*fieldHeight);
    qcustomplot->setMinimumSize(scaling*fieldWidth+marginWidth,scaling*fieldHeight);

    qcustomplot->setSizePolicy(spolicy);
    boxLayout->addWidget(qcustomplot);


    //    QSpacerItem* hSpacer = new QSpacerItem(1,1, QSizePolicy::Expanding, QSizePolicy::Fixed);
    //    QSpacerItem* vSpacer = new QSpacerItem(1,1, QSizePolicy::Fixed,     QSizePolicy::Expanding);
    //    boxLayout->addItem(hSpacer,0,1,2,2,Qt::AlignTop);
    //    boxLayout->addItem(vSpacer,1,1,2,2,Qt::AlignTop);

    //boxLayout->addItem(0,1)
    //    boxLayout->setSpacing(0);


    //addItem(qcustomplot);

    // qcustomplot->move(110,120);

    qcustomplot -> yAxis -> setRangeReversed(true);
    qcustomplot -> axisRect()->setBackground(QBrush(QColor(211,211,211,255)));//setBackground(Qt::gray);
    bool ticksOn  = false;
    qcustomplot -> xAxis ->setTickLabels(ticksOn);
    qcustomplot -> yAxis ->setTickLabels(ticksOn);

    qcustomplot -> setInteractions(QCP::iRangeDrag | QCP::iRangeZoom );
    connect(qcustomplot, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));




    qcustomplot->addLayer("image"      ,qcustomplot->layer("main")      , QCustomPlot::limAbove);
    qcustomplot->addLayer("CMAP"       ,qcustomplot->layer("image")  , QCustomPlot::limAbove);
    qcustomplot->addLayer("customGrid" ,qcustomplot->layer("CMAP")     , QCustomPlot::limAbove);
    qcustomplot->addLayer("probesArea" ,qcustomplot->layer("customGrid"), QCustomPlot::limAbove);
    qcustomplot->addLayer("isodoses"   ,qcustomplot->layer("probesArea"), QCustomPlot::limAbove);

    qcustomplot->axisRect()->setAutoMargins(QCP::msLeft | QCP::msTop | QCP::msBottom);
    qcustomplot->axisRect()->setMargins(QMargins(0,0,marginWidth,0));
    qcustomplot->axisRect()->insetLayout()->setInsetPlacement(0, QCPLayoutInset::ipFree);
    qcustomplot->axisRect()->insetLayout()->setInsetRect(0, QRectF(1.0,0.0,0.1,0.01));
    qcustomplot->axisRect()->insetLayout()->setMargins(QMargins(5,0,0,0));
    //


    if (currentMode == "isodoses"){
        buildIsodoses(qcustomplot);
    }else{
        buildIsoCMAP(qcustomplot);
    }


    qcustomplot ->xAxis->setRange(0,fieldWidth);
    qcustomplot ->yAxis->setRange(0,fieldHeight);

    qcustomplot->replot();

    isodoseTabArea = qcustomplot;






    tab->show();
}

void MainWindow::saveDVH(){
    savePicture(ui->axisDVH);
}

void MainWindow::saveIsodose(){
    savePicture(ui->axis4isodoses);
}

void MainWindow::saveTabIsodose(){
    savePicture(isodoseTabArea);
}

void MainWindow::savePicture(QCustomPlot* printArea)
{
    QString locstr = ui -> statusbar->currentMessage();
    QString filter = "*.jpg";

    QString filename = QFileDialog::getSaveFileName(0, "Save graph as", locstr, filter);

    QFile file(filename);

    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << file.errorString();
    } else {
        printArea->saveJpg(filename);
    }
}

//==================================================
///===========   Вращение матрицы    ===============
//==================================================

void MainWindow::rotate90degLeft(){
    matrixRotation::rotate90degLeft(doseVector, fieldWidth,fieldHeight);
    doseV.setAs(doseVector,fieldWidth,fieldHeight,1);
    buildIsodoses(isodoseArea);
}
void MainWindow::rotate90degRight(){
    matrixRotation::rotate90degRight(doseVector, fieldWidth,fieldHeight);
    doseV.setAs(doseVector,fieldWidth,fieldHeight,1);
    buildIsodoses(isodoseArea);
}

void MainWindow::flipHorizontally(){
    matrixRotation::flipHorizontally(doseVector, fieldWidth,fieldHeight);
    doseV.setAs(doseVector,fieldWidth,fieldHeight,1);
    buildIsodoses(isodoseArea);
}
void MainWindow::flipVertically  (){
    matrixRotation::flipVertically(doseVector, fieldWidth,fieldHeight);
    doseV.setAs(doseVector,fieldWidth,fieldHeight,1);
    buildIsodoses(isodoseArea);
}
