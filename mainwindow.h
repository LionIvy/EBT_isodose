#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"
#include "DoseVector.h"
#include "Target.h"
#include "DoseFieldAnalize.h"
#include "MatrixRotation.h"

//#include "FieldQuality.h"
//#include "TargetFieldQuality.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


typedef std::vector<double> dblVector;

struct TargetObj{
    QString name;
    int row;
    int col;
    Target form;
};



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:

    //Isodose related
    void loadEBTfile();
    void setIsodoseLines();
    void pushIsodoseButton();

    void pushIsoCMAP();



    //Grid related
    void copyGridXStep2YStep(QString txt);
    void changeGridChBox(int boxValue);
    void updateGrid();

<<<<<<< HEAD

=======
>>>>>>> targets
    void showCursor(QMouseEvent *event);
    void selectionChanged();

    void updateTargetGrid();
    void changeTargetChBox(int boxValue);
    void copyGridXStep2Radii(QString txt);
    void copyRxVal2Ry(QString txt);

    void setTargetList();
    //void setRectangleTarget();
    void setEmtyRectangleTarget();
    void addToRectangleTarget();
    void remFromRectangleTarget();
    void clearRectangleTarget();
    void printDVH();
    void setDoseRange4DVH();
    void renameTargets();



    void resizeTable();
    void printTargetStats();

    void openGraph();

    void legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item);

    void saveTables();

    void saveDVH();
    void saveIsodose();
    void saveTabIsodose();


    void rotate90degLeft();
    void rotate90degRight();

    void flipHorizontally();
    void flipVertically  ();

private:
    const double colorModifier = 0.95;
    Ui::MainWindow *ui;


    // App data
    QString rootFolder;
    QString shortcutFolder;
    QString ebtFolder;
    QString imageFolder;
    QCustomPlot* isodoseArea;

    //bool isodoseTabExist = false;
    QCustomPlot* isodoseTabArea;

    // Window setup


    void loadTIFF(QString filename);

    QString currentMode = "isodoses";//CMAP
    void buildIsodoses(QCustomPlot* printArea);
    void buildIsoCMAP(QCustomPlot* printArea);

    std::vector<double> doseVector;
    int fieldWidth = 0;
    int fieldHeight = 0;
    inline int doseIndex(int X, int Y){return (fieldWidth*Y + X);}

    bool doseVectorIsSet;
    DoseVector doseV;

    std::vector<double> isodoses;
    std::vector<int> curveX;
    std::vector<int> curveY;
    bool isodosesAreSet;
    void search4Value(QString strLines, int &startPoint, int &finPoint, const int stringSize);
    bool addToIsodoseList(QString strLines, int startPoint,int finPoint);
    bool addRangeToIsodoseList(QString strLines, int startPoint,int &finPoint, const int stringSize);
    bool dosePrintLock = false;


    QVector<QRgb> colorList;
    void setLinesColorStyle();


    void setCustomGrid(QCustomPlot* printArea);

    std::vector<TargetObj> targetList;
    TargetObj rectArea;
    //std::vector<Target> targetList;
    //std::vector<FieldQuality> targetQualityList;

    QColor lastTargetColor=Qt::black;
    void showTargets(QCustomPlot* printArea, QColor tgtColor=Qt::black);
    bool targetListIsSet;// = false;

    void evalFieldQuality();




  //==================================================
  ///===========Настройки окна приложения=============
  //==================================================
  //
    void setLineValidators();


    void printDoseFieldInFile(double lvl, std::vector<std::vector<double>> planeMtrx,QString fName);
    void printRevDoseFieldInFile(double lvl, std::vector<std::vector<double>> planeMtrx,QString fName);

    void savePicture(QCustomPlot* printArea);


};
#endif // MAINWINDOW_H
