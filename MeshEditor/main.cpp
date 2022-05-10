#include <QApplication>
#include <QGuiApplication>
#include <QQuickView>
#include <QQmlEngine>
#include <QSurfaceFormat>
#include "View/QMLContext.h"
#include "View/view.h"
QQmlContext* s_context;
int main(int argc, char** argv)
{
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
	QApplication app(argc, argv);

	QSurfaceFormat format;
	format.setMajorVersion(3);
	format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);//QSurfaceFormat::CoreProfile
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4);
	QSurfaceFormat::setDefaultFormat(format);
    QCoreApplication::setOrganizationName("Some organization");

	qmlRegisterType<View>("MyRenderLibrary", 1, 0, "GLRenderSystem");
    //app->setC

	QQuickView view;
    s_context=view.rootContext();
    //ModelTab* modelTab=new ModelTab;

    //view.rootContext()->setContextProperty("ModelTab", modelTab);
	view.setResizeMode(QQuickView::SizeRootObjectToView);
	view.setSource(QUrl("qrc:///qml/main.qml"));
    view.setTitle("PerformanceEditor");
    app.setWindowIcon(QIcon("C:/Users/Stepan/Documents/workplace/PerformanceEditor/MeshEditor/icons/magic-wand.png"));
	view.resize(1366, 768);
	view.show();

	return app.exec();
}
