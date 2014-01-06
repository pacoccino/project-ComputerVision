TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_features2d -lopencv_video

SOURCES += src/main.cpp \
    src/PixelClassifier.cpp \
    src/Tools.cpp

HEADERS += \
    src/PixelClassifier.h \
    src/Tools.h

