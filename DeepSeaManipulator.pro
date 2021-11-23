TEMPLATE = app
CONFIG += c++11

INCLUDEPATH += \
    D:\MVS\Development\Includes
INCLUDEPATH += \
    $$quote(E:\Qt Workspace\Work\DeepSeaManipulator\includes)

#TARGET = adueditor
QT += serialbus widgets
#QT += serialbus-private core-private
requires(qtConfig(combobox))
#requires(qtConfig(modbus-serialport))

LIBS += \
    -LD:\MVS\Development\Libraries\win64 -lMvCameraControl
LIBS += \
    -L$$quote(E:\Qt Workspace\Work\DeepSeaManipulator\lib) -lDeepSeaController

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
