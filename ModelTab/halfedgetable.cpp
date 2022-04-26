#include "halfedgetable.h"

namespace HalfEdge
{
    VertexHandle HalfEdgeTable::addVertex(glm::vec3 position)
    {
        points.emplace_back(position);
        return handle(vertices.emplace_back());
    }

    HalfEdgeHandle HalfEdgeTable::createHalfEdge(VertexHandle start, VertexHandle end)
    {
        // Save the pair of (start, end) vertices to pairsMap
        auto iter = hehsStartEnd.emplace(std::make_pair(start.index, end.index), HalfEdgeHandle());
        auto &item = iter.first;

        if (iter.second)
        {
            HalfEdge he;
            he.dst = end;
            item->second = handle(halfEdges.emplace_back(he));

            return item->second;
        }
        else
        {
            if (!isBoundary(item->second)) // Check for non-manifold
                return HalfEdgeHandle();

            return item->second;
        }
    }

    void HalfEdgeTable::initHalfEdge(HalfEdgeHandle heh, HalfEdgeHandle next, HalfEdgeHandle prev, FaceHandle fh, VertexHandle start, VertexHandle end)
    {
        deref(heh).fh = fh;
        deref(heh).next = next;
        deref(heh).prev = prev;
        deref(heh).dst = end;
        deref(start).heh = heh;
    }

    inline bool HalfEdgeTable::isBoundary(HalfEdgeHandle heh)
    {
        return deref(heh).fh.index == -1;
    }

    void HalfEdgeTable::adjustOutgoingHalfEdge(VertexHandle vh)
    {
        HalfEdgeHandle heh = deref(vh).heh;
        HalfEdgeHandle nextHeh = heh;
        HalfEdgeHandle startHeh;

        while (nextHeh.index != startHeh.index)
        {
            if (isBoundary(nextHeh))
            {
                deref(vh).heh = nextHeh;
                break;
            }

            nextHeh = next(twin(nextHeh));

            if (startHeh.index == -1)
                startHeh = heh;
        }
    }

    HalfEdgeHandle HalfEdgeTable::outgoingHalfEdge(VertexHandle vh)
    {
        return deref(vh).heh;
    }

    FaceHandle HalfEdgeTable::addFace(VertexHandle vh0, VertexHandle vh1, VertexHandle vh2)
    {
        // create new half-edges
        HalfEdgeHandle heh0 = createHalfEdge(vh0, vh1);
        HalfEdgeHandle heh1 = createHalfEdge(vh1, vh2);
        HalfEdgeHandle heh2 = createHalfEdge(vh2, vh0);

        if (heh0.index == -1 || heh1.index == -1 || heh2.index == -1)
        {
            updateNonManifold({ vh0, vh1, vh2 }, { heh0, heh1, heh2 });
            return FaceHandle();
        }

        // create new face
        Face face;
        // set vh0's half edge as halfEdge belonging to the Face
        face.heh = heh0;
        FaceHandle fh = handle(faces.emplace_back(face));

        // set next/prev for edges and face they belongs
        initHalfEdge(heh0, heh1, heh2, fh, vh0, vh1);
        initHalfEdge(heh1, heh2, heh0, fh, vh1, vh2);
        initHalfEdge(heh2, heh0, heh1, fh, vh2, vh0);

        return fh;
    }

    FaceHandle HalfEdgeTable::addFace(VertexHandle vh0, VertexHandle vh1, VertexHandle vh2, VertexHandle vh3)
    {
        // create new half-edges
        HalfEdgeHandle heh0 = createHalfEdge(vh0, vh1);
        HalfEdgeHandle heh1 = createHalfEdge(vh1, vh2);
        HalfEdgeHandle heh2 = createHalfEdge(vh2, vh3);
        HalfEdgeHandle heh3 = createHalfEdge(vh3, vh0);

        if (heh0.index == -1 || heh1.index == -1 || heh2.index == -1 || heh3.index == -1)
        {
            updateNonManifold({ vh0, vh1, vh2, vh3 }, { heh0, heh1, heh2, heh3 });
            return FaceHandle();
        }

        // create new face
        Face face;
        // set vh0's half edge as halfEdge belonging to the Face
        face.heh = heh0;
        FaceHandle fh = handle(faces.emplace_back(face));

        // set next/prev for edges and face they belongs
        initHalfEdge(heh0, heh1, heh3, fh, vh0, vh1);
        initHalfEdge(heh1, heh2, heh0, fh, vh1, vh2);
        initHalfEdge(heh2, heh3, heh1, fh, vh2, vh3);
        initHalfEdge(heh3, heh0, heh2, fh, vh3, vh0);

        return fh;
    }

