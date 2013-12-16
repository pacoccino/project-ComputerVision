TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_features2d

SOURCES += src/main.cpp

