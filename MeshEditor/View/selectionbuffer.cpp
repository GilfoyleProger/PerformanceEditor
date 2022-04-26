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
		nodeInfo->setSelected(true);
		items.push_back(nodeInfo);
	}
	emit selectionBufferChanged();
}

void SelectionBuffer::deselect(NodeInfo& nodeInfo, bool recursively)
{
	if (recursively)
	{
		setSelectRecursively(&nodeInfo, false);
	}
	else
	{
		nodeInfo.setSelected(false);
	}
}

bool SelectionBuffer::isSelected(NodeInfo& nodeInfo, bool requireAllChilds)
{
	if (requireAllChilds)
	{
		return isAllChildSelected(&nodeInfo);
	}
	else
	{
        return nodeInfo.selected();//std::find(items.begin(), items.end(), &nodeInfo) != items.end();
	}
}

void SelectionBuffer::clear()
{
	for (auto& item : items)
	{
		item->setSelected(false);
        emit item->selectedChanged();
	}
	items.clear();
}

bool SelectionBuffer::isEmpty()
{
	return items.empty();
}

void SelectionBuffer::setSelectRecursively(NodeInfo* nodeInfo, bool state)
{
	nodeInfo->setSelected(true);
	items.push_back(nodeInfo);
	setSelectRecursively(nodeInfo->getChildren(), true);
}

void SelectionBuffer::setSelectRecursively(std::vector<NodeInfo*> nodes, bool state)
{
	for (const auto& node : nodes)
	{
		node->setSelected(true);
		items.push_back(node);
		setSelectRecursively(node->getChildren(), true);
	}
}

bool SelectionBuffer::isAllChildSelected(NodeInfo* nodeInfo)
{
	bool childsSelected = false;
	isAllChildSelected(nodeInfo->getChildren(), childsSelected);
    bool nodeSelected = nodeInfo->selected(); //std::find(items.begin(), items.end(), nodeInfo) != items.end();
	return nodeSelected && childsSelected;
}

void SelectionBuffer::isAllChildSelected(std::vector<NodeInfo*> nodes, bool& childsSelected)
{
	for (auto node : nodes)
	{
        bool isItemExist = node->selected();//std::find(items.begin(), items.end(), node) != items.end();
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
