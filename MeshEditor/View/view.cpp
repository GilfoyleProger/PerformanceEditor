#include "view.h"

#include <QQuickWindow>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFramebufferObjectFormat>
#include <memory>

#include "Controller/controllerdispatcher.h"
#include "Controller/orbit.h"
#include "Controller/zoom.h"
#include "Controller/pan.h"
#include "glrenderengine.h"

#include "buildplate.h"
#include "Controller/addroadtilecontroller.h"
#include "modelloader.h"
#include <glm/gtx/string_cast.hpp>
#include "nodeoctree.h"
#include "Controller/pointlight.h"
#include "QMLContext.h"


class ViewRenderer : public QQuickFramebufferObject::Renderer
{
public:
	QOpenGLContext* getContext()
	{
		return QOpenGLContext::currentContext();
	}

	ViewRenderer()
    {
		renderEngine = std::make_unique<GLRenderEngine>();
		renderEngine->init();

		//std::string vertexShaderPath = QDir::currentPath().toStdString() + "/shaders/object.vs";
		//std::string fragmentShaderPath = QDir::currentPath().toStdString() + "/shaders/object.fs";
		std::string vertexShaderPath = "C:/Users/Stepan/Documents/workspace/PerformanceEditor/MeshEditor/shaders/object.vert";
		std::string fragmentShaderPath = "C:/Users/Stepan/Documents/workspace/PerformanceEditor/MeshEditor/shaders/object.frag";
		shaderProgram = std::unique_ptr<ShaderProgram>(renderEngine->createShaderProgram(vertexShaderPath, fragmentShaderPath));

		vertexShaderPath = "C:/Users/Stepan/Documents/workspace/PerformanceEditor/MeshEditor/shaders/manipulator.vert";
		fragmentShaderPath = "C:/Users/Stepan/Documents/workspace/PerformanceEditor/MeshEditor/shaders/manipulator.frag";
		manipulatorShader = std::unique_ptr<ShaderProgram>(renderEngine->createShaderProgram(vertexShaderPath, fragmentShaderPath));

		vertexShaderPath = "C:/Users/Stepan/Documents/workspace/PerformanceEditor/MeshEditor/shaders/light.vert";
		fragmentShaderPath = "C:/Users/Stepan/Documents/workspace/PerformanceEditor/MeshEditor/shaders/light.frag";
		lightShader = std::unique_ptr<ShaderProgram>(renderEngine->createShaderProgram(vertexShaderPath, fragmentShaderPath));

		modelPtr = std::make_unique<Model>();
		loader = new ModelLoader(renderEngine.get());

        //std::unique_ptr<Node> rootNode = loader->loadModel("C:/Users/Stepan/Desktop/textured-cube/cube.obj");
        std::unique_ptr<Node> rootNode = loader->loadModel("C:/Users/Stepan/Desktop/red-cube.obj");
		if (rootNode) {
			rootNode->setName("Solid");
			rootNode->getChildren().back()->setName("Solid");


			modelPtr->attachNode(std::move(rootNode));
		}

        std::unique_ptr<PointLight> pointLight = std::make_unique<PointLight>();
        pointLight->ambient() = glm::vec3(0.2, 0.2, 0.2);
        pointLight->diffuse() = glm::vec3(0.5f, 0.5f, 0.5f);
        pointLight->specular() = glm::vec3(1.0, 1.0, 1.0);

        pointLight->setAmbientEnabled(true);
        pointLight->setDiffuseEnabled(true);
        pointLight->setSpecularEnabled(true);

        pointLight->setPos({ 4,0,0 });
        pointLight->setName("Light");



		if (modelPtr != nullptr)
		{
			std::vector<Node*> nodeList = BboxHelper::getNodeList(*modelPtr);
			bbox box = BboxHelper::calcNodesBoundingBox(nodeList);
			octreePtr = std::make_unique<NodeOctree>(box);

			for (const auto& node : nodeList)
				octreePtr->addNode(node);

            octreePtr->addNode(pointLight.get());
        }

        pointLights.emplace_back(std::move(pointLight));
	}

