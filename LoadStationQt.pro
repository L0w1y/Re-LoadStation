QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    conv_temp.cpp \
    converter.cpp \
    converter_new.cpp \
    extractor.cpp \
    extractor_new.cpp \
    loader.cpp \
    main.cpp \
    widget.cpp \
#    despencives/jsoncpp.cpp \
 \#    despencives/json/json.h
    worker.cpp

HEADERS += \
    converter.h \
    converter_new.h \
    extractor.h \
    extractor_new.h \
    loader.h \
    widget.h \
    worker.h

FORMS += \
    widget.ui

TRANSLATIONS += \
    LoadStationQt_ru_RU.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
