#pragma once

#include "model.h"

#include <QObject>
#include <QVector3D>
#include "Controller/pointlight.h"

class ModelTab;

class MaterialInfo : public QObject
{
	Q_OBJECT
		Q_PROPERTY(QString name READ name CONSTANT)
		Q_PROPERTY(QVector3D ambient READ ambient WRITE setAmbient NOTIFY materialChanged)
		Q_PROPERTY(QVector3D diffuse READ diffuse WRITE setDiffuse NOTIFY materialChanged)
		Q_PROPERTY(QVector3D specular READ specular WRITE setSpecular NOTIFY materialChanged)
		Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY materialChanged)
		Q_PROPERTY(QVector3D emission READ emission WRITE setEmission NOTIFY materialChanged)

		Q_PROPERTY(bool ambientEnabled READ ambientEnabled WRITE setAmbientEnabled NOTIFY materialChanged)
		Q_PROPERTY(bool diffuseEnabled READ diffuseEnabled WRITE setDiffuseEnabled NOTIFY materialChanged)
		Q_PROPERTY(bool specularEnabled READ specularEnabled WRITE setSpecularEnabled NOTIFY materialChanged)
		Q_PROPERTY(bool emissionEnabled READ emissionEnabled WRITE setEmissionEnabled NOTIFY materialChanged)

    Q_PROPERTY(bool diffuseMapEnabled READ diffuseMapEnabled WRITE setDiffuseMapEnabled NOTIFY materialChanged)
    Q_PROPERTY(bool specularMapEnabled READ specularMapEnabled WRITE setSpecularMapEnabled NOTIFY materialChanged)
	Q_PROPERTY(bool normalMapEnabled READ normalMapEnabled WRITE setNormalMapEnabled NOTIFY materialChanged)

    Q_PROPERTY(QString diffuseMapName READ diffuseMapName CONSTANT)
		Q_PROPERTY(QString specularMapName READ specularMapName CONSTANT)
		Q_PROPERTY(QString normalMapName READ normalMapName CONSTANT)

public:
	explicit MaterialInfo(Mesh* mesh)
	{
		meshPtr = mesh;
	}

	QString name() const;

	QVector3D ambient();
	void setAmbient(const QVector3D& newAmbient);

	QVector3D diffuse();
	void setDiffuse(const QVector3D& newDiffuse);

	QVector3D specular();
	void setSpecular(const QVector3D& newSpecular);

	QVector3D emission();
	void setEmission(const QVector3D& newEmission);

	float shininess();
	void setShininess(float newShininess);

	bool ambientEnabled();
	void setAmbientEnabled(bool state);

	bool diffuseEnabled();
	void setDiffuseEnabled(bool state);

	bool specularEnabled();
	void setSpecularEnabled(bool state);

	bool emissionEnabled();
	void setEmissionEnabled(bool state);

    bool diffuseMapEnabled()
    {
        return meshPtr->getMaterial().diffuseMapEnabled;
    }
    void setDiffuseMapEnabled(bool state)
    {
        if (state == diffuseMapEnabled())
            return;
        meshPtr->getMaterial().diffuseMapEnabled = state;
        emit materialChanged();
    }

    QString diffuseMapName()
    {
		if (meshPtr->getDiffuseMap()) 
		{
			return QString::fromStdString(meshPtr->getDiffuseMap()->name);
		}
        return "none";
    }

    bool specularMapEnabled()
    {
        return meshPtr->getMaterial().specularMapEnabled;
    }
    void setSpecularMapEnabled(bool state)
    {
        if (state == specularMapEnabled())
            return;
        meshPtr->getMaterial().specularMapEnabled = state;
        emit materialChanged();
    }
    QString specularMapName()
    {
        if (meshPtr->getSpecularMap())
        {
            return QString::fromStdString(meshPtr->getSpecularMap()->name);
        }
        return "none";
    }

	bool normalMapEnabled()
	{
		return meshPtr->getMaterial().normalMapEnabled;
	}
	void setNormalMapEnabled(bool state)
	{
		if (state == normalMapEnabled())
			return;
		meshPtr->getMaterial().normalMapEnabled = state;
		emit materialChanged();
	}
	QString normalMapName()
	{
		if (meshPtr->getNormalMap())
		{
			return QString::fromStdString(meshPtr->getNormalMap()->name);
		}
		return "none";
	}
signals:
	void materialChanged();

private:
	Mesh* meshPtr = nullptr;
};

class PointLightParamsInfo : public QObject
{
	Q_OBJECT
		Q_PROPERTY(QVector3D ambient READ ambient WRITE setAmbient NOTIFY paramsChanged)
		Q_PROPERTY(QVector3D diffuse READ diffuse WRITE setDiffuse NOTIFY paramsChanged)
		Q_PROPERTY(QVector3D specular READ specular WRITE setSpecular NOTIFY paramsChanged)