	void synchronize(QQuickFramebufferObject* item) override
	{
		quickWindowPtr = item->window();
        viewPtr = static_cast<View*>(item);
        viewPtr->getViewport().setViewportSize(viewPtr->width(), viewPtr->height());
		viewPtr->modelPtr = modelPtr.get();
		viewPtr->renderEngine = renderEngine.get();
		viewPtr->octreePtr = octreePtr.get();
//viewPtr->forceActiveFocus();
        if (needToInitModel)
        {
			viewPtr->modelTab->setModel(modelPtr.get());
            needToInitModel = false;
            emit viewPtr->startAddNodeInfoAction();
        }

        if (viewPtr->needToResetManipulator)
		{
			viewPtr->manipulatorPtr.reset();
        }

        if (viewPtr->needToLoadModel)
		{
            std::unique_ptr<Node> rootNode = loader->loadModel(currentModelPathToRead);
			if (rootNode) {
				modelPtr->attachNode(std::move(rootNode));
				viewPtr->needToLoadModel = false;

				std::vector<Node*> nodeList = BboxHelper::getNodeList(modelPtr->getNodes().back().get());
				for (const auto& node : nodeList)
					octreePtr->addNode(node);

				emit viewPtr->startAddNodeInfoAction();
			}
        }

        viewPtr->renderer = this;
	}

	void render() override
	{
		viewPtr->modelPtr = modelPtr.get();
		viewPtr->renderEngine = renderEngine.get();
        renderEngine->setViewport(0, 0, viewPtr->width(), viewPtr->height());
        //renderEngine->clearDisplay(0.2f, 0.2f, 0.2f);
        renderEngine->clearDisplay(0.0f, 0.0f, 0.0f);

		renderEngine->setProjMatrix(glmMat4ToQMatrix4x4(viewPtr->getViewport().calcProjectionMatrix()));
		renderEngine->setViewMatrix(glmMat4ToQMatrix4x4(viewPtr->getViewport().getCamera().calcViewMatrix()));

		if (modelPtr != nullptr)
			updateNodes(modelPtr->getNodes());
		updateLight();
		renderEngine->clearDepthBuffer();
		updateManipulator();

		quickWindowPtr->resetOpenGLState();
		update();
	}

	QOpenGLFramebufferObject* createFramebufferObject(const QSize& size) override
	{
		QOpenGLFramebufferObjectFormat format;
		format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
		format.setSamples(4);
		return new QOpenGLFramebufferObject(size, format);
	}


    std::vector<std::unique_ptr<PointLight>>& getPointLights()
    {
        return pointLights;
    }

    std::string currentModelPathToRead;

private:
    std::vector<std::unique_ptr<PointLight>> pointLights;
    std::unique_ptr<Model> modelPtr;
    bool needToInitModel = true;
    ModelLoader* loader;
	std::unique_ptr<GLRenderEngine> renderEngine;
	std::unique_ptr<ShaderProgram> shaderProgram;

	QQuickWindow* quickWindowPtr;
	std::unique_ptr<NodeOctree> octreePtr;
	View* viewPtr;

	std::unique_ptr<ShaderProgram> manipulatorShader;
	std::unique_ptr<ShaderProgram> lightShader;
	
	void updateManipulator()
	{
		if (!viewPtr->needToResetManipulator)
		{
			if (viewPtr->manipulatorPtr)
			{
				if (!viewPtr->manipulatorPtr->getMeshes().empty())
				{
					renderEngine->setWorldMatrix(glmMat4ToQMatrix4x4(viewPtr->manipulatorPtr->calcAbsoluteTransform()));
					for (auto& mesh : viewPtr->manipulatorPtr->getMeshes())
					{
						mesh->render(renderEngine.get(), manipulatorShader.get());
					}
				}

				if (!viewPtr->manipulatorPtr->getChildren().empty())
					updateNodes(viewPtr->manipulatorPtr->getChildren());
			}
		}
	}
	
	void updateLight()
	{
        for(auto& pointLight : pointLights){
        renderEngine->setWorldMatrix(glmMat4ToQMatrix4x4(pointLight->calcAbsoluteTransform()));
        for (auto& mesh : pointLight->getMeshes())
		{
			lightShader->bind();
			glm::vec3 color = glm::vec3(1.0f);
			mesh->setShaderUniform("inColor", { color.x, color.y, color.z });
            NodeInfo* lightInfo = viewPtr->modelTab->getNodeInfo(pointLight.get());
            mesh->setShaderUniform("isSelected", lightInfo ? lightInfo->selected() : false);
            if(pointLight->visible())
			mesh->render(renderEngine.get(), lightShader.get());
		}
        }
	}

