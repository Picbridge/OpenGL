/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: <Mesh.h>
Purpose: <Load objects and set basic position,scale info>
Language: <c++,visual studio2017>
//Platform: <visual studio2017 , win10>
Project: <jihoon.bae_CS300_3>
Author: <jihoon.bae>
Creation date: <28/09/2018>
End Header --------------------------------------------------------*/

#pragma once

#include "Main/Support.h"
#include "Vertex.h"
#include "BoundingVolumes.h"
#include <list>

#define PRIMITIVEINDEX 0xFFFF
typedef std::unordered_map<std::string, HalfEdge*>::iterator HEdgeIter;

enum MAPTYPE
{
    NA,
    SPHERICAL,
    PLANAR,

    NUMOFMAPS
};

class Mesh
{
public:

    Mesh();
    ~Mesh();
    void LoadFromObj(char* fileName, MAPTYPE map = NA);
    void InitializeBuffers();
    void ClearMesh();
    void ClearContainers();
    void DrawMesh();
    void DrawBoundary();

    Vector3 GetMax() const { return mMax; }
    Vector3 GetMin() const { return mMin; }
    BVInfo* GetBVInfo() { return mBVInfo; }
    std::vector<Vertex>& GetVertices() { return mVertices; }
    std::vector<Face*>& GetFaces() { return mFaces; }

private:

    std::string mName;
    unsigned mVertexCount;
    unsigned mIndexCount;
    unsigned mFaceCount;

    std::vector<Vertex> mVertices;
    std::vector<unsigned> mIndices;
    std::vector<Face*> mFaces;
    std::unordered_map<std::string, HalfEdge*> mHEdgeMap;

    std::vector<Vector3> mBoundaryLines;
    std::priority_queue<Face*, std::vector<Face*>, Face::sortFaces> mValenceList;

    // Use to remove duplicates
    std::list<Vector3> mVertNormals;
    std::list<Vector3> mVertTangents;
    std::list<Vector3> mVertBinormals;

    unsigned mVertexArrayId;
    unsigned mVertexBufferId;
    unsigned mIndexBufferId;

    Vector3 mMax;
    Vector3 mMin;
    BVInfo* mBVInfo;

    // Private Member functions
    void OneRingTraversal(Vertex& v, void (Mesh::*fp)(HalfEdge* hedge));

    void CalculateNormals();
    void AddNormalsToList(HalfEdge* hedge);
    void CalculateTangentBinormal(Vertex& v0, Vertex& v1, Vertex& v2,
        Vector3& tangent, Vector3& binormal);

    void SortValenceList();
    Face* FindLeastNeighbors(Face* f, bool level = true);
    HalfEdge* GetEdgeFromLastTwoIndex(Face* f);
    void TriangulateIndices();
    void AddIndex(HalfEdge* hedge, Vector3& color);

    void FindBoundaryEdges();

    // Computing Bounding Volumes
    void StoreBVInfo();

    void AxisAllignedBoundingBox(AABB& box);
    void CentroidSphere(Sphere& s);

    void RitterSphere(Sphere& s);
    void GrowSphere(Sphere& s, Vector3& pt);

    void LarssonSphere(Sphere& s);
    void ExtremePointsAlongDirection(Vector3 dir, int& min, int& max);

    void EigenSphere(Sphere& s);
    void CovarianceMatrix(Matrix3& m);
    void JacobiRotation(Matrix3& a, Matrix3& v);
    void CalculateRotation(Matrix3& a, int p, int q, float &c, float &s);

    void EigenEllipsoid(Ellipsoid& s);
    void GrowEllipsoid(Ellipsoid& e);
    float EllipsoidEquation(Ellipsoid &e, Vector3& p);
    void OrientedBoundingBox(OBB& b);
    void ReOrientate();
};