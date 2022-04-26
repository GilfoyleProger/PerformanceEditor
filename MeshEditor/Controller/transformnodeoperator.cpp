#include "TransformNodeOperator.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/intersect.hpp"
#include "glm/gtx/matrix_decompose.hpp"
#include "glm/gtx/quaternion.hpp"
#include "BboxHelper.h"
#include <algorithm>
#include "View/view.h"

void TransformNodeOperator::onEnter(View& view)
{
	currentSate = State::Idle;
}

void TransformNodeOperator::onExit(View& view)
{
	if (triadPtr)
		resetTriad(view);

    SelectionBuffer::get().clear();
    //NodeInfo* nodeInfo = view.modelTab->getNodeInfo(nodeToEdit);
    //nodeInfo->selectedChanged();
}

void TransformNodeOperator::onMouseMove(View& view, double x, double y)
{
	if (currentSate == State::Edit) {
		//y = view.getViewport().getHeight() - y;
		currentManipulator->handleMovement(MovementType::Drag, view.getViewport(), x, y);
	}
}

void TransformNodeOperator::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
	if (currentSate == State::Idle)
	{
		if (button == ButtonCode::LeftButton && action == Action::Press)
		{
			std::vector<Contact> contacts = view.raycast(x, y, FilterValue::NM);

			if (contacts.empty())
			{
				resetTriad(view);
				SelectionBuffer::get().clear();
               // NodeInfo* nodeInfo = view.modelTab->getNodeInfo(nodeToEdit);
               // nodeInfo->selectedChanged();
				return;
			}

			Contact& contact = contacts.front();

			if (dynamic_cast<Manipulator*>(contact.node))
			{
				currentSate = State::Edit;
				currentManipulator = dynamic_cast<Manipulator*>(contact.node);
			}
			else
			{
				nodeToEdit = contact.node;
				NodeInfo* nodeInfo = view.modelTab->getNodeInfo(contact.node);
                SelectionBuffer::get().clear();
				SelectionBuffer::get().select(nodeInfo, false);
               // nodeInfo->selectedChanged();
				currentSate = State::Idle;

				initTriad(view);

				triadPtr->setCallback([&node = nodeToEdit, &prev = prevDelta](const glm::mat4& delta)
					{
						prev = delta * prev;
						node->applyRelativeTransform(delta);
					});
			}
		}
	}

	if (currentSate == State::Edit)
	{
		if (button == ButtonCode::LeftButton && action == Action::Press)
		{
			//y = view.getViewport().getHeight() - y;
			currentManipulator->handleMovement(MovementType::Push, view.getViewport(), x, y);
		}
		else if (button == ButtonCode::LeftButton && action == Action::Release)
		{
			currentSate = State::Idle;
			currentManipulator->handleMovement(MovementType::Release, view.getViewport(), x, y);
			//for (auto& child : triadPtr->getChildren())
			//	child->applyRelativeTransform(glm::inverse(prevDelta));
			prevDelta = glm::mat4(1.0f);
			for (auto& child : triadPtr->getChildren())
				view.getOctree()->updateNode(child.get());
			view.getOctree()->updateNode(nodeToEdit);
			// y = view.getViewport().getHeight() - y;
			//currentManipulator->handleMovement(MovementType::Release, view.getViewport(), x, y);
		}
	}
}

void TransformNodeOperator::initTriad(View& view)
{
	resetTriad(view);

	view.setManipulator(std::make_unique<Triad>());
	triadPtr = dynamic_cast<Triad*>(view.getManipulator());
	triadPtr->setName("Triad");
	/*
	 std::unique_ptr<ScaleManipulator> scaleX = std::make_unique<ScaleManipulator>(glm::vec3{ 1,0,0 });
	 //scaleX->getMesh()->setMeshColor(glm::vec3{ 0,0,0.8 });
	 triadPtr->attachNode(std::move(scaleX));

	 std::unique_ptr<ScaleManipulator> scaleY = std::make_unique<ScaleManipulator>(glm::vec3{ 0,1,0 });
	// scaleY->getMesh()->setMeshColor(glm::vec3{ 0,0.8,0 });
	 triadPtr->attachNode(std::move(scaleY));
	 std::unique_ptr<ScaleManipulator> scaleZ = std::make_unique<ScaleManipulator>(glm::vec3{ 0,0,1 });
	 //scaleZ->getMesh()->setMeshColor(glm::vec3{ 0.8,0,0 });
	 triadPtr->attachNode(std::move(scaleZ));
	 */
	calcScaleMat(view);
	calcRotationMat();
	calcTranslationMat();

	for (auto& child : triadPtr->getChildren())
	{
		view.getOctree()->addNode(child.get());
		//child->getMesh()->renderLines();
	}
}