	QMatrix4x4 glmMat4ToQMatrix4x4(glm::mat4 matrix)
	{
		glm::mat4 glmMatrix = glm::transpose(matrix);
		QMatrix4x4 result;
		result.setRow(0, QVector4D(glmMatrix[0][0], glmMatrix[0][1], glmMatrix[0][2], glmMatrix[0][3]));
		result.setRow(1, QVector4D(glmMatrix[1][0], glmMatrix[1][1], glmMatrix[1][2], glmMatrix[1][3]));
		result.setRow(2, QVector4D(glmMatrix[2][0], glmMatrix[2][1], glmMatrix[2][2], glmMatrix[2][3]));
		result.setRow(3, QVector4D(glmMatrix[3][0], glmMatrix[3][1], glmMatrix[3][2], glmMatrix[3][3]));
		return result;
	}

	QMatrix4x4 toQT(glm::mat4 matrix)
	{
		glm::mat4 glmMatrix = matrix;
		QMatrix4x4 result;
		result.setRow(0, QVector4D(glmMatrix[0][0], glmMatrix[0][1], glmMatrix[0][2], glmMatrix[0][3]));
		result.setRow(1, QVector4D(glmMatrix[1][0], glmMatrix[1][1], glmMatrix[1][2], glmMatrix[1][3]));
		result.setRow(2, QVector4D(glmMatrix[2][0], glmMatrix[2][1], glmMatrix[2][2], glmMatrix[2][3]));
		result.setRow(3, QVector4D(glmMatrix[3][0], glmMatrix[3][1], glmMatrix[3][2], glmMatrix[3][3]));
		return result;
	}

	void updateNodes(const std::vector<std::unique_ptr<Node>>& nodes)
	{
		shaderProgram->bind();

		for (const auto& node : nodes)
		{
			if (node->getName() == "Light")
			{
				int k = 0;
			}
			renderEngine->setWorldMatrix(glmMat4ToQMatrix4x4(node->calcAbsoluteTransform()));
			for (const auto& mesh : node->getMeshes())
			{
				if (mesh != nullptr)
				{
					mesh->setShaderUniform("material.ambient", QVector3D{ mesh->getMaterial().ambient.x, mesh->getMaterial().ambient.y, mesh->getMaterial().ambient.z });
					mesh->setShaderUniform("material.ambientEnabled", mesh->getMaterial().ambientEnabled);
					mesh->setShaderUniform("material.diffuse", QVector3D{ mesh->getMaterial().diffuse.x, mesh->getMaterial().diffuse.y, mesh->getMaterial().diffuse.z });
					mesh->setShaderUniform("material.diffuseEnabled", mesh->getMaterial().diffuseEnabled);
					mesh->setShaderUniform("material.specular", QVector3D{ mesh->getMaterial().specular.x, mesh->getMaterial().specular.y, mesh->getMaterial().specular.z });
					mesh->setShaderUniform("material.specularEnabled", mesh->getMaterial().specularEnabled);
					mesh->setShaderUniform("material.shininess", mesh->getMaterial().shininess);

                    mesh->setShaderUniform("material.emission", QVector3D{ mesh->getMaterial().emission.x, mesh->getMaterial().emission.y, mesh->getMaterial().emission.z });
                    mesh->setShaderUniform("material.emissionEnabled", mesh->getMaterial().emissionEnabled);

                    mesh->setShaderUniform("light.ambientEnabled", pointLights[0]->ambientEnabled());
                    mesh->setShaderUniform("light.diffuseEnabled", pointLights[0]->diffuseEnabled());
                    mesh->setShaderUniform("light.specularEnabled", pointLights[0]->specularEnabled());

					
                    glm::vec3 posit = pointLights[0]->getPos();
                    mesh->setShaderUniform("light.enabled", pointLights[0]->enabled());
					
                    mesh->setShaderUniform("light.position", QVector3D(pointLights[0]->getPos().x, pointLights[0]->getPos().y, pointLights[0]->getPos().z));
                    mesh->setShaderUniform("light.ambient", { pointLights[0]->ambient().x, pointLights[0]->ambient().y, pointLights[0]->ambient().z });
                    mesh->setShaderUniform("light.diffuse", { pointLights[0]->diffuse().x,  pointLights[0]->diffuse().y, pointLights[0]->diffuse().z });
                    mesh->setShaderUniform("light.specular", { pointLights[0]->specular().x, pointLights[0]->specular().y, pointLights[0]->specular().z });
					
					auto iter = viewPtr->modelTab->getNodeInfo(node.get());
                    mesh->setShaderUniform("isSelected", (iter) ? iter->selected() : false);

					glm::vec3 eye = viewPtr->getViewport().getCamera().getEye();
					mesh->setShaderUniform("viewPos", { eye.x, eye.y, eye.z });
					if (dynamic_cast<Manipulator*>(node.get()))
					{
						for (auto& mesh : node->getMeshes())
						{
							Manipulator* tm = dynamic_cast<Manipulator*>(node.get());
							if (tm) {
								glm::vec3 color = tm->getColor();
								mesh->setShaderUniform("inColor", { color.x, color.y, color.z });
							}
						}
						manipulatorShader->bind();

						mesh->render(renderEngine.get(), manipulatorShader.get());
					}
					else
						mesh->render(renderEngine.get(), shaderProgram.get());
				}
			}
			updateNodes(node->getChildren());
		}
	}
};

