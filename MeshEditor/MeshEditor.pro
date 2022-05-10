QT += quick core gui widgets
CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        Controller/controllerdispatcher.cpp \
        Controller/manipulator.cpp \
        Controller/orbit.cpp \
        Controller/pan.cpp \
        Controller/pointlight.cpp \
        Controller/rotationmanipulator.cpp \
        Controller/scalemanipulator.cpp \
        Controller/transformnodecontroller.cpp \
        Controller/translationmanipulator.cpp \
        Controller/triad.cpp \
        Controller/zoom.cpp \
        View/camera.cpp \
        View/modelstore.cpp \
        View/selectionbuffer.cpp \
        View/view.cpp \
        View/viewport.cpp \
        main.cpp

RESOURCES += qml.qrc \
    assets.qrc

INCLUDEPATH += $$PWD/ThirdParty/glm/glm

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    Controller/controller.h \
    Controller/controllerdispatcher.h \
    Controller/filtervalue.h \
    Controller/manipulator.h \
    Controller/orbit.h \
    Controller/pan.h \
    Controller/pointlight.h \
    Controller/rotationmanipulator.h \
    Controller/scalemanipulator.h \
    Controller/transformnodecontroller.h \
    Controller/translationmanipulator.h \
    Controller/triad.h \
    Controller/zoom.h \
    View/QMLContext.h \
    View/camera.h \
    View/modelstore.h \
    View/selectionbuffer.h \
    View/view.h \
    View/viewport.h

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../GLRenderEngine/release/ -lGLRenderEngine
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../GLRenderEngine/debug/ -lGLRenderEngine
else:unix: LIBS += -L$$OUT_PWD/../GLRenderEngine/ -lGLRenderEngine

INCLUDEPATH += $$PWD/../GLRenderEngine
DEPENDPATH += $$PWD/../GLRenderEngine

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GLRenderEngine/release/libGLRenderEngine.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GLRenderEngine/debug/libGLRenderEngine.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GLRenderEngine/release/GLRenderEngine.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../GLRenderEngine/debug/GLRenderEngine.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../GLRenderEngine/libGLRenderEngine.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ModelTab/release/ -lModelTab
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ModelTab/debug/ -lModelTab
else:unix: LIBS += -L$$OUT_PWD/../ModelTab/ -lModelTab

INCLUDEPATH += $$PWD/../ModelTab
DEPENDPATH += $$PWD/../ModelTab

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ModelTab/release/libModelTab.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ModelTab/debug/libModelTab.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ModelTab/release/ModelTab.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ModelTab/debug/ModelTab.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../ModelTab/libModelTab.a

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../ModelLoader/release/ -lModelLoader
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../ModelLoader/debug/ -lModelLoader

INCLUDEPATH += $$PWD/../ModelLoader
DEPENDPATH += $$PWD/../ModelLoader

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ModelLoader/release/libModelLoader.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ModelLoader/debug/libModelLoader.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ModelLoader/release/ModelLoader.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../ModelLoader/debug/ModelLoader.lib

INCLUDEPATH += $$PWD/ThirdParty/assimp/include
DEPENDPATH += $$PWD/ThirdParty/assimp/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ThirdParty/assimp/lib/ -lassimp-vc142-mt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ThirdParty/assimp/lib/ -lassimp-vc142-mtd

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/ThirdParty/assimp/lib/libassimp-vc142-mt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/ThirdParty/assimp/lib/libassimp-vc142-mtd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/ThirdParty/assimp/lib/assimp-vc142-mt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/ThirdParty/assimp/lib/assimp-vc142-mtd.lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ThirdParty/assimp/lib/ -lIrrXML
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ThirdParty/assimp/lib/ -lIrrXMLd

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/ThirdParty/assimp/lib/libIrrXML.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/ThirdParty/assimp/lib/libIrrXMLd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/ThirdParty/assimp/lib/IrrXML.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/ThirdParty/assimp/lib/IrrXMLd.lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ThirdParty/assimp/lib/ -lzlib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ThirdParty/assimp/lib/ -lzlibd

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/ThirdParty/assimp/lib/libzlib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/ThirdParty/assimp/lib/libzlibd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/ThirdParty/assimp/lib/zlib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/ThirdParty/assimp/lib/zlibd.lib

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ThirdParty/assimp/lib/ -lzlibstatic
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ThirdParty/assimp/lib/ -lzlibstaticd

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/ThirdParty/assimp/lib/libzlibstatic.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/ThirdParty/assimp/lib/libzlibstaticd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/ThirdParty/assimp/lib/zlibstatic.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/ThirdParty/assimp/lib/zlibstaticd.lib
