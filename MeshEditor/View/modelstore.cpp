#include "modelstore.h"
#include "selectionbuffer.h"

QString MaterialInfo::name() const
{
	return QString::fromStdString(meshPtr->getMaterial().name);
}

QVector3D MaterialInfo::ambient()
{
	return QVector3D(meshPtr->getMaterial().ambient.x, meshPtr->getMaterial().ambient.y, meshPtr->getMaterial().ambient.z);
}

void MaterialInfo::setAmbient(const QVector3D& newAmbient)
{
	if (newAmbient == ambient())
		return;
	meshPtr->getMaterial().ambient.x = newAmbient.x();
	meshPtr->getMaterial().ambient.y = newAmbient.y();
	meshPtr->getMaterial().ambient.z = newAmbient.z();
	emit materialChanged();
}

QVector3D MaterialInfo::diffuse()
{
	return QVector3D(meshPtr->getMaterial().diffuse.x, meshPtr->getMaterial().diffuse.y, meshPtr->getMaterial().diffuse.z);
}

void MaterialInfo::setDiffuse(const QVector3D& newDiffuse)
{
	if (newDiffuse == ambient())
		return;
	meshPtr->getMaterial().diffuse.x = newDiffuse.x();
	meshPtr->getMaterial().diffuse.y = newDiffuse.y();
	meshPtr->getMaterial().diffuse.z = newDiffuse.z();
	emit materialChanged();
}

QVector3D MaterialInfo::specular()
{
	return QVector3D(meshPtr->getMaterial().specular.x, meshPtr->getMaterial().specular.y, meshPtr->getMaterial().specular.z);
}

void MaterialInfo::setSpecular(const QVector3D& newSpecular)
{
	if (newSpecular == specular())
		return;
	meshPtr->getMaterial().specular.x = newSpecular.x();
	meshPtr->getMaterial().specular.y = newSpecular.y();
	meshPtr->getMaterial().specular.z = newSpecular.z();
	emit materialChanged();
}

float MaterialInfo::shininess()
{
	return meshPtr->getMaterial().shininess;
}

void MaterialInfo::setShininess(float newShininess)
{
	if (newShininess == shininess())
		return;
	meshPtr->getMaterial().shininess = newShininess;
	emit materialChanged();
}

QVector3D MaterialInfo::emission()
{
	return QVector3D(meshPtr->getMaterial().emission.x, meshPtr->getMaterial().emission.y, meshPtr->getMaterial().emission.z);
}

void MaterialInfo::setEmission(const QVector3D& newEmission)
{
	if (newEmission == emission())
		return;
	meshPtr->getMaterial().emission.x = newEmission.x();
	meshPtr->getMaterial().emission.y = newEmission.y();
	meshPtr->getMaterial().emission.z = newEmission.z();
	emit materialChanged();
}

bool MaterialInfo::ambientEnabled()
{
	return meshPtr->getMaterial().ambientEnabled;
}

void MaterialInfo::setAmbientEnabled(bool state)
{
	if (state == ambientEnabled())
		return;
	meshPtr->getMaterial().ambientEnabled = state;
	emit materialChanged();
}

bool MaterialInfo::diffuseEnabled()
{
	return meshPtr->getMaterial().diffuseEnabled;
}

void MaterialInfo::setDiffuseEnabled(bool state)
{
	if (state == diffuseEnabled())
		return;
	meshPtr->getMaterial().diffuseEnabled = state;
	emit materialChanged();
}

bool MaterialInfo::specularEnabled()
{
	return meshPtr->getMaterial().specularEnabled;
}

void MaterialInfo::setSpecularEnabled(bool state)
{
	if (state == specularEnabled())
		return;
	meshPtr->getMaterial().specularEnabled = state;
	emit materialChanged();
}

bool MaterialInfo::emissionEnabled()
{
	return meshPtr->getMaterial().emissionEnabled;
}

