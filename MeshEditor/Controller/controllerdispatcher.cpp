#include "controllerdispatcher.h"
#include <utility>
#include "View/view.h"

void ControllerDispatcher::addController(KeyCode enterKey, KeyCode exitKey, std::unique_ptr<Controller> controller)
{
	try
	{
		for (const auto& op : keysToController)
		{
			KeyCode enterKeyCode = op.first.first;

			if (enterKey == enterKeyCode)
				throw std::logic_error("logic_error: This key is already in use by another controller");
		}
		std::pair<KeyCode, KeyCode> keys = std::make_pair(enterKey, exitKey);
		keysToController.emplace_back(keys, std::move(controller));
	}
	catch (std::logic_error& err)
	{
		std::cout << err.what() << "\n";
	}
}

void ControllerDispatcher::addController(ButtonCode button, std::unique_ptr<Controller> controller)
{
	try
	{
		auto iter = buttonToController.emplace(button, std::move(controller));
		if (!iter.second)
			throw std::logic_error("logic_error: This mouse button is already in use by another controller");
	}
	catch (std::logic_error& err)
	{
		std::cout << err.what() << "\n";
	}
}

void ControllerDispatcher::addController(KeyCode key, std::unique_ptr<Controller> controller)
{
	try
	{
		auto iter = keyToController.emplace(key, std::move(controller));
		if (!iter.second)
			throw std::logic_error("logic_error: This key is already in use by another controller");
	}
	catch (std::logic_error& err)
	{
		std::cout << err.what() << "\n";
	}
}

void ControllerDispatcher::processMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
	onMouseInput(view, button, action, mods, x, y);
}

void ControllerDispatcher::processMouseMove(View& view, double x, double y)
{
	onMouseMove(view, x, y);
}

void ControllerDispatcher::processKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{
	onKeyboardInput(view, key, action, mods);
}

void ControllerDispatcher::onMouseMove(View& view, double x, double y)
{
	if (activeController.controllerPtr != nullptr)
		activeController.controllerPtr->onMouseMove(view, x, y);
}

void ControllerDispatcher::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
	auto controllerIter = buttonToController.find(button);
	if (controllerIter != buttonToController.end())
	{
		if (!activeController.hasExitButton)
			activeController.controllerPtr = controllerIter->second.get();
	}

	if (activeController.controllerPtr != nullptr)
		activeController.controllerPtr->onMouseInput(view, button, action, mods, x, y);
}

void ControllerDispatcher::onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods)
{
	for (const auto& op : keysToController)
	{
		KeyCode enterKeyCode = op.first.first;
		KeyCode exitKeyCode = op.first.second;

		if (key == enterKeyCode && action == Action::Press)
		{
			if (activeController.controllerPtr)
				activeController.controllerPtr->onExit(view);
			activeController.controllerPtr = op.second.get();
			activeController.hasExitButton = true;
			activeController.controllerPtr->onEnter(view);
		}
		else if (key == exitKeyCode)
		{
			if (activeController.controllerPtr == op.second.get())
			{
				activeController.controllerPtr->onExit(view);
				activeController.controllerPtr = nullptr;
				activeController.hasExitButton = false;
				return;
			}
		}
	}

	auto lambdaIter = keyToLambda.find(key);
	if (lambdaIter != keyToLambda.end())
	{
		lambdaIter->second(view, action, mods);
	}
	else
	{
		auto controllerIter = keyToController.find(key);
		if (controllerIter != keyToController.end())
		{
			auto& controllerPtr = controllerIter->second;
			controllerPtr->onKeyboardInput(view, key, action, mods);
		}
	}

	if (activeController.controllerPtr != nullptr)
		activeController.controllerPtr->onKeyboardInput(view, key, action, mods);
}
