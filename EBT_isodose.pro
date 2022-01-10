QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


#D:\programs\Qt\5.14.1\Src\qtimageformats\src\3rdparty\libtiff
#include (/home/ivan/programs/QT/5.14.1/Src/qtimageformats/src/plugins/imageformats/tiff/tiff.pro)
#INCLUDEPATH += D:/programs/Qt/5.14.1/Src/qtimageformats/src/3rdparty/libtiff/libtiff

#SUBDIRS += \
#    ../../../programs/Qt/5.14.1/Src/qtimageformats/src/plugins/imageformats/tiff/tiff.pro \
#    ../../../programs/Qt/5.14.1/Src/qtimageformats/src/src.pro



SOURCES += \
    DoseFieldAnalize.cpp \
    DoseVector.cpp \
    FieldQuality.cpp \
    MatrixRotation.cpp \
    Target.cpp \
    main.cpp \
    mainwindow.cpp \
    mooretracing.cpp \
    qcustomplot.cpp \
    test.cpp

HEADERS += \
    DoseFieldAnalize.h \
    DoseVector.h \
    FieldQuality.h \
    MatrixRotation.h \
    Target.h \
    mainwindow.h \
    mooretracing.h \
    qcustomplot.h \
    test.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target



unix:!macx: LIBS += -ltiff

RESOURCES += \
    Resource.qrc