    void HalfEdgeTable::connectTwins()
    {
        // Traverse all half-edges from map and check
        // if there is a half-edge with the vertices (end, start)
        std::vector<HalfEdgeHandle> boundHalfEdges;
        for (const auto& pair : hehsStartEnd)
        {
            int start = pair.first.first;
            int end = pair.first.second;
            std::pair<int, int> reversePair = { end, start };

            auto twin = hehsStartEnd.find(reversePair);

            if (twin != hehsStartEnd.end())
            {
                // are twins
                deref(pair.second).twin = twin->second;
                deref(twin->second).twin = pair.second;
            }
            else
            {
                // then create twin
                HalfEdgeHandle newTwin = handle(halfEdges.emplace_back());
                deref(pair.second).twin = newTwin;
                deref(newTwin).twin = pair.second;

                // dest vertex for twin
                VertexHandle twinDst = handle(vertices[start]);
                deref(newTwin).dst = twinDst;
                // set twin as outgoing for its start vertex
                VertexHandle twinSrc = handle(vertices[end]);
                deref(twinSrc).heh = newTwin;

                // Save the pair of twins (end, start) vertices to pairsMap
                hehsStartEnd.emplace(reversePair, newTwin);
                boundHalfEdges.emplace_back(newTwin);
            }
        }

        // Connect outer half-edges
        for (const auto& boundHeh : boundHalfEdges)
        {
            // Search the closest outgoing half-edge from the end of boundaryHeh
            HalfEdgeHandle innerNext = twin(boundHeh);
            do
            {
                innerNext = twin(prev(innerNext));
            } while (!isBoundary(innerNext));

            deref(boundHeh).next = innerNext;
            deref(innerNext).prev = boundHeh;

            VertexHandle boundHehEndVertex = destVertex(boundHeh);
            if (prev(outgoingHalfEdge(boundHehEndVertex)).index == -1)
            {
                deref(boundHehEndVertex).heh = innerNext;
            }
            else
            {
                // Make boundHeh and outgoing heh from the end of boundHeh adjacent
                HalfEdgeHandle hehFromBoundHehEnd = outgoingHalfEdge(boundHehEndVertex);
                HalfEdgeHandle prevHehFromBoundHehEnd = prev(hehFromBoundHehEnd);

                HalfEdgeHandle boundHehNext = next(boundHeh);
                deref(boundHeh).next = hehFromBoundHehEnd;
                deref(hehFromBoundHehEnd).prev = boundHeh;

                deref(prevHehFromBoundHehEnd).next = boundHehNext;
                deref(boundHehNext).prev = prevHehFromBoundHehEnd;
            }
        }
    }

    void HalfEdgeTable::relinkHalfEdge(HalfEdgeHandle heh)
    {
        HalfEdgeHandle h0 = heh;
        VertexHandle v0 = destVertex(h0);
        HalfEdgeHandle next0 = next(h0);
        HalfEdgeHandle prev0 = prev(h0);

        HalfEdgeHandle h1 = twin(heh);
        VertexHandle v1 = destVertex(h1);
        HalfEdgeHandle next1 = next(h1);
        HalfEdgeHandle prev1 = prev(h1);

        deref(prev0).next = next1;
        deref(next1).prev = prev0;

        deref(prev1).next = next0;
        deref(next0).prev = prev1;

        deref(h0).dst.index = -1; // mark h0 as deleted
        deref(h1).dst.index = -1; // mark h1 as deleted

        if (deref(v0).heh.index == h1.index)
        {
            if (next0.index == h1.index)
                deref(v0).heh.index = -1;
            else
                deref(v0).heh = next0;
        }

        if (deref(v1).heh.index == h0.index)
        {
            if (next1.index == h0.index)
                deref(v1).heh.index = -1;
            else
                deref(v1).heh.index = next1.index;
        }
    }