void MaterialInfo::setEmissionEnabled(bool state)
{
	if (state == emissionEnabled())
		return;
	meshPtr->getMaterial().emissionEnabled = state;
	emit materialChanged();
}



QVector3D PointLightParamsInfo::ambient()
{
	return QVector3D(lightPtr->ambient().x, lightPtr->ambient().y, lightPtr->ambient().z);
}

void PointLightParamsInfo::setAmbient(const QVector3D& newAmbient)
{
	if (newAmbient == ambient())
		return;
	lightPtr->ambient().x = newAmbient.x();
	lightPtr->ambient().y = newAmbient.y();
	lightPtr->ambient().z = newAmbient.z();
	emit paramsChanged();
}

QVector3D PointLightParamsInfo::diffuse()
{
	return QVector3D(lightPtr->diffuse().x, lightPtr->diffuse().y, lightPtr->diffuse().z);
}

void PointLightParamsInfo::setDiffuse(const QVector3D& newDiffuse)
{
	if (newDiffuse == ambient())
		return;
	lightPtr->diffuse().x = newDiffuse.x();
	lightPtr->diffuse().y = newDiffuse.y();
	lightPtr->diffuse().z = newDiffuse.z();
	emit paramsChanged();
}

QVector3D PointLightParamsInfo::specular()
{
	return QVector3D(lightPtr->specular().x, lightPtr->specular().y, lightPtr->specular().z);
}

void PointLightParamsInfo::setSpecular(const QVector3D& newSpecular)
{
	if (newSpecular == specular())
		return;
	lightPtr->specular().x = newSpecular.x();
	lightPtr->specular().y = newSpecular.y();
	lightPtr->specular().z = newSpecular.z();
	emit paramsChanged();
}

bool PointLightParamsInfo::enabled()
{
	return lightPtr->enabled();
}

void PointLightParamsInfo::setEnabled(bool state)
{
	if (state == enabled())
		return;
	lightPtr->setEnabled(state);
	emit paramsChanged();
}

bool PointLightParamsInfo::ambientEnabled()
{
	return lightPtr->ambientEnabled();
}

void PointLightParamsInfo::setAmbientEnabled(bool state)
{
	if (state == ambientEnabled())
		return;
	lightPtr->setAmbientEnabled(state);
	emit paramsChanged();
}

bool PointLightParamsInfo::diffuseEnabled()
{
	return lightPtr->diffuseEnabled();
}

void PointLightParamsInfo::setDiffuseEnabled(bool state)
{
	if (state == diffuseEnabled())
		return;
	lightPtr->setDiffuseEnabled(state);
	emit paramsChanged();
}

bool PointLightParamsInfo::specularEnabled()
{
	return lightPtr->specularEnabled();
}

void PointLightParamsInfo::setSpecularEnabled(bool state)
{
	if (state == specularEnabled())
		return;
	lightPtr->setSpecularEnabled(state);
	emit paramsChanged();
}

bool PointLightParamsInfo::visible()
{
	return lightPtr->visible();
}

void PointLightParamsInfo::setVisible(bool state)
{
	if (state == visible())
		return;
	lightPtr->setVisible(state);
	emit paramsChanged();
}



NodeInfo::NodeInfo(Node* inNodePtr, std::map<Node*, NodeInfo*>* inNodesInfo, ModelTab* _modelTab)
{
	nodePtr = inNodePtr;
	nodesInfo = inNodesInfo;
	modelTab = _modelTab;
}

QString NodeInfo::name()
{
	return QString::fromStdString(nodePtr->getName());
}

bool NodeInfo::selected()
{
	//if (!nodePtr->getParent() && nodePtr->getMeshes().empty())
	//	return SelectionBuffer::get().isOneChildSelected(this);
	//else
	return  SelectionBuffer::get().isSelected(this);
}

