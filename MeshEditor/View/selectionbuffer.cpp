#include "selectionbuffer.h"
#include <algorithm>

SelectionBuffer& SelectionBuffer::get()
{
	static SelectionBuffer selectionBuffer;
	return selectionBuffer;
}

void SelectionBuffer::select(NodeInfo* nodeInfo, bool recursively)
{
	if (recursively)
	{
		setSelectRecursively(nodeInfo, true);
	}
	else
	{
		items.push_back(nodeInfo);
		emit nodeInfo->selectedChanged();
	}
	emit selectionBufferChanged();
}

void SelectionBuffer::deselect(NodeInfo* nodeInfo, bool recursively)
{
	if (recursively)
	{
		setSelectRecursively(nodeInfo, false);
	}
	else
	{
		auto item = std::find(std::begin(items), std::end(items), nodeInfo);
		if (item != items.end())
		{
			NodeInfo* node = *item;
			items.erase(item);
			emit node->selectedChanged();
		}
	}
	emit selectionBufferChanged();
}

bool SelectionBuffer::isSelected(NodeInfo* nodeInfo, bool requireAllChilds)
{
	if (requireAllChilds)
	{
		return isAllChildSelected(nodeInfo);
	}
	else
	{
		return  std::find(items.begin(), items.end(), nodeInfo) != items.end();
	}
}

void SelectionBuffer::clear()
{
	while (!items.empty())
	{
		NodeInfo* node = items.front();
		items.erase(items.begin());
		emit node->selectedChanged();
	}
	emit selectionBufferChanged();
}

bool SelectionBuffer::isEmpty()
{
	return items.empty();
}

void SelectionBuffer::setSelectRecursively(NodeInfo* nodeInfo, bool state)
{
	if (state)
	{
		items.push_back(nodeInfo);
		emit nodeInfo->selectedChanged();
	}
	else
	{
		auto item = std::find(std::begin(items), std::end(items), nodeInfo);
		if (item != items.end())
		{
			NodeInfo* node = *item;
			items.erase(item);
			emit node->selectedChanged();
		}
	}
	setSelectRecursively(nodeInfo->getChildren(), state);
	emit selectionBufferChanged();
}

void SelectionBuffer::setSelectRecursively(std::vector<NodeInfo*> nodes, bool state)
{
	for (const auto& node : nodes)
	{
		if (state)
		{
			items.push_back(node);
		}
		else
		{
			auto item = std::find(std::begin(items), std::end(items), node);
			if (item != items.end())
			{
				NodeInfo* node = *item;
				items.erase(item);
				emit node->selectedChanged();
			}
		}
		setSelectRecursively(node->getChildren(), state);
	}
}

bool SelectionBuffer::isAllChildSelected(NodeInfo* nodeInfo)
{
	bool childsSelected = false;
	isAllChildSelected(nodeInfo->getChildren(), childsSelected);
	bool nodeSelected = std::find(items.begin(), items.end(), nodeInfo) != items.end();
	return nodeSelected && childsSelected;
}

void SelectionBuffer::isAllChildSelected(std::vector<NodeInfo*> nodes, bool& childsSelected)
{
	for (auto node : nodes)
	{
		bool isItemExist = std::find(items.begin(), items.end(), node) != items.end();
		if (!isItemExist)
		{
			childsSelected = false;
			break;
		}
		else
		{
			childsSelected = true;
			isAllChildSelected(node->getChildren(), childsSelected);
		}
	}
}
bool SelectionBuffer::isOneChildSelected(NodeInfo* nodeInfo)
{
	bool childsSelected = false;
	isOneChildSelected(nodeInfo->getChildren(), childsSelected);
	bool nodeSelected = std::find(items.begin(), items.end(), nodeInfo) != items.end();
	return nodeSelected || childsSelected;
}
void SelectionBuffer::isOneChildSelected(std::vector<NodeInfo*> nodes, bool& childsSelected)
{
	if (childsSelected)
		return;

	for (auto node : nodes)
	{
		bool isItemExist = std::find(items.begin(), items.end(), node) != items.end();
		if (isItemExist)
		{
			childsSelected = true;
			return;
		}
		else if (!childsSelected)
		{
			childsSelected = false;
			isOneChildSelected(node->getChildren(), childsSelected);
		}
	}
}
