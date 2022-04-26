#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <map>

namespace HalfEdge
{
    struct HalfEdgeHandle { int64_t index = -1; };
    struct VertexHandle { int64_t index = -1; };
    struct FaceHandle { int64_t index = -1; };

    struct HalfEdge
    {
        //The face it belongs to, is invalid (== -1) if a boundary half-edge
        FaceHandle fh;
        //The vertex it points to is always valid
        VertexHandle dst;
        HalfEdgeHandle twin;
        //The next HalfEdge in the CCW order is always valid
        HalfEdgeHandle next;
        //The previous HalfEdge in the CCW order can be stored for the optimization purposes.
        //For the triangle meshes prev = next->next->next
        HalfEdgeHandle prev;
    };

    struct Face
    {
        //One of the HalfEdges belonging to the Face, always valid
        HalfEdgeHandle heh;
        glm::vec3 normal{ 1,1,1 };
        std::vector<glm::vec2> textureCoords;
    };

    struct Vertex
    {
        //An outgoing HalfEdge from this vertex. It is == -1 if the vertex is isolated
        HalfEdgeHandle heh;
        //glm::vec3 normal {1,1,1};
       // glm::vec2 textureCoords{0,0};
    };

    class HalfEdgeTable
    {
    public:
        //Adds vertex
        VertexHandle addVertex(glm::vec3 position);
        //Adds triangulated face
        FaceHandle addFace(VertexHandle vh0, VertexHandle vh1, VertexHandle vh2);
        //Adds quad face
        FaceHandle addFace(VertexHandle vh0, VertexHandle vh1, VertexHandle vh2, VertexHandle vh3);
        //Builds twins for half-edges. This function must be called in the end
        void connectTwins();

        //Deletes face and re-link all vertices, half-edges, faces.
        void deleteFace(FaceHandle fh);
    public:
        //For a given half-edge returns previous linked half-edge
        HalfEdgeHandle prev(HalfEdgeHandle heh) const;
        //For a given half-edge returns next linked half-edge
        HalfEdgeHandle next(HalfEdgeHandle heh) const;
        //For a given half-edge returns twin half-edge
        HalfEdgeHandle twin(HalfEdgeHandle heh) const;
        //For a given half-edge returns end vertex
        VertexHandle destVertex(HalfEdgeHandle heh) const;
        //For a given half-edge returns start vertex
        VertexHandle sourceVertex(HalfEdgeHandle heh) const;

        //For a given half-edge handle returns half-edge
        HalfEdge& deref(HalfEdgeHandle vh);
        const HalfEdge& deref(HalfEdgeHandle vh) const;
        //For a given half-edge returns half-edge handle
        HalfEdgeHandle handle(const HalfEdge& v) const;

        Vertex& deref(VertexHandle vh);
        const Vertex& deref(VertexHandle vh) const;
        VertexHandle handle(const Vertex& v) const;

        Face& deref(FaceHandle fh);
        const Face& deref(FaceHandle fh) const;
        FaceHandle handle(const Face& f) const;

        //For a given vertex handle set/get point
        const glm::vec3& getPoint(VertexHandle handle) const;
        void setPoint(VertexHandle handle, glm::vec3 data);

        //For a given half-edge handle set/get start point (vertex)
        const glm::vec3& getStartPoint(HalfEdgeHandle handle) const;
        void setStartPoint(HalfEdgeHandle handle, glm::vec3 data);

        //For a given half-edge handle set/get end point (vertex)
        const glm::vec3& getEndPoint(HalfEdgeHandle handle) const;
        void setEndPoint(HalfEdgeHandle handle, glm::vec3 data);

        const std::vector<Vertex>& getVertices() const;
        const std::vector<Face>& getFaces() const;

        friend HalfEdgeTable& operator += (HalfEdgeTable& first, const HalfEdgeTable& second);
        friend HalfEdgeTable operator + (HalfEdgeTable first, const HalfEdgeTable& second);
    private:
        std::vector<Vertex> vertices;
        std::vector<HalfEdge> halfEdges;
        std::vector<Face> faces;
        std::vector<glm::vec3> points;
        //Heh and its pair of initial and final vertices
        std::map<std::pair<int, int>, HalfEdgeHandle> hehsStartEnd;

        //Create half-edge from its start and end vertices
        HalfEdgeHandle createHalfEdge(VertexHandle start, VertexHandle end);

        //For a given vertex handle get outgoing half-edge
        HalfEdgeHandle outgoingHalfEdge(VertexHandle vh);

        //For a given heh set next/prev and face it belongs to
        void initHalfEdge(HalfEdgeHandle heh, HalfEdgeHandle prev, HalfEdgeHandle next, FaceHandle fh, VertexHandle start, VertexHandle end);

        //The half-edge is at the boundary, if it doesn't belong to any face
        inline bool isBoundary(HalfEdgeHandle heh);
        //The vertex is boundary, if it belong to boundary half-edge
        //inline bool isBoundary(VertexHandle heh);

        //Adjust outgoing halfedge handle for vertices,
        //so that it is a boundary halfedge whenever possible
        void adjustOutgoingHalfEdge(VertexHandle vh);

        //Relinks heh, next/prev, and vertex handles for the given halfEdge heh
        void relinkHalfEdge(HalfEdgeHandle heh);

        //Remove half-edges that have non-initialized fields
        //and return map with mapping between old hehs and new ones
        std::map<int, int> removeNonInitializedHalfEdges();

        //Remove vertices that have non-initialized fields
        //and return map with mapping between old vhs and new ones
        std::map<int, int> removeNonInitializedVertices();

        //Go through half-edges and replace old handles with new ones
        void updateHalfEdgesHandles(FaceHandle deletedFace,
                                    std::map<int, int> oldToNewVh,
                                    std::map<int, int> oldToNewHeh);

        //Go through vertices and replace old handles with new ones
        void updateVerticesHandles(std::map<int, int> oldToNewHeh);

        //Go through faces and replace old handles with new ones
        void updateFacesHandles(std::map<int, int> oldToNewHeh);

        //From map update hehs and its pair of start and end vertices too
        void updateHehsStartEndMap();

        //Remove half-edges of face, which creates a non-manifold situation.
        //Insert a new face with new vertex handles to make this face separate from the rest.
        void updateNonManifold(std::vector<VertexHandle> vertexHandles, std::vector<HalfEdgeHandle> edgeHandles);
    };
}
