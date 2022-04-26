#include "Triad.h"

Triad::Triad()
{
	Manipulator::attachNode(std::make_unique<TranslationManipulator>(glm::vec3{ 1,0,0 }));
	dynamic_cast<Manipulator*>(Manipulator::getChildren().back().get())->setColor({ 1,0,0 });
	Manipulator::getChildren().back().get()->setName("TranslateX");

	Manipulator::attachNode(std::make_unique<TranslationManipulator>(glm::vec3{ 0,1,0 }));
	dynamic_cast<Manipulator*>(Manipulator::getChildren().back().get())->setColor({ 0,1,0 });
	Manipulator::getChildren().back().get()->setName("TranslateY");

	Manipulator::attachNode(std::make_unique<TranslationManipulator>(glm::vec3{ 0,0,1 }));
	dynamic_cast<Manipulator*>(Manipulator::getChildren().back().get())->setColor({ 0,0,1 });
	Manipulator::getChildren().back().get()->setName("TranslateZ");

	Manipulator::attachNode(std::make_unique<RotationManipulator>(glm::vec3{ 1,0,0 }));
	dynamic_cast<Manipulator*>(Manipulator::getChildren().back().get())->setColor({ 1,0,0 });
	Manipulator::getChildren().back().get()->setName("RotateX");

	Manipulator::attachNode(std::make_unique<RotationManipulator>(glm::vec3{ 0,1,0 }));
	dynamic_cast<Manipulator*>(Manipulator::getChildren().back().get())->setColor({ 0,1,0 });
	Manipulator::getChildren().back().get()->setName("RotateY");

	Manipulator::attachNode(std::make_unique<RotationManipulator>(glm::vec3{ 0,0,1 }));
	dynamic_cast<Manipulator*>(Manipulator::getChildren().back().get())->setColor({ 0,0,1 });
	Manipulator::getChildren().back().get()->setName("RotateZ");

	Manipulator::attachNode(std::make_unique<ScaleManipulator>(glm::vec3{ 1,0,0 }));
	dynamic_cast<Manipulator*>(Manipulator::getChildren().back().get())->setColor({ 1,0,0 });
	Manipulator::getChildren().back().get()->setName("ScaleX");

	Manipulator::attachNode(std::make_unique<ScaleManipulator>(glm::vec3{ 0,1,0 }));
	dynamic_cast<Manipulator*>(Manipulator::getChildren().back().get())->setColor({ 0,1,0 });
	Manipulator::getChildren().back().get()->setName("ScaleY");

	Manipulator::attachNode(std::make_unique<ScaleManipulator>(glm::vec3{ 0,0,1 }));
	dynamic_cast<Manipulator*>(Manipulator::getChildren().back().get())->setColor({ 0,0,1 });
	Manipulator::getChildren().back().get()->setName("ScaleZ");



	int childrenCount = getChildren().size();
	/*
		getChildren()[childrenCount - 1]->getMesh()->setMeshColor({ 1,0,0 });
		getChildren()[childrenCount - 2]->getMesh()->setMeshColor({ 0,1,0 });
		getChildren()[childrenCount - 3]->getMesh()->setMeshColor({ 0,0,1 });

		getChildren()[childrenCount - 4]->getMesh()->setMeshColor({ 1,0,0 });
		getChildren()[childrenCount - 5]->getMesh()->setMeshColor({ 0,1,0 });
		getChildren()[childrenCount - 6]->getMesh()->setMeshColor({ 0,0,1 });*/
}