View::View(QQuickItem* parent) : QQuickFramebufferObject(parent)
{
    modelTab = new ModelTab;

	s_context->setContextProperty("ModelTab", modelTab);


    connect(this, &View::startAddNodeInfoAction, this, &View::addNodeInfo);
    setMirrorVertically(true);
	setAcceptedMouseButtons(Qt::AllButtons);
	forceActiveFocus();

	viewport.setViewportSize(960, 720);
	viewport.setFOV(45);
	viewport.setZNear(0.01);
	viewport.setZFar(200);
	viewport.getCamera().setDistanceToTarget(40);
	viewport.getCamera().setIsoView();

	addController(ButtonCode::RightButton, std::make_unique<Orbit>());
	addController(ButtonCode::LeftButton, std::make_unique<Pan>());
	addController(ButtonCode::ScrollButton, std::make_unique<Zoom>());
	//addController(ButtonCode::LeftButton, std::make_unique<AddRoadTileController>());
	addController(KeyCode::T, KeyCode::ESC, std::make_unique<TransformNodeOperator>());
	addController(KeyCode::D, [](View& view, Action action, Modifier)
		{
			if (action == Action::Press)
			{
				auto& camera = view.getViewport().getCamera();
				glm::vec3 delta = camera.calcRight() * 0.5f;
				camera.translate(delta);
			}
		});

	addController(KeyCode::A, [](View& view, Action action, Modifier)
		{
			if (action == Action::Press)
			{
				auto& camera = view.getViewport().getCamera();
				glm::vec3 delta = camera.calcRight() * -0.5f;
				camera.translate(delta);
			}
		});
	addController(KeyCode::W, [](View& view, Action action, Modifier)
		{
			if (action == Action::Press)
			{
				auto& camera = view.getViewport().getCamera();
				glm::vec3 up = glm::normalize(glm::cross(camera.calcRight(), camera.calcForward()));
				camera.translate(up * 0.5f);
			}
		});
	addController(KeyCode::S, [](View& view, Action action, Modifier)
		{
			if (action == Action::Press)
			{
				auto& camera = view.getViewport().getCamera();
				glm::vec3 up = glm::normalize(glm::cross(camera.calcRight(), camera.calcForward()));
				camera.translate(up * -0.5f);
			}
		});
}

QQuickFramebufferObject::Renderer* View::createRenderer() const
{
	return new ViewRenderer;
}

void View::keyPressEvent(QKeyEvent* event)
{
	KeyCode keyCode = KeyCode::UNKNOWN;

	if (event->key() == Qt::Key_D)
		keyCode = KeyCode::D;
	else if (event->key() == Qt::Key_A)
		keyCode = KeyCode::A;
	else if (event->key() == Qt::Key_W)
		keyCode = KeyCode::W;
	else if (event->key() == Qt::Key_S)
		keyCode = KeyCode::S;
	else if (event->key() == Qt::Key_T)
		keyCode = KeyCode::T;
	else if (event->key() == Qt::Key_Escape)
		keyCode = KeyCode::ESC;

	controllerDispatcher.processKeyboardInput(*this, keyCode, Action::Press, Modifier::NoModifier);
	update();
}

