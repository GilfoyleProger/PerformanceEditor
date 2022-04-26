#pragma once

#include "controller.h"

#include <functional>
#include <iostream>
#include <vector>
#include <memory>
#include <map>

class ControllerDispatcher : public Controller
{
public:
	friend class View;

	void addController(KeyCode enterKey, KeyCode exitKey, std::unique_ptr<Controller> controller);
	void addController(ButtonCode button, std::unique_ptr<Controller> controller);
	void addController(KeyCode key, std::unique_ptr<Controller> controller);

	template<class Lambda>
	void addController(KeyCode key, Lambda lambda)
	{
		try
		{
			auto iter = keyToLambda.emplace(key, lambda);
			if (!iter.second)
				throw std::logic_error("logic_error: This key is already in use by another controller");
		}
		catch (std::logic_error& err)
		{
			std::cout << err.what() << "\n";
		}
	}
private:
	void processMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y);
	void processMouseMove(View& view, double x, double y);
	void processKeyboardInput(View& view, KeyCode key, Action action, Modifier mods);

	void onMouseMove(View& view, double x, double y) override;
	void onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y) override;
	void onKeyboardInput(View& view, KeyCode key, Action action, Modifier mods) override;

	using KeyCallback = std::function<void(View&, Action, Modifier)>;

	std::map<ButtonCode, std::unique_ptr<Controller>> buttonToController;
	std::map<KeyCode, std::unique_ptr<Controller>> keyToController;
	std::map<KeyCode, KeyCallback> keyToLambda;

	std::vector<std::pair<std::pair<KeyCode, KeyCode>, std::unique_ptr<Controller>>> keysToController;

	struct ActiveController
	{
		Controller* controllerPtr = nullptr;
		bool hasExitButton = false;
	};
	ActiveController activeController;
};
