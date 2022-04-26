QT -= gui
QT += quick

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

INCLUDEPATH += $$PWD/ThirdParty/glm/glm
INCLUDEPATH += $$PWD/ThirdParty/stb

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    modelloader.cpp

HEADERS += \
    modelloader.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

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