		Q_PROPERTY(bool ambientEnabled READ ambientEnabled WRITE setAmbientEnabled NOTIFY paramsChanged)
		Q_PROPERTY(bool diffuseEnabled READ diffuseEnabled WRITE setDiffuseEnabled NOTIFY paramsChanged)
		Q_PROPERTY(bool specularEnabled READ specularEnabled WRITE setSpecularEnabled NOTIFY paramsChanged)
		Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY paramsChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY paramsChanged)

public:
	explicit PointLightParamsInfo(PointLight* inLightPtr)
	{
		lightPtr = inLightPtr;
	}

	QVector3D ambient();
	void setAmbient(const QVector3D& newAmbient);

	QVector3D diffuse();
	void setDiffuse(const QVector3D& newDiffuse);

	QVector3D specular();
	void setSpecular(const QVector3D& newSpecular);

	bool enabled();
	void setEnabled(bool state);

	bool ambientEnabled();
	void setAmbientEnabled(bool state);

	bool diffuseEnabled();
	void setDiffuseEnabled(bool state);

	bool specularEnabled();
	void setSpecularEnabled(bool state);

    bool visible();
    void setVisible(bool state);

signals:
	void paramsChanged();

private:
	PointLight* lightPtr;
};

class NodeInfo : public QObject
{
	Q_OBJECT
		Q_PROPERTY(QString name READ name CONSTANT)
		Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectedChanged)
		Q_PROPERTY(bool recursivelySelected READ recursivelySelected WRITE setRecursivelySelected NOTIFY selectedChanged)

public:
	explicit NodeInfo(Node* inNodePtr, std::map<Node*, NodeInfo*>* inNodesInfo, ModelTab* modelTab);

	QString name();

	bool recursivelySelected();
	void setRecursivelySelected(bool state);

	bool selected();
	void setSelected(bool state);

	std::vector<NodeInfo*> getChildren();
	Node* getNode()
	{
		return nodePtr;
	}
	
signals:
	void selectedChanged();

protected:
	Node* nodePtr = nullptr;
	std::map<Node*, NodeInfo*>* nodesInfo;
	bool isSelected = false;
	ModelTab* modelTab;
};

class ModelNodeInfo : public NodeInfo
{
	Q_OBJECT
		Q_PROPERTY(QList<QObject*> materials READ materials NOTIFY materialsChanged)

public:
	ModelNodeInfo(Node* inNodePtr, std::map<Node*, NodeInfo*>* inNodesInfo,ModelTab* modelTab) : NodeInfo(inNodePtr, inNodesInfo, modelTab)
	{
		for (auto& mesh : inNodePtr->getMeshes())
		{
			MaterialInfo* materialInfo = new MaterialInfo(mesh.get());
			emit materialInfo->materialChanged();
			_materials.push_back(materialInfo);
		}
	}

	QList<QObject*> materials()
	{
		QList<QObject*> result;
		for (auto& material : _materials)
		{
			result.push_back(material);
		}
		return result;
	}

signals:
	void materialsChanged();

private:
	QList<MaterialInfo*> _materials;
};

class PointLightNodeInfo : public NodeInfo
{
	Q_OBJECT
		Q_PROPERTY(QObject* params READ params NOTIFY paramsChanged)

public:
	PointLightNodeInfo(Node* inNodePtr, std::map<Node*, NodeInfo*>* inNodesInfo,ModelTab*modelTab) : NodeInfo(inNodePtr, inNodesInfo, modelTab)
	{

		_lightParams = new PointLightParamsInfo(dynamic_cast<PointLight*>(inNodePtr));
		emit _lightParams->paramsChanged();
	}

	QObject* params()
	{
		QObject* result = _lightParams;
		return result;
	}

signals:
	void paramsChanged();

private:
	PointLightParamsInfo* _lightParams;
};

class ModelTab : public QObject
{
	Q_OBJECT
		Q_PROPERTY(QList<QObject*> nodes READ nodes NOTIFY modelsChanged)
		Q_PROPERTY(QList<QObject*> models READ models NOTIFY modelsChanged)
    Q_PROPERTY(QList<QObject*> lights READ lights NOTIFY modelsChanged)
		Q_PROPERTY(QObject* currentNode READ currentNode NOTIFY currentNodeChanged)
		Q_PROPERTY(QObject* currentLight READ currentLight NOTIFY currentLightChanged)
Q_PROPERTY(QObject* currentLight READ currentLight NOTIFY currentLightChanged)
    Q_PROPERTY(bool lightingEnabled READ lightingEnabled WRITE setLightingEnabled NOTIFY lightingChanged)
    Q_PROPERTY(QVector3D backgroundColor READ backgroundColor WRITE setBackgroundColor NOTIFY backgroundChanged)

public:
	explicit ModelTab();

    void setBackgroundColor(QVector3D newColor)
    {
        if(newColor==_backgroundColor)
            return;
        _backgroundColor=newColor;
    }
    QVector3D backgroundColor()
    {
        return _backgroundColor;
    }

    void setLightingEnabled(bool state)
    {
        if(state==_lightingEnabled)
            return;
        _lightingEnabled = state;
        emit lightingChanged();
    }

