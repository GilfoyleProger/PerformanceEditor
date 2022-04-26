QT = quick

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bboxhelper.cpp \
    buildplate.cpp \
    faceoctree.cpp \
    halfedgetable.cpp \
    light.cpp \
    mesh.cpp \
    model.cpp \
    node.cpp \
    nodeoctree.cpp

HEADERS += \
    bboxhelper.h \
    buildplate.h \
    contact.h \
    faceoctree.h \
    halfedgetable.h \
    light.h \
    mesh.h \
    model.h \
    node.h \
    nodeoctree.h \
    octree.h

INCLUDEPATH += $$PWD/ThirdParty/glm/glm

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target

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