void TransformNodeOperator::resetTriad(View& view)
{
	if (triadPtr)
	{
		for (auto& child : triadPtr->getChildren())
			view.getOctree()->removeNode(child.get());
		view.resetManipulator();
		//view.manipulatorPtr.reset();
		if (nodeToEdit)
			view.getOctree()->updateNode(nodeToEdit);
		triadPtr = nullptr;
		view.update();
	}
}

void TransformNodeOperator::calcRotationMat()
{
	glm::vec3 scale;
	glm::quat rotation;
	glm::vec3 translation;
	glm::vec3 skew;
	glm::vec4 perspective;

	glm::decompose(nodeToEdit->calcAbsoluteTransform(), scale, rotation, translation, skew, perspective);
	glm::mat4 resMat = glm::toMat4(rotation);

	for (auto& child : triadPtr->getChildren())
		child->applyRelativeTransform(resMat);
}

void TransformNodeOperator::calcScaleMat(View& view)
{
	std::vector<Node*>nodeList = BboxHelper::getNodeList(triadPtr);
	bbox triadBbox = nodeList[0]->getLocalBbox();
	for (int i = 1; i < nodeList.size(); i++)
	{
		triadBbox = BboxHelper::combineBoxes(triadBbox, nodeList[i]->getLocalBbox());
	}

	glm::vec4 viewport = glm::vec4(0, 0, view.getViewport().getWidth(), view.getViewport().getHeight());
	auto& camera = view.getViewport().getCamera();
	glm::vec3 nodeOrigin = triadPtr->calcAbsoluteTransform() * glm::vec4(0, 0, 0, 1);
	glm::vec3 cameraNodeDir = nodeOrigin - view.getViewport().getCamera().getEye();//glm::normalize(
	glm::vec3 intersection = cameraNodeDir * (float)view.getViewport().getZNear() +
		view.getViewport().getCamera().getEye();

	float scaleFactor = glm::length(intersection - nodeOrigin) * view.getViewport().getZNear();
	glm::mat4 scale = glm::scale(glm::vec3(scaleFactor * 4.0f));
	


		//glm::mat4 scaleToNode = glm::scale(glm::mat4(1.0f), { scaleFactor, scaleFactor, scaleFactor });
		//triadPtr->setRelativeTransform(triadPtr->getRelativeTransform() * scale); 
			for (auto& child : triadPtr->getChildren())
				child->setRelativeTransform(child->getRelativeTransform() * scale);
	/*
	RotationManipulator* rotationManipulator = nullptr;
	for (auto& child : triadPtr->getChildren())
	{
		rotationManipulator = dynamic_cast<RotationManipulator*>(child.get());
		if (rotationManipulator)
			break;
	}

	bbox rotManipBbox = BboxHelper::getBbox(*rotationManipulator);
	float rotManipDiagonal = glm::length(rotManipBbox.max - rotManipBbox.min);

	bbox nodeBbox = BboxHelper::getBbox(*nodeToEdit);
	float nodeDiagonal = glm::length(nodeBbox.max - nodeBbox.min);
	*/
	////float scaleFactor = (nodeDiagonal / rotManipDiagonal) * 1.5f;
	//glm::mat4 scaleToNode = glm::scale(glm::mat4(1.0f), { scaleFactor, scaleFactor, scaleFactor });

	// if (rotationManipulator)
	// {
	 //    bbox rotManipBbox = BboxHelper::getBbox(*rotationManipulator);
	 //    float rotManipDiagonal = glm::length(rotManipBbox.max - rotManipBbox.min);
//
//	for (auto& child : triadPtr->getChildren())
	//	child->setRelativeTransform(child->getRelativeTransform() * scaleToNode);
	// }
}

void TransformNodeOperator::calcTranslationMat()
{
	bbox nodeBbox = nodeToEdit->getLocalBbox();
	glm::vec3 nodeMidPoint = (nodeBbox.max + nodeBbox.min) / 2.0f;
	nodeMidPoint = nodeToEdit->getRelativeTransform() * glm::vec4(nodeMidPoint, 1.0f);
	glm::mat4 translateToMidPoint = glm::translate(glm::mat4(1.0f), nodeMidPoint);

	for (auto& child : triadPtr->getChildren())
		child->applyRelativeTransform(translateToMidPoint);

	if (nodeToEdit->getParent() != nullptr)
	{
		glm::mat4 trf = nodeToEdit->getParent()->calcAbsoluteTransform();
		glm::mat4 trans = glm::translate(glm::mat4(1.0f), glm::vec3(trf[3]));
		triadPtr->applyRelativeTransform(trans);
	}
}

