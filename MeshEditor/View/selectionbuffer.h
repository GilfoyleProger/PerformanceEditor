#pragma once

#include "modelstore.h"
#include <QObject>

class SelectionBuffer : public QObject
{
	Q_OBJECT
public:
	SelectionBuffer(const SelectionBuffer&) = delete;
	SelectionBuffer& operator=(const SelectionBuffer&) = delete;
	static SelectionBuffer& get();
	void select(NodeInfo* nodeInfo, bool recursively = false);
	void deselect(NodeInfo* nodeInfo, bool recursively = false);
	bool isSelected(NodeInfo* nodeInfo, bool requireChilds = false);
	void clear();
	bool isEmpty();
	std::vector<NodeInfo*> getItems() { return items; }
	bool isOneChildSelected(NodeInfo* nodeInfo);
	bool isAllChildSelected(NodeInfo* nodeInfo);
signals:
	void selectionBufferChanged();
private:
	SelectionBuffer() = default;
	std::vector<NodeInfo*> items;

	void setSelectRecursively(NodeInfo* nodeInfo, bool state);
	void setSelectRecursively(std::vector<NodeInfo*> nodesInfo, bool state);
	void isAllChildSelected(std::vector<NodeInfo*> nodesInfo, bool& childsSelected);
	void isOneChildSelected(std::vector<NodeInfo*> nodesInfo, bool& childsSelected);
};