    std::map<int, int> HalfEdgeTable::removeNonInitializedHalfEdges()
    {
        // mapping between old half-edge handles and new ones
        std::map<int, int> oldToNewHeh;

        bool isFoundNonInitializedHalfEdge = false;
        std::vector<HalfEdge> newHalfEdges;
        int newHehIndex = 0;

        for (int i = 0; i < halfEdges.size(); i++)
        {
            if (halfEdges[i].dst.index != -1)
            {
                newHalfEdges.emplace_back(halfEdges[i]);

                if (isFoundNonInitializedHalfEdge)
                    oldToNewHeh.emplace(i, newHehIndex);

                ++newHehIndex;
            }
            else
            {
                isFoundNonInitializedHalfEdge = true;
            }
        }
        halfEdges = newHalfEdges;

        return oldToNewHeh;
    }

    std::map<int, int> HalfEdgeTable::removeNonInitializedVertices()
    {
        // mapping between old vertices handles and new ones
        std::map<int, int> oldToNewVh;
        const int initPointsCount = points.size();

        bool isFoundNonInitializedVertex = false;
        std::vector<Vertex> newVertices;
        int newVhIndex = 0;

        for (int i = 0; i < vertices.size(); i++)
        {
            if (vertices[i].heh.index != -1)
            {
                newVertices.emplace_back(vertices[i]);

                if (isFoundNonInitializedVertex)
                    oldToNewVh.emplace(i, newVhIndex);

                ++newVhIndex;
            }
            else
            {
                isFoundNonInitializedVertex = true;

                int indexToDelete = i - (initPointsCount - points.size());
                points.erase(points.begin() + indexToDelete);
            }
        }
        vertices = newVertices;

        return oldToNewVh;
    }

    void HalfEdgeTable::updateHalfEdgesHandles(FaceHandle deletedFace,
                                               std::map<int, int> oldToNewVh,
                                               std::map<int, int> oldToNewHeh)
    {
        //Go through half-edges and replace old handles with new ones
        for (auto& he : halfEdges)
        {
            if (he.fh.index > deletedFace.index)
                he.fh.index -= 1;

            auto foundTwin = oldToNewHeh.find(he.twin.index);
            if (foundTwin != oldToNewHeh.end())
                he.twin.index = foundTwin->second;

            auto foundNext = oldToNewHeh.find(he.next.index);
            if (foundNext != oldToNewHeh.end())
                he.next.index = foundNext->second;

            auto foundPrev = oldToNewHeh.find(he.prev.index);
            if (foundPrev != oldToNewHeh.end())
                he.prev.index = foundPrev->second;

            auto foundDst = oldToNewVh.find(he.dst.index);
            if (foundDst != oldToNewVh.end())
                he.dst.index = foundDst->second;
        }
    }

    void HalfEdgeTable::updateVerticesHandles(std::map<int, int> oldToNewHeh)
    {
        if (!oldToNewHeh.empty())
        {
            for (auto& v : vertices)
            {
                auto foundHeh = oldToNewHeh.find(v.heh.index);
                if (foundHeh != oldToNewHeh.end())
                    v.heh.index = foundHeh->second;
            }
        }
    }

    void HalfEdgeTable::updateFacesHandles(std::map<int, int> oldToNewHeh)
    {
        if (!oldToNewHeh.empty())
        {
            for (auto& face : faces)
            {
                auto foundHeh = oldToNewHeh.find(face.heh.index);
                if (foundHeh != oldToNewHeh.end())
                    face.heh.index = foundHeh->second;
            }
        }
    }

    void HalfEdgeTable::updateHehsStartEndMap()
    {
        std::map<std::pair<int, int>, HalfEdgeHandle> newHehsStartEnd;
        int i = 0;
        for (const auto& he : halfEdges)
        {
            ++i;
            VertexHandle startVertex = sourceVertex(handle(he));
            VertexHandle endVertex = destVertex(handle(he));
            std::pair<int, int> p = { startVertex.index, endVertex.index };
            newHehsStartEnd.emplace(p, handle(he));
        }

        hehsStartEnd = newHehsStartEnd;
    }