void NodeInfo::setSelected(bool state)
{
	if (state == SelectionBuffer::get().isSelected(this))
		return;

	if (state)
	{
		modelTab->setLastSelected(nodePtr);
		SelectionBuffer::get().clear();
		SelectionBuffer::get().select(this);

	}
	else
	{
		//if (modelTab->getLastSelected() == nodePtr)
		//	modelTab->setLastSelected(nullptr);
		SelectionBuffer::get().deselect(this);

	}
	Node* root = nodePtr->getRoot();
	if (root) {
		if (root->getMeshes().empty()) {
			emit modelTab->getNodeInfo(root)->selectedChanged();
			//emit modelTab->getNodeInfo(root)->selectedChanged();
			//SelectionBuffer::get().select(modelTab->getNodeInfo(root));
			//modelTab->getNodeInfo(root)selectedChanged
			//	emit root->();
		}
	}
	//isSelected = state;
//emit selectedChanged();
}


bool NodeInfo::recursivelySelected()
{
	return  SelectionBuffer::get().isOneChildSelected(this);
}

void NodeInfo::setRecursivelySelected(bool state)
{
	if (state == SelectionBuffer::get().isSelected(this))
		return;

	if (state)
	{
		modelTab->setLastSelected(nodePtr);
		SelectionBuffer::get().clear();
		SelectionBuffer::get().select(this, true);
	}
	else
	{
		//	if (modelTab->getLastSelected() == nodePtr)
		//		modelTab->setLastSelected(nullptr);
		SelectionBuffer::get().deselect(this, true);
	}
}








std::vector<NodeInfo*> NodeInfo::getChildren()
{
	std::vector<NodeInfo*>childs;
	for (auto& nodeChild : nodePtr->getChildren())
	{
		childs.push_back(nodesInfo->find(nodeChild.get())->second);
	}
	return childs;
}



ModelTab::ModelTab()
{
	connect(&SelectionBuffer::get(), &SelectionBuffer::selectionBufferChanged, this, &ModelTab::updateNodeToEdit);
	//emit backgroundChanged();
}

void ModelTab::updateNodeToEdit()
{
	if (!SelectionBuffer::get().getItems().empty())
	{
		for (auto item : SelectionBuffer::get().getItems())
		{
			if (dynamic_cast<ModelNodeInfo*>(item))
			{
				if (item->getNode()->getMeshes().empty() && !item->getNode()->getParent())
				{
					for (auto& node : item->getNode()->getChildren()) 
					{
						if (!node->getMeshes().empty()) 
						{
							nodeToEdit = getNodeInfo(node.get());
							break;
						}
					}
					//nodeToEdit = getNodeInfo(item->getNode()->getChildren().front().get());
				}
				else
				{
					nodeToEdit = item;
				}
				break;
			}
		}

		for (auto item : SelectionBuffer::get().getItems())
		{
			if (dynamic_cast<PointLightNodeInfo*>(item))
			{
				lightToEdit = item;
				break;
			}
		}
	}

	Node* prevRootNode = nullptr;
	for (auto& nodeToInfo : nodesInfo)
	{
		Node* rootNode = nodeToInfo.first->getRoot();

		if (rootNode->getMeshes().empty())
		{
			if (rootNode != prevRootNode)
			{
				prevRootNode = rootNode;
				NodeInfo* rootNodeInfo = getNodeInfo(rootNode);
				emit rootNodeInfo->selectedChanged();
			}
		}
	}
	int modelCount = 0;
	for (auto& node : nodesInfo)
	{
		if (dynamic_cast<ModelNodeInfo*>(node.second))
		{
			++modelCount;
		}
	}
	if (modelCount == 0) 
	{
		nodeToEdit = nullptr;
	}

	emit currentNodeChanged();
	emit currentLightChanged();
	emit modelsChanged();
}

/*
bool NodeInfo::selected()
{
	return isSelected;
}

void NodeInfo::setSelected(bool state)
{
	isSelected = state;
	//SelectionBuffer::get().clear();
	//SelectionBuffer::get().select(this);
	emit selectedChanged();
}
*/
