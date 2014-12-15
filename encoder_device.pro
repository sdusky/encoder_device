#-------------------------------------------------
#
# Project created by QtCreator 2014-12-03T10:42:11
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = encoder_device
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    encoder_alarm.cpp \
    encoder_encoder.cpp \
    encoder_information.cpp \
    encoder_sensor.cpp \
    encoder_settings.cpp \
    encoder_tcpsocket.cpp \
    encoder_udpsocket.cpp \
    encoder_v4l2.cpp \
    MFC_API/SsbSipMfcEncAPI.c

HEADERS += \
    encoder_alarm.h \
    encoder_encoder.h \
    encoder_information.h \
    encoder_package.h \
    encoder_sensor.h \
    encoder_settings.h \
    encoder_tcpsocket.h \
    encoder_udpsocket.h \
    encoder_v4l2.h \
    MFC_API/mfc_interface.h \
    MFC_API/SsbSipMfcApi.h
