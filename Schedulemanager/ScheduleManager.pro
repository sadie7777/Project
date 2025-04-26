QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# 소스 파일
SOURCES += \
    main.cpp \
    mainwindow.cpp \
    customcalendar.cpp \
    eventdialog.cpp

# 헤더 파일
HEADERS += \
    mainwindow.h \
    customcalendar.h \
    eventdialog.h \
    event.h

# UI 파일
FORMS += \
    mainwindow.ui \
    eventdialog.ui

# 리소스 파일
RESOURCES += \
    arrow.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=