    void HalfEdgeTable::updateNonManifold(std::vector<VertexHandle> vertexHandles, std::vector<HalfEdgeHandle> edgeHandles)
    {
        std::vector<VertexHandle> newVertexHandles;

        for (int i = 0; i < vertexHandles.size(); i++)
        {
            if (deref(vertexHandles[i]).heh.index != -1)
                newVertexHandles.emplace_back(addVertex(getPoint(vertexHandles[i])));
            else
                newVertexHandles.emplace_back(vertexHandles[i]);
        }

        int removedEdgesCount = 0;
        for (int i = 0; i < vertexHandles.size(); i++)
        {
            VertexHandle start = vertexHandles[i];
            VertexHandle end = vertexHandles[((i + 1) == vertexHandles.size()) ? 0 : (i + 1)];

            if (edgeHandles[i].index != -1)
            {
                auto iter = hehsStartEnd.find({ start.index, end.index });
                halfEdges.erase(halfEdges.begin() + (iter->second.index - removedEdgesCount++));
                hehsStartEnd.erase(iter);
            }
        }

        if (newVertexHandles.size() == 3)
            addFace(newVertexHandles[0], newVertexHandles[1], newVertexHandles[2]);
        else if (newVertexHandles.size() == 4)
            addFace(newVertexHandles[0], newVertexHandles[1], newVertexHandles[2], newVertexHandles[3]);
    }

    void HalfEdgeTable::deleteFace(FaceHandle fh)
    {
        std::vector<VertexHandle> tempVertices;
        std::vector<HalfEdgeHandle> tempHalfEdges;

        HalfEdgeHandle heh = deref(fh).heh;
        HalfEdgeHandle nextHeh = heh;
        HalfEdgeHandle startHeh;

        while (nextHeh.index != startHeh.index)
        {
            deref(nextHeh).fh.index = -1;
            tempVertices.emplace_back(deref(nextHeh).dst);

            if (isBoundary(twin(nextHeh)))
                tempHalfEdges.emplace_back(nextHeh);

            nextHeh = next((nextHeh));

            if (startHeh.index == -1)
                startHeh = heh;
        }

        for (const auto& heh : tempHalfEdges)
        {
            relinkHalfEdge(heh);
        }

        for (const auto& vh : tempVertices)
        {
            if (deref(vh).heh.index != -1)
                adjustOutgoingHalfEdge(vh);
        }

        faces.erase(fh.index + faces.begin()); //Remove face fh from faces array

        //Mapping between old half-edge handles and new ones
        std::map<int, int> oldToNewHeh = removeNonInitializedHalfEdges();

        //Mapping between old vertices handles and new ones
        std::map<int, int> oldToNewVh = removeNonInitializedVertices();

        //Go through half-edges, faces and replace old handles with new ones
        updateHalfEdgesHandles(fh, oldToNewVh, oldToNewHeh);

        //Go through vertices and replace old handles with new ones
        updateVerticesHandles(oldToNewHeh);

        //Go through faces and replace old handles with new ones
        updateFacesHandles(oldToNewHeh);

        // update heh and its pair of start and end vertices too
        updateHehsStartEndMap();
    }

    HalfEdgeHandle HalfEdgeTable::prev(HalfEdgeHandle heh) const
    {
        return deref(heh).prev;
    }

    HalfEdgeHandle HalfEdgeTable::next(HalfEdgeHandle heh) const
    {
        return deref(heh).next;
    }

    HalfEdgeHandle HalfEdgeTable::twin(HalfEdgeHandle heh) const
    {
        return deref(heh).twin;
    }

    VertexHandle HalfEdgeTable::destVertex(HalfEdgeHandle heh) const
    {
        return deref(heh).dst;
    }

    VertexHandle HalfEdgeTable::sourceVertex(HalfEdgeHandle heh) const
    {
        return destVertex(twin(heh));
    }

    HalfEdge & HalfEdgeTable::deref(HalfEdgeHandle vh)
    {
        return halfEdges[vh.index];
    }

    const HalfEdge & HalfEdgeTable::deref(HalfEdgeHandle vh) const
    {
        return halfEdges[vh.index];
    }

