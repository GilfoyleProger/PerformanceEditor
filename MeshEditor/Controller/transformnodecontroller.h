#pragma once

#include "Manipulator.h"
#include "controller.h"
#include "Contact.h"
#include "Triad.h"
//#include "View/view.h"
#include <map>
//#include <QObject>
class View;
class TransformNodeOperator : public Controller//, public QObject
{
    //Q_OBJECT
//public:
    //   explicit TransformNodeOperator() {}
private:
   // 
    void onEnter(View& view) override;
    void onExit(View& view) override;

    void onMouseMove(View& view, double x, double y) override;
    void onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y) override;

    enum class State { Idle, Edit };

    Manipulator* currentManipulator;
    State currentSate;
    Node* nodeToEdit;
    Triad* triadPtr;

    void initTriad(View& view);
    void resetTriad(View& view);
    void calcRotationMat();
    void calcScaleMat(View& view);
    void calcTranslationMat();

    glm::mat4 prevDelta = glm::mat4(1.0f);

    View* mview=nullptr;
    bool isActive = false;
//public slots:
    void activateManipulator();
// slots:
 //signals:
//public:
    //void activateManipulator();
};