void View::mousePressEvent(QMouseEvent* event)
{
	ButtonCode buttonCode = ButtonCode::Unknown;

	if (event->button() == Qt::LeftButton)
		buttonCode = ButtonCode::LeftButton;
	else if (event->button() == Qt::RightButton)
		buttonCode = ButtonCode::RightButton;
	else if (event->button() == Qt::MiddleButton)
		buttonCode = ButtonCode::ScrollButton;

	QPoint pos = event->pos();
	double y = viewport.getHeight() - pos.y();
	controllerDispatcher.processMouseInput(*this, buttonCode, Action::Press, Modifier::NoModifier, pos.x(), pos.y());
	update();
}

void View::mouseReleaseEvent(QMouseEvent* event)
{
	ButtonCode buttonCode = ButtonCode::Unknown;

	if (event->button() == Qt::LeftButton)
		buttonCode = ButtonCode::LeftButton;
	else if (event->button() == Qt::RightButton)
		buttonCode = ButtonCode::RightButton;
	else if (event->button() == Qt::MiddleButton)
		buttonCode = ButtonCode::ScrollButton;

	QPoint pos = event->pos();
	double y = viewport.getHeight() - pos.y();
	controllerDispatcher.processMouseInput(*this, buttonCode, Action::Release, Modifier::NoModifier, pos.x(), pos.y());
	update();
}

void View::mouseMoveEvent(QMouseEvent* event)
{
	QPoint pos = event->pos();
	double y = viewport.getHeight() - pos.y();
	controllerDispatcher.processMouseMove(*this, pos.x(), pos.y());
	//forceActiveFocus();
	update();
}

void View::wheelEvent(QWheelEvent* event)
{
	QPoint pos = event->angleDelta();
	controllerDispatcher.processMouseInput(*this, ButtonCode::ScrollButton, Action::Press, Modifier::NoModifier, pos.x(), pos.y());
	update();
}

void View::addController(KeyCode enterKey, KeyCode exitKey, std::unique_ptr<Controller> op)
{
	controllerDispatcher.addController(enterKey, exitKey, std::move(op));
}

void View::addController(ButtonCode button, std::unique_ptr<Controller> op)
{
	controllerDispatcher.addController(button, std::move(op));
}

void View::addController(KeyCode key, std::unique_ptr<Controller> op)
{
	controllerDispatcher.addController(key, std::move(op));
}

std::vector<Contact> View::raycast(double x, double y, FilterValue filterValues)
{
	std::vector<Contact> contacts;
	ray cursorRay = viewport.calcCursorRay(x, y);
	std::vector<Contact> candidatesToRaycast = broadPhase(cursorRay, *modelPtr);
	contacts = narrowPhase(cursorRay, filterValues, candidatesToRaycast);
	return contacts;
}

Viewport& View::getViewport()
{
	return viewport;
}

const Viewport& View::getViewport() const
{
	return viewport;
}


std::vector<Contact> View::broadPhase(const ray& cursorRay, const Model& model)
{
	return octreePtr->getCandidatesToContact(cursorRay);
}

std::vector<Contact> View::narrowPhase(ray cursorRay, FilterValue filterValues, std::vector<Contact>& candidateNode)
{
	std::map<double, Contact> distToContactPair;

	for (auto& candidate : candidateNode)
	{
		double distToIntersectedFace = 0.0;
		if (rayIntersectFace(cursorRay, candidate.face, *candidate.node, *candidate.mesh, distToIntersectedFace))
			if (distToIntersectedFace > std::numeric_limits<double>::epsilon())
			{
				//if(dynamic_cast<Manipulator*>(candidate.node)){}
				glm::vec3 point = cursorRay.orig + (cursorRay.dir * (float)distToIntersectedFace);
				distToContactPair.emplace(distToIntersectedFace, Contact{ candidate.face, candidate.mesh,candidate.node, point });
			}
	}

	std::vector<Contact> contacts;
	for (const auto& contact : distToContactPair)
	{
		if (filterValues == FilterValue::Manipulator)
		{
			if (dynamic_cast<Manipulator*>(contact.second.node))
				contacts.emplace_back(contact.second);
		}
		else if (filterValues == FilterValue::Node)
		{
			if (dynamic_cast<Node*>(contact.second.node) && !dynamic_cast<Manipulator*>(contact.second.node))
				contacts.emplace_back(contact.second);
		}
		else
		{
			contacts.emplace_back(contact.second);
		}
	}

	std::vector<Contact> nodeContacts;
	std::vector<Contact> manipulatorContacts;
	for (const auto& contact : contacts)
	{
		if (dynamic_cast<Manipulator*>(contact.node))
			manipulatorContacts.push_back(contact);
		else
			nodeContacts.push_back(contact);
	}

	contacts.clear();
	contacts.insert(contacts.end(), manipulatorContacts.begin(), manipulatorContacts.end());
	contacts.insert(contacts.end(), nodeContacts.begin(), nodeContacts.end());

	return contacts;
}

