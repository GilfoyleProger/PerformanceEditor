#include "addroadtilecontroller.h"
#include "contact.h"
#include "View/view.h"
#include <QDebug>
//#include "modelloader.h"
#include "modelloader.h"
void AddRoadTileController::updateNodes(const std::vector<std::unique_ptr<Node>>& nodes) 
{
    /*
    for (const auto& node : nodes)
    {
        if (node->getMesh() != nullptr)
        {

            std::string vertexShaderPath = "C:/Users/Stepan/Documents/workspace/PerfomanceEditor/Sources/shaders/model.vs";
            std::string fragmentShaderPath = "C:/Users/Stepan/Documents/workspace/PerfomanceEditor/Sources/shaders/model.fs";
            //std::string vertexShaderPath = "C:/Users/Stepan/Documents/Thesis/ViaFactorem/GLRenderEngine/shaders/vertexShader.shader";
            //std::string fragmentShaderPath = "C:/Users/Stepan/Documents/Thesis/ViaFactorem/GLRenderEngine/shaders/fragmentShader.shader";
            node->getMesh()->setShaderPaths(vertexShaderPath, fragmentShaderPath);
            //node->getMesh()->setShaderUniform("color", QVector3D(0, 0, 1));
        }
        updateNodes(node->getChildren());
    }
    */
}
void AddRoadTileController::onMouseInput(View& view, ButtonCode button, Action action, Modifier mods, double x, double y)
{
    if (action == Action::Release)
    {
        std::vector<Contact> contacts = view.raycast(x, y, FilterValue::Node);

        if (contacts.empty())
            return;

        Contact& contact = contacts.front();
        Node* node = contact.node;
        if (node)
        {
            /*
            Mesh* mesh = node->getMesh();
            if (mesh)
            {
                view.getOctree()->removeFaceFromNode(contact.face, node);
                mesh->deleteFace(contact.face);
            }*/
        }
    }
    if (action == Action::Release)
        {
          /*  std::vector<Contact> contacts = view.raycast(x, y, FilterValue::Node);

            if (contacts.empty())
                return;

            Contact& contact = contacts.front();
            Node* node = contact.node;
            if (node)
            {
                Mesh* mesh = node->getMesh();
                if (mesh)
                {*/
                   // ModelLoader loader;
                   // std::unique_ptr<Node> nodeFa= std::move(loader.loadModel("C:/Users/Stepan/Desktop/backpack/backpack.obj"));
                   // std::string vertexShaderPath = "C:/Users/Stepan/Documents/Thesis/ViaFactorem/GLRenderEngine/shaders/vertexShader.shader";
                   // std::string fragmentShaderPath = "C:/Users/Stepan/Documents/Thesis/ViaFactorem/GLRenderEngine/shaders/fragmentShader.shader";
                   // updateNodes(nodeFa->getChildren());
                   // for(auto& node : )
                  //  nodeFa->getChildren()[0]->getMesh()->setShaderPaths(vertexShaderPath, fragmentShaderPath);
                   // nodeFa->getChildren()[0]->getMesh()->setShaderUniform("color", QVector3D(0, 0, 1));
                    //nodeFa->getMesh()->setShaderPaths(vertexShaderPath,fragmentShaderPath );
                   // nodeFa->getMesh()->setShaderUniform("color", QVector3D(0, 0, 1));
                   // nodeFa->getMesh()->setMeshParam("color", QVector3D(0, 0, 1));
                    //view.modelPtr->attachNode(std::move(nodeFa));
                         /*
                    qDebug() << " x = " << contact.point.x << " y = " << contact.point.y << " z = " << contact.point.z << "\n";
                    HalfEdge::HalfEdgeTable halfEdgeTable;
                    double cubeSideLength = 1.0; 

                    if (cubeSideLength > 0)
                    {
                        double halfEdge = cubeSideLength / 2.0;
                        std::vector<HalfEdge::VertexHandle> cubeVertices;

                        cubeVertices.emplace_back(halfEdgeTable.addVertex({ halfEdge, -halfEdge,  halfEdge }));
                        cubeVertices.emplace_back(halfEdgeTable.addVertex({ halfEdge, -halfEdge, -halfEdge }));
                        cubeVertices.emplace_back(halfEdgeTable.addVertex({ -halfEdge, -halfEdge, -halfEdge }));
                        cubeVertices.emplace_back(halfEdgeTable.addVertex({ -halfEdge, -halfEdge,  halfEdge }));
                        cubeVertices.emplace_back(halfEdgeTable.addVertex({ halfEdge,  halfEdge,  halfEdge }));
                        cubeVertices.emplace_back(halfEdgeTable.addVertex({ halfEdge,  halfEdge, -halfEdge }));
                        cubeVertices.emplace_back(halfEdgeTable.addVertex({ -halfEdge,  halfEdge, -halfEdge }));
                        cubeVertices.emplace_back(halfEdgeTable.addVertex({ -halfEdge,  halfEdge,  halfEdge }));

                        std::vector<int> indices = { 4, 5, 6, 7,   // top
                                                     3, 7, 6, 2,   // left
                                                     2, 6, 5, 1,   // back
                                                     1, 0, 3, 2,   // bottom
                                                     0, 4, 7, 3,   // front
                                                     1, 5, 4, 0 }; // right

                        for (int i = 0; i < indices.size(); i += 4)
                        {
                            halfEdgeTable.addFace(cubeVertices[indices[i]],
                                cubeVertices[indices[i + 1]],
                                cubeVertices[indices[i + 2]],
                                cubeVertices[indices[i + 3]]);
                        }
                        halfEdgeTable.connectTwins();



                    }
                    std::unique_ptr<Mesh> mesh1 = std::make_unique<Mesh>(halfEdgeTable);
                    std::unique_ptr<Node> node1 = std::make_unique<Node>();
                    node1->attachMesh(std::move(mesh1));
                    //view.modelPtr = std::make_unique<Model>();
                    view.modelPtr->attachNode(std::move(node1));*/
                   // view.getOctree()->removeFaceFromNode(contact.face, node);
                   // mesh->deleteFace(contact.face);
                }
            //}
       // }
}
