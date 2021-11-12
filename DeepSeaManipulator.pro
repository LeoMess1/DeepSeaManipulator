TEMPLATE = app
CONFIG += c++11

INCLUDEPATH += \
    D:\MVS\Development\Includes
INCLUDEPATH += \
    $$quote(E:\VS2019 WorkSpace\Work\DeepSeaController\public)

#TARGET = adueditor
QT += serialbus widgets
QT += serialbus-private core-private
requires(qtConfig(combobox))
requires(qtConfig(modbus-serialport))

LIBS += \
    -LD:\MVS\Development\Libraries\win64 -lMvCameraControl
LIBS += \
    -L$$quote(E:\VS2019 WorkSpace\Work\DeepSeaController\Debug) -lDeepSeaController

SOURCES += \
    MvCamera.cpp \
    main.cpp \
    mainwindow.cpp \
    modbustcpclient.cpp \
    mycamera.cpp \
    mycontroller.cpp \
    mymodbus.cpp

HEADERS += \
    MvCamera.h \
    mainwindow.h  \
    modbustcpclient.h \
    modbustcpclient_p.h \
    mycamera.h \
    mycontroller.h \
    mymodbus.h \
    plaintextedit.h

FORMS += \
    interface.ui

#target.path = $$[QT_INSTALL_EXAMPLES]/serialbus/modbus/adueditor
INSTALLS += target

RESOURCES += \
    res.qrc