    HalfEdgeHandle HalfEdgeTable::handle(const HalfEdge & v) const
    {
        return { static_cast<int>(&v - &halfEdges[0]) };
    }

    Vertex & HalfEdgeTable::deref(VertexHandle vh)
    {
        return vertices[vh.index];
    }

    const Vertex & HalfEdgeTable::deref(VertexHandle vh) const
    {
        return vertices[vh.index];
    }

    VertexHandle HalfEdgeTable::handle(const Vertex & v) const
    {
        return { static_cast<int>(&v - &vertices[0]) };
    }

    Face & HalfEdgeTable::deref(FaceHandle fh)
    {
        return faces[fh.index];
    }

    const Face & HalfEdgeTable::deref(FaceHandle fh) const
    {
        return faces[fh.index];
    }

    FaceHandle HalfEdgeTable::handle(const Face & f) const
    {
        return { static_cast<int>(&f - &faces[0]) };
    }

    const glm::vec3 & HalfEdgeTable::getPoint(VertexHandle handle) const
    {
        return points[handle.index];
    }

    void HalfEdgeTable::setPoint(VertexHandle handle, glm::vec3 data)
    {
        points[handle.index] = data;
    }

    const glm::vec3 & HalfEdgeTable::getStartPoint(HalfEdgeHandle handle) const
    {
        VertexHandle vh = sourceVertex(handle);
        return getPoint(vh);
    }

    void HalfEdgeTable::setStartPoint(HalfEdgeHandle handle, glm::vec3 data)
    {
        VertexHandle vh = sourceVertex(handle);
        setPoint(vh, data);
    }

    const glm::vec3 & HalfEdgeTable::getEndPoint(HalfEdgeHandle handle) const
    {
        VertexHandle vh = destVertex(handle);
        return getPoint(vh);
    }

    void HalfEdgeTable::setEndPoint(HalfEdgeHandle handle, glm::vec3 data)
    {
        VertexHandle vh = destVertex(handle);
        setPoint(vh, data);
    }

    const std::vector<Vertex>& HalfEdgeTable::getVertices() const
    {
        return vertices;
    }

    const std::vector<Face>& HalfEdgeTable::getFaces() const
    {
        return faces;
    }

    HalfEdgeTable & operator += (HalfEdgeTable & first, const HalfEdgeTable & second)
    {
        first.points.insert(first.points.end(), second.points.begin(), second.points.end());

        const int firstHalfEdgesCount = first.halfEdges.size();
        const int firstVerticesCount = first.vertices.size();
        const int firstFacesCount = first.faces.size();

        first.vertices.insert(first.vertices.end(), second.vertices.begin(), second.vertices.end());
        for (int i = firstVerticesCount; i < first.vertices.size(); i++)
        {
            first.vertices[i].heh.index += firstHalfEdgesCount;
        }

        first.halfEdges.insert(first.halfEdges.end(), second.halfEdges.begin(), second.halfEdges.end());
        for (int i = firstHalfEdgesCount; i < first.halfEdges.size(); i++)
        {
            first.halfEdges[i].dst.index += firstVerticesCount;
            first.halfEdges[i].fh.index += firstFacesCount;
            first.halfEdges[i].next.index += firstHalfEdgesCount;
            first.halfEdges[i].prev.index += firstHalfEdgesCount;
            first.halfEdges[i].twin.index += firstHalfEdgesCount;
        }

        first.faces.insert(first.faces.end(), second.faces.begin(), second.faces.end());
        for (int i = firstFacesCount; i < first.faces.size(); i++)
        {
            first.faces[i].heh.index += firstHalfEdgesCount;
        }

        for (const auto& iter : second.hehsStartEnd)
        {
            auto& hehStartEnd = iter.first;
            first.hehsStartEnd.emplace(std::make_pair(hehStartEnd.first + firstVerticesCount,
                                                      hehStartEnd.second + firstVerticesCount),
                                       HalfEdgeHandle{ iter.second.index + firstHalfEdgesCount });
        }

        return first;
    }

    HalfEdgeTable operator+(HalfEdgeTable first, const HalfEdgeTable & second)
    {
        first += second;
        return first;
    }
}
