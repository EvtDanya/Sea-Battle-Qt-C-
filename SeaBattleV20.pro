QT += core gui network multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cell.cpp \
    fleet.cpp \
    gamealgorythm.cpp \
    hellowindow.cpp \
    localgamewindow.cpp \
    main.cpp \
    mainwindow.cpp \
    presets.cpp \
    sfmlwidget.cpp \
    ship.cpp \
    shipsplacing.cpp \
    sound.cpp \
    statistics.cpp \
    statwindow.cpp

HEADERS += \
    cell.h \
    fleet.h \
    gamealgorythm.h \
    hellowindow.h \
    localgamewindow.h \
    mainwindow.h \
    sfmlwidget.h \
    ship.h \
    shipsplacing.h \
    sound.h \
    statistics.h \
    statwindow.h

FORMS += \
    hellowindow.ui \
    localgamewindow.ui \
    mainwindow.ui \
    shipsplacing.ui \
    soundsliderwindow.ui \
    statwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

INCLUDEPATH += C:\Users\fomin\Desktop\coding\qt\SFML_for_Qt\SFML\include
DEPENDPATH += C:\Users\fomin\Desktop\coding\qt\SFML_for_Qt\SFML\include

LIBS += -LC:\Users\fomin\Desktop\coding\qt\SFML_for_Qt\LIBS

CONFIG(release, debug|release): LIBS += -lsfml-audio -lsfml-graphics -lsfml-main -lsfml-network -lsfml-window -lsfml-system
CONFIG(debug, debug|release): LIBS += -lsfml-audio-d -lsfml-graphics-d -lsfml-main-d -lsfml-network-d -lsfml-window-d -lsfml-system-d
RC_ICONS = images\helm.ico
