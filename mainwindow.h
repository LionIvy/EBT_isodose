#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

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
    void buildIsodoses();
    void buildIsoCMAP();


    //Grid related
    void copyGridXStep2YStep(QString txt);
    void changeGridChBox(int boxValue);
    void updateGrid();

    void showCursor(QMouseEvent *event);



private:
    Ui::MainWindow *ui;


    // App data
    QString rootFolder;
    QString shortcutFolder;
    QString ebtFolder;
    QString imageFolder;

    // Window setup


    void loadTIFF(QString filename);


    std::vector<double> doseVector;
    int fieldWidth = 0;
    int fieldHeight = 0;
    inline int doseIndex(int X, int Y){return (fieldWidth*Y + X);}
    bool doseVectorIsSet;


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


    void setCustomGrid();

  //==================================================
  ///===========Настройки окна приложения=============
  //==================================================
  //
    void setLineValidators();


    void printDoseFieldInFile(double lvl, std::vector<std::vector<double>> planeMtrx,QString fName);




};
#endif // MAINWINDOW_H