bool View::rayIntersectFace(const ray& rayToIntersect, const HalfEdge::FaceHandle& fh, const Node& node, const Mesh& mesh, double& distToIntersectedFace)
{
	std::vector<glm::vec3> faceVertices;
	auto& halfEdgeTable = mesh.getHalfEdgeTable();
	HalfEdge::HalfEdgeHandle startHeh = halfEdgeTable.deref(fh).heh;
	HalfEdge::HalfEdgeHandle nextHeh = startHeh;
	do
	{
		faceVertices.emplace_back(node.calcAbsoluteTransform() * glm::vec4(halfEdgeTable.getStartPoint(nextHeh), 1.0f));

		nextHeh = halfEdgeTable.next(nextHeh);
	} while (nextHeh.index != startHeh.index);

	if (faceVertices.size() > 2)
	{
		if (rayIntersectTriangle(rayToIntersect, faceVertices[0], faceVertices[1], faceVertices[2], distToIntersectedFace))
			return true;
	}

	if (faceVertices.size() == 4)
	{
		if (rayIntersectTriangle(rayToIntersect, faceVertices[2], faceVertices[3], faceVertices[0], distToIntersectedFace))
			return true;
	}

	return false;
}

bool View::rayIntersectTriangle(ray ray, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, double& distToIntersectedPoint)
{
	constexpr double tolerance = std::numeric_limits<double>::epsilon();
	glm::vec3 v0v1 = v1 - v0;
	glm::vec3 v0v2 = v2 - v0;

	// Compute the unnormalized normal of plane in which the triangle lies
	glm::vec3 planeNormal = glm::cross(v0v1, v0v2);
	double triangleArea = glm::length(planeNormal);
	// Normalize the plane normal
	planeNormal /= triangleArea;

	// Check if ray and plane are parallel
	double planeNormalDotRayDir = glm::dot(ray.dir, planeNormal);
	if (planeNormalDotRayDir > -tolerance && planeNormalDotRayDir < tolerance)
		return false;

	// Calculate the distance from ray origin to the ray-plane intersection point
	distToIntersectedPoint = glm::dot((v0 - ray.orig), planeNormal) / planeNormalDotRayDir;
	// Calculate the ray-plane intersection point
	glm::vec3 intersectionPoint = ray.orig + static_cast<float>(distToIntersectedPoint) * ray.dir;

	// Point inside-outside triangle test
	glm::vec3 vp0 = intersectionPoint - v0;
	glm::vec3 pvec = glm::cross(v0v1, vp0);
	double u = glm::length(pvec);
	if (glm::dot(pvec, planeNormal) < -tolerance || u > triangleArea)
		return false;

	glm::vec3 v1v2 = v2 - v1;
	glm::vec3 vp1 = intersectionPoint - v1;
	pvec = glm::cross(v1v2, vp1);
	double v = glm::length(pvec);
	if (glm::dot(pvec, planeNormal) < -tolerance || (u + v) > triangleArea)
		return false;

	return true;
}

void View::setOctree(NodeOctree* octree)
{
	octreePtr = octree;
}

NodeOctree* View::getOctree() const
{
	return octreePtr;
}

void View::setManipulator(std::unique_ptr<Manipulator> inManipulator)
{
	manipulatorPtr = std::move(inManipulator);
    needToResetManipulator = false;
	update();
}

Manipulator* View::getManipulator()
{
	return manipulatorPtr.get();
}

void View::resetManipulator()
{
    needToResetManipulator = true;
	update();
}

void View::loadModel(QString path)
{
	path.remove("file:///");
    renderer->currentModelPathToRead = path.toStdString();
    needToLoadModel = true;
	update();
}

void View::addNodeInfo()
{
    modelTab->clearNodeInfo();
    for (auto& node : modelPtr->getNodes())
        modelTab->addNodeInfo(node.get());
    for(auto& pointLight : renderer->getPointLights())
        modelTab->addNodeInfo(pointLight.get());


    modelTab->modelsChanged();
modelTab->updateNodeToEdit();

   // lightStorage->clearNodeInfo();
   // lightStorage->addLightInfo(renderer->getPointLight());
}
