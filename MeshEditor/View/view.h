#pragma once

#include <QQuickFramebufferObject>
#include <QtQuick>
#include "model.h"
#include "Controller/controllerdispatcher.h"
#include "viewport.h"
#include "contact.h"
#include "Controller/filtervalue.h"
#include "nodeoctree.h"
#include "Controller/manipulator.h"
#include "Controller/transformnodecontroller.h"

#include<QObject>
#include <QVector3D>

#include "modelstore.h"
#include "selectionbuffer.h"

class ViewRenderer;

class View : public QQuickFramebufferObject
{
	Q_OBJECT
    //Q_PROPERTY(void nodes READ nodes NOTIFY modelsChanged)
public:
      void addNodeInfo();
//void addPointLight();

	explicit View(QQuickItem* parent = 0);
	Renderer* createRenderer() const override;

	virtual void keyPressEvent(QKeyEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void wheelEvent(QWheelEvent* event) override;

	void addController(KeyCode enterKey, KeyCode exitKey, std::unique_ptr<Controller> controller);
	void addController(ButtonCode button, std::unique_ptr<Controller> controller);
	void addController(KeyCode key, std::unique_ptr<Controller> controller);

	template<class Lambda>
	void addController(KeyCode key, Lambda lambda)
	{
		controllerDispatcher.addController(key, lambda);
	}
	std::vector<Contact> raycast(double x, double y, FilterValue filterValues);
	Viewport& getViewport();
	const Viewport& getViewport() const;
	Model* modelPtr;
	GLRenderEngine* getRE() { return renderEngine; }
	GLRenderEngine* renderEngine;
	NodeOctree* octreePtr;

	void setOctree(NodeOctree* nodeOctree);
	NodeOctree* getOctree() const;

	void setManipulator(std::unique_ptr<Manipulator> inManipulator);
	Manipulator* getManipulator();
	void resetManipulator();
	std::unique_ptr<Manipulator> manipulatorPtr;
	bool needToResetManipulator = false;
	bool needToLoadModel = false;
	ViewRenderer* renderer;
	ModelTab* modelTab;
	// ModelStorage* modelStorage;
    bool isMouseTrackingEnabled = true;
public slots:
	void loadModel(QString path);
    void saveModel(QString path);
    void addPointLight();
    void removePointLight(int index);
	void removeModel(int index);
	void runTransformOperator(Node* node);
signals:
	void startAddNodeInfoAction();
	void completeLoadModel(Node* node);

private:
	enum class MouseState { Release, Drag, Press };
	MouseState prevMouseState = MouseState::Release;
	
	void mouseTracking(MouseState state, double x, double y);

	
	// std::unique_ptr<Manipulator> manipulatorPtr;
	ControllerDispatcher controllerDispatcher;
	Viewport viewport;

	std::vector<Contact> broadPhase(const ray& cursorRay, const Model& model);

	std::vector<Contact> narrowPhase(ray cursorRay, FilterValue filterValues, std::vector<Contact>& candidateNode);

	bool rayIntersectFace(const ray& rayToIntersect, const HalfEdge::FaceHandle& fh, const Node& node, const Mesh& mesh, double& distToIntersectedFace);
	bool rayIntersectTriangle(ray ray, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, double& distToIntersectedTriangle);
};
