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
	void deselect(NodeInfo& entityOccurrence, bool recursively = false);
	bool isSelected(NodeInfo& entityOccurrence, bool requireChilds = false);
	void clear();
	bool isEmpty();
	std::vector<NodeInfo*> getItems() { return items; }
signals:
	void selectionBufferChanged();
private:
	SelectionBuffer() = default;
	std::vector<NodeInfo*> items;
	//NodeInfo* nodeToEdit = nullptr;


	void setSelectRecursively(NodeInfo* nodeInfo, bool state);
	void setSelectRecursively(std::vector<NodeInfo*> nodesInfo, bool state);
	void isAllChildSelected(std::vector<NodeInfo*> nodesInfo, bool& childsSelected);
    bool isAllChildSelected(NodeInfo* nodeInfo);
};