    bool lightingEnabled()
    {
        return _lightingEnabled;
    }

	QObject* currentNode()
	{
		return dynamic_cast<ModelNodeInfo*>(nodeToEdit);
	}

	QObject* currentLight()
	{
		if (dynamic_cast<PointLightNodeInfo*>(lightToEdit))
		{
			PointLightNodeInfo* pointLightInfo = dynamic_cast<PointLightNodeInfo*>(lightToEdit);
			return pointLightInfo;
		}
		else
		{
			return nullptr;
		}
	}

	QList<QObject*> models()
	{
		QList<QObject*> result;

		for (auto& node : nodesInfo)
		{
			if (dynamic_cast<ModelNodeInfo*>(node.second) && !node.first->getParent())
			{
				result.push_back(node.second);
			}
		}
		return result;
	}

	QList<QObject*> nodes()
	{
		QList<QObject*> result;

		for (auto& node : nodesInfo)
		{
			if (dynamic_cast<ModelNodeInfo*>(node.second))
			{
				result.push_back(node.second);
			}
		}
		return result;
	}

    QList<QObject*> lights()
    {
        QList<QObject*> result;

        for (auto& node : nodesInfo)
        {/*
            if (dynamic_cast<PointLight*>(node.first) && !dynamic_cast<Node*>(node.first))
            {
                result.push_back(node.second);
            }*/
            
            if (dynamic_cast<PointLightNodeInfo*>(node.second))
            {
                result.push_back(node.second);
				emit node.second->selectedChanged();
            }
        }
        return result;
    }

	void clearNodeInfo()
	{
		nodesInfo.clear();
	}

	void setNodeToEdit(Node* node)
	{
		nodeToEdit = getNodeInfo(node);
		emit currentNodeChanged();
	}

	void addNodeInfo(Node* node)
	{
		if (node)
		{
			//if (!node->getMeshes().empty())
			//{
				if (dynamic_cast<Node*>(node) && !dynamic_cast<PointLight*>(node))
				{
					ModelNodeInfo* nodeInfo = new ModelNodeInfo(node, &nodesInfo, this);
					nodesInfo.emplace(node, nodeInfo);
					nodeToEdit = nodeInfo;
					emit currentNodeChanged();
				}
				else if (dynamic_cast<PointLight*>(node))
				{
                    PointLightNodeInfo* lightInfo = new PointLightNodeInfo(node, &nodesInfo, this);
					nodesInfo.emplace(node, lightInfo);
					lightToEdit = lightInfo;
					emit currentLightChanged();
				}
			//}
			if (!node->getChildren().empty())
				addNodeInfo(node->getChildren());
		}
	}
	void addNodeInfo(const std::vector<std::unique_ptr<Node>>& nodes)
	{
		for (const auto& node : nodes)
		{
			if (node)
			{
				//if (!node->getMeshes().empty())
				//{
					if (dynamic_cast<Node*>(node.get()) && !dynamic_cast<PointLight*>(node.get()))
					{
						ModelNodeInfo* nodeInfo = new ModelNodeInfo(node.get(), &nodesInfo, this);
						nodesInfo.emplace(node.get(), nodeInfo);
						nodeToEdit = nodeInfo;
						emit currentNodeChanged();
					}
					else if (dynamic_cast<PointLight*>(node.get()))
					{
                        PointLightNodeInfo* lightInfo = new PointLightNodeInfo(node.get(), &nodesInfo, this);
						nodesInfo.emplace(node.get(), lightInfo);
						lightToEdit = lightInfo;
						emit currentLightChanged();
					}
				//}
				addNodeInfo(node->getChildren());
			}
		}
	}

	NodeInfo* getNodeInfo(Node* node)
	{
		auto iter = nodesInfo.find(node);
		return iter != nodesInfo.end() ? iter->second : nullptr;
	}

	void updateNodeToEdit();

	void setModel(Model* model) 
	{
		modelPtr = model;
	}

	Model* getModel() 
	{
		return modelPtr;
	}

	Node* getLastSelected() 
	{
		return lastSelected;
	}
	void setLastSelected(Node*node)
	{
		lastSelected = node;
	}

	std::vector<NodeInfo*>getModels() 
	{
		std::vector<NodeInfo*> result;

		for (auto& node : nodesInfo)
		{
			if (dynamic_cast<ModelNodeInfo*>(node.second) && !node.first->getParent())
			{
				result.push_back(node.second);
			}
		}
		return result;
	}

signals:
	void modelsChanged();
	void currentNodeChanged();
	void currentLightChanged();
    void lightingChanged();
    void backgroundChanged();
private:
	std::map<Node*, NodeInfo*> lightsInfo;
	std::map<Node*, NodeInfo*> nodesInfo;
	NodeInfo* lightToEdit = nullptr;
	NodeInfo* nodeToEdit = nullptr;
	Model* modelPtr = nullptr;
    bool _lightingEnabled = true;
    QVector3D _backgroundColor={0,0,0};

	Node* lastSelected=nullptr;
};
