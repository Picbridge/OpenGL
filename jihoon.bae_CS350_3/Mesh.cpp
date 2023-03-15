/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: <Mesh.cpp>
Purpose: <Load objects and set basic position,scale info>
Language: <c++,visual studio2017>
//Platform: <visual studio2017 , win10>
Project: <jihoon.bae_CS300_3>
Author: <jihoon.bae>
Creation date: <28/09/2018>
End Header --------------------------------------------------------*/

#include "Mesh.h"
#include <fstream>
#include <string>
#include "BVH.h"

Mesh::Mesh()
{
    mVertexCount = 0;
    mIndexCount = 0;
    mFaceCount = 0;
    mMax = Vector3(FLT_MIN, FLT_MIN, FLT_MIN);
    mMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
    mBVInfo = new BVInfo();
}

Mesh::~Mesh()
{
    delete mBVInfo;
    ClearMesh();
}

void Mesh::LoadFromObj(char* fileName, MAPTYPE map)
{
    int count = 0;
    mName = fileName;
    std::cout << "Loading " << fileName << " mesh ";
    std::string file = "assets/";
    file.append(fileName);
    file.append(".obj");

    std::ifstream objFile(file);

    if (!objFile.is_open())
        printf("Error opening file\n");

    else
    {
        std::string tag;

        while (!objFile.eof())
        {
            objFile >> tag;

            if (tag == "v")
            {
                float x, y, z;

                objFile >> x;
                objFile >> y;
                objFile >> z;

                Vertex vertex(Vector3(x, y, z));

                switch (map)
                {
                case SPHERICAL:
                {
                    Vector3 pos = vertex.mPos;
                    float length = pos.Length();

                    vertex.mUVCoords.x = ArcTan2(pos.y, pos.x) / (2 * Math::cPi);
                    vertex.mUVCoords.y = ArcCos(pos.z / length) / Math::cPi;
                }
                    break;

                case PLANAR:
                {
                    vertex.mUVCoords.x = vertex.mPos.x * .5f + .5f;
                    vertex.mUVCoords.y = vertex.mPos.y * .5f + .5f;
                }
                    break;
                }

                mVertices.push_back(vertex);

                float index = mVertices.size() - 1.f;

                mMax = Math::Max(mMax, vertex.mPos);
                mMin = Math::Min(mMin, vertex.mPos);
            }

            else if (tag == "f")
            {
                unsigned x, y, z;

                objFile >> x;
                objFile >> y;
                objFile >> z;

                --x;
                --y;
                --z;

                // Create new face
                Face* newFace = new Face();

                // Calculate face normal
                Vertex v0 = mVertices[x],
                    v1 = mVertices[y],
                    v2 = mVertices[z];

                Vector3 a = v1.mPos - v0.mPos,
                    b = v2.mPos - v0.mPos,
                    normal = a.Cross(b);

                if (normal != Vector3(0, 0, 0))
                    normal.Normalize();

                newFace->mFaceNormal = normal;
                mFaces.push_back(newFace);

                // Create Half Edges
                HalfEdge* firstEdge = NULL;
                for (unsigned i = 1; i <= 3; ++i)
                {
                    unsigned to, from;
                    if (i == 1)
                    {
                        from = x;
                        to = y;
                    }
                    else if (i == 2)
                    {
                        from = y;
                        to = z;
                    }
                    else
                    {
                        from = z;
                        to = x;
                    }

                    std::string key = CreateKeyString(from, to),
                        twin_key = CreateKeyString(to, from);

                    HEdgeIter iter = mHEdgeMap.find(key);
                    HalfEdge* edge;

                    // Half Edge doesn't exist
                    if (iter == mHEdgeMap.end())
                    {
                        edge = new HalfEdge();
                        edge->mVertex = &mVertices[to];
                        edge->mFace = newFace;

                        HalfEdge* twin = new HalfEdge();
                        twin->mVertex = &mVertices[from];

                        edge->mTwin = twin;
                        twin->mTwin = edge;

                        // Add new half edges
                        mHEdgeMap.insert(std::pair<std::string, HalfEdge*>(key, edge));
                        mHEdgeMap.insert(std::pair<std::string, HalfEdge*>(twin_key, twin));
                    }

                    // Half Edge already exists
                    else
                    {
                        edge = iter->second;

                        // Update edge
                        if (!edge->mFace)
                            edge->mFace = newFace;
                    }

                    // Update Face
                    if (!newFace->mHEdge)
                        newFace->mHEdge = edge;

                    //Update Vertex
                    if (!mVertices[from].mHEdge)
                    {
                        mVertices[from].mHEdge = edge;
                        ++count;
                    }

                    if (i == 1)
                        firstEdge = edge;

                    if (i == 2)
                    {
                        firstEdge->mNext = edge;
                    }
                    else if (i == 3)
                    {
                        firstEdge->mNext->mNext = edge;
                        edge->mNext = firstEdge;
                    }
                }

                CalculateTangentBinormal(v0, v1, v2, newFace->mTangent, newFace->mBinormal);
            }

            else if (tag == "Vertices:")
            {
                objFile >> mVertexCount;
                mVertices.reserve(mVertexCount);
            }

            else if (tag == "Faces:")
            {
                objFile >> mFaceCount;
            }
        }
    }

    objFile.close();

    ReOrientate();

    // Do normal, tangent and binormal calculations
    CalculateNormals();

    // For triangle strips
    SortValenceList();
    TriangulateIndices();

    // To draw boundaries of open meshes
    FindBoundaryEdges();

    StoreBVInfo();

    std::cout << "Finshed loading "<< fileName << " \n";
}

void Mesh::ClearMesh()
{
    mVertexCount = mIndexCount = mFaceCount = 0;

    // Disable vertex array attributes.
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(5);

    // Release the vertex buffer.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &mVertexBufferId);

    // Release the index buffer.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &mIndexBufferId);

    // Release the vertex array object.
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &mVertexArrayId);
}

void Mesh::InitializeBuffers()
{
    // Create and bind vao
    glGenVertexArrays(1, &mVertexArrayId);
    glBindVertexArray(mVertexArrayId);

    // Generate the vertex buffer
    glGenBuffers(1, &mVertexBufferId);
    // Bind and load vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
    glBufferData(GL_ARRAY_BUFFER, mVertexCount * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW);

    // Enable vertex array attributes
    glEnableVertexAttribArray(0);  // Vertex position.
    glEnableVertexAttribArray(1);  // Vertex color.
    glEnableVertexAttribArray(2);  // Normal.
    glEnableVertexAttribArray(3);  // Tangent
    glEnableVertexAttribArray(4);  // Binormal
    glEnableVertexAttribArray(5);  // UV Coords

    // location and format of the POSITION of the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), 0);

    // location and format of the COLOR of the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (const GLvoid*)(3 * sizeof(float)));

    // location and format of the NORMAL of the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
    glVertexAttribPointer(2, 3, GL_FLOAT, true, sizeof(Vertex), (const GLvoid*)(6 * sizeof(float)));

    // location and format of the TANGENT of the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
    glVertexAttribPointer(3, 3, GL_FLOAT, true, sizeof(Vertex), (const GLvoid*)(9 * sizeof(float)));

    // location and format of the BINORMAL of the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
    glVertexAttribPointer(4, 3, GL_FLOAT, true, sizeof(Vertex), (const GLvoid*)(12 * sizeof(float)));

    // location and format of the UVCOORDS of the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
    glVertexAttribPointer(5, 2, GL_FLOAT, true, sizeof(Vertex), (const GLvoid*)(15 * sizeof(float)));

    // Generate the index buffer
    glGenBuffers(1, &mIndexBufferId);
    // Bind the index buffer and load the index data into it
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndexCount* sizeof(unsigned), &mIndices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Mesh::DrawMesh()
{
    // Bind the vertex array object
    glBindVertexArray(mVertexArrayId);

    glDrawElements(GL_TRIANGLE_STRIP, mIndexCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

void Mesh::CalculateNormals()
{
    for (unsigned i = 0; i < mVertices.size(); ++i)
    {
        OneRingTraversal(mVertices[i], &Mesh::AddNormalsToList);

        std::list<Vector3>::iterator iter;

        // Remove duplicate normals
        mVertNormals.unique();
        if (!mVertNormals.empty())
        {
            for (iter = mVertNormals.begin(); iter != mVertNormals.end(); ++iter)
                mVertices[i].mNormal += *iter;

            mVertices[i].mNormal.Normalize();
        }

        //// Remove duplicate tangents
        //mVertTangents.unique();
        //if(!mVertTangents.empty())
        //{
        //  for(iter = mVertTangents.begin(); iter != mVertTangents.end(); ++iter)
        //    mVertices[i].mTangent += *iter;
        //
        //  // Prevent division by zero
        //  if(mVertices[i].mTangent != Vector3(0,0,0))
        //    mVertices[i].mTangent.Normalize();
        //}
        //
        //// Remove duplicate binormals
        //mVertBinormals.unique();
        //if(!mVertBinormals.empty())
        //{
        //  for(iter = mVertBinormals.begin(); iter != mVertBinormals.end(); ++iter)
        //    mVertices[i].mBinormal += *iter;
        //
        //  // Prevent division by zero
        //  if(mVertices[i].mBinormal != Vector3(0,0,0))
        //    mVertices[i].mBinormal.Normalize();
        //}

        mVertNormals.clear();
        mVertTangents.clear();
        mVertBinormals.clear();
    }
}

void Mesh::AddNormalsToList(HalfEdge* hedge)
{
    // Face edge is pointing at
    Face* f = hedge->mFace;

    if (f)
    {
        mVertNormals.push_back(f->mFaceNormal);
        //mVertTangents.push_back(f->mTangent);
        //mVertBinormals.push_back(f->mBinormal);
    }
}

void Mesh::CalculateTangentBinormal(Vertex& v0, Vertex& v1, Vertex& v2, Vector3& tangent, Vector3& binormal)
{
    Vector3 vec0, vec1;
    Vector2 uv0, uv1;
    float div, denominator;

    // Calculate the two vectors for this face.
    vec0 = v1.mPos - v0.mPos;
    vec1 = v2.mPos - v0.mPos;

    // Calculate the u and v texture space vectors.
    uv0 = v1.mUVCoords - v0.mUVCoords;
    uv1 = v2.mUVCoords - v0.mUVCoords;

    // Calculate the denominator of the tangent/binormal equation.
    denominator = uv0.x * uv1.y - uv1.x * uv0.y;
    if (!Math::IsZero(denominator))
        div = 1.0f / (denominator);
    else
        return;

    // Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
    tangent = (uv1.y * vec0 - uv0.y * vec1) * div;
    binormal = (uv0.x * vec1 - uv1.x * vec0) * div;

    // Normalize the tangent and then store it
    tangent.Normalize();

    // Normalize the binormal and then store it
    binormal.Normalize();
}

void Mesh::OneRingTraversal(Vertex& v, void (Mesh::*fp)(HalfEdge* hedge))
{
    if (!v.mHEdge)
        return;

    HalfEdge* edge = v.mHEdge;
    HalfEdge* end = edge;
    bool traverseOtherDir = false;

    // Traverse CCW
    do
    {
        // Must use this pointer to know which object
        (this->*fp)(edge);

        // Traverse CCW
        if (edge->mNext)
            edge = edge->Prev()->mTwin;

        // Hole in the mesh
        else
        {
            traverseOtherDir = true;
            break;
        }
    } while (edge != end);

    // Traverse CW
    if (traverseOtherDir)
    {
        do
        {
            // Must use this pointer to know which object
            (this->*fp)(edge);

            // Traverse CW
            edge = edge->mTwin->mNext;
        } while (edge); // Breaks when it reaches the hole
    }
}

void Mesh::FindBoundaryEdges()
{
    HEdgeIter iter;
    for (iter = mHEdgeMap.begin(); iter != mHEdgeMap.end(); ++iter)
    {
        HalfEdge* edge = iter->second;

        if (!edge->mFace)
        {
            // Add first point of edge
            mBoundaryLines.push_back(edge->mTwin->mVertex->mPos);

            // Add second point of edge
            mBoundaryLines.push_back(edge->mVertex->mPos);
        }
    }
}

void Mesh::DrawBoundary()
{
    if (mBoundaryLines.empty())
        return;

    glColor4f(1.f, 0.0f, 0.0f, 1.f);

    glBegin(GL_LINES);

    for (unsigned i = 0; i < mBoundaryLines.size(); ++i)
    {
        Vector3 p1 = mBoundaryLines[i];
        Vector3 p2 = mBoundaryLines[++i];

        glVertex3f(p1.x, p1.y, p1.z);
        glVertex3f(p2.x, p2.y, p2.z);
    }

    glEnd();
}

void Mesh::SortValenceList()
{
    for (unsigned i = 0; i < mFaces.size(); ++i)
    {
        HalfEdge* hedge = mFaces[i]->mHEdge;
        HalfEdge* end = hedge;

        do
        {
            if (hedge->mTwin->mFace)
                mFaces[i]->mNeighbors.push_back(hedge->mTwin);
            hedge = hedge->mNext;
        } while (hedge != end);

        mValenceList.push(mFaces[i]);
    }
}

void Mesh::TriangulateIndices()
{
    while (mValenceList.size())
    {
        bool newStrip = true;
        Vector3 color;
        color.x = RandomFloat(0, 1);
        color.y = RandomFloat(0, 1);
        color.z = RandomFloat(0, 1);

        // Start with triangle with least number of neighbors
        // Chosen arbitrarily if same number of neighbors
        Face* f = mValenceList.top();
        mValenceList.pop();

        // Skips if visited before - already belongs to a triangle strip
        if (f->mVisited)
            continue;

        // Find neighbor face with least neighbors and not yet visited
        Face* nextFace = FindLeastNeighbors(f);

        // No neighbors - just 1 triangle
        if (!nextFace)
        {
            if (mIndices.size())
                mIndices.push_back(PRIMITIVEINDEX);

            AddIndex(f->mHEdge, color);
            AddIndex(f->mHEdge->mNext, color);
            AddIndex(f->mHEdge->Prev(), color);
        }

        // Has neighbors
        else
        {
            HalfEdge* hedge = f->FindSharedHalfEdge(nextFace);
            HalfEdge* prevEdge = NULL;

            // Create new strip
            while (nextFace && nextFace->GetNumofFreeNeighbors())
            {
                // First triangle of new strip, have to add all 3 vertices
                if (newStrip)
                {
                    if (mIndices.size())
                        mIndices.push_back(PRIMITIVEINDEX);

                    AddIndex(hedge, color);
                    AddIndex(hedge->mNext, color);
                    AddIndex(hedge->Prev(), color);
                    AddIndex(hedge->mTwin, color);

                    newStrip = false;
                    f = nextFace;
                    prevEdge = GetEdgeFromLastTwoIndex(f);
                }

                else
                {
                    nextFace = FindLeastNeighbors(f);

                    if (nextFace)
                    {
                        hedge = f->FindSharedHalfEdge(nextFace);

                        // Order is correct
                        if (hedge == prevEdge)
                        {
                            AddIndex(hedge->mTwin, color);
                        }

                        // Order wrong - create degenerate triangle
                        else
                        {
                            unsigned index = mIndices[mIndices.size() - 1];
                            unsigned index2 = mIndices[mIndices.size() - 3];
                            mIndices.push_back(index);
                            mIndices.push_back(index2);
                            AddIndex(hedge->mTwin, color);
                        }

                        f = nextFace;
                        prevEdge = GetEdgeFromLastTwoIndex(nextFace);
                    }
                }
            }
        }
    }

    mIndexCount = mIndices.size();
}

void Mesh::AddIndex(HalfEdge* hedge, Vector3& color)
{
    Vertex* v = hedge->mNext->mVertex;
    v->mColor = color;
    unsigned index = v - &mVertices[0];
    mIndices.push_back(index);
    hedge->mFace->mVisited = true;
}

Face* Mesh::FindLeastNeighbors(Face* f, bool level)
{
    std::vector<Face*> neighbors;
    std::vector<HalfEdge*>::iterator iter;
    for (iter = f->mNeighbors.begin(); iter != f->mNeighbors.end(); ++iter)
    {
        if ((*iter)->mFace != f && !(*iter)->mFace->mVisited)
            neighbors.push_back((*iter)->mFace);
    }

    if (neighbors.size() == 1)
        return neighbors[0];

    else if (neighbors.size() == 2)
    {
        Face* nbr1 = neighbors[0];
        Face* nbr2 = neighbors[1];

        // Both free - have to determine which has fewer neighbors
        unsigned neighborCount1 = nbr1->GetNumofFreeNeighbors();
        unsigned neighborCount2 = nbr2->GetNumofFreeNeighbors();

        if (level && neighborCount2 == neighborCount1)
        {
            // 1 level recursion
            Face* f1 = FindLeastNeighbors(nbr1, false);
            Face* f2 = FindLeastNeighbors(nbr2, false);

            // nbr2 has less neighbors
            if (f1 && !f2)
                return nbr2;
            // nbr1 has less neighbors
            else if (!f1 && f2)
                return nbr1;
            // both have no neighbors
            else if (!f1 && !f2)
                return nbr1;

            if (f2->GetNumofFreeNeighbors() < f1->GetNumofFreeNeighbors())
                return nbr2;
            else
                return nbr1;
        }

        if (neighborCount2 < neighborCount1)
            return nbr2;
        else
            return nbr1;
    }

    else if (neighbors.size() == 3)
    {
        Face* nbr1 = neighbors[0];
        Face* nbr2 = neighbors[1];
        Face* nbr3 = neighbors[2];

        unsigned neighborCount1 = nbr1->GetNumofFreeNeighbors();
        unsigned neighborCount2 = nbr2->GetNumofFreeNeighbors();
        unsigned neighborCount3 = nbr3->GetNumofFreeNeighbors();

        if (neighborCount1 == neighborCount2 && neighborCount1 == neighborCount3)
            return nbr1;

        if (neighborCount1 < neighborCount2)
        {
            if (neighborCount1 < neighborCount3)
                return nbr1;
            else
                return nbr3;
        }

        else
        {
            if (neighborCount2 < neighborCount3)
                return nbr2;
            else
                return nbr3;
        }
    }

    return NULL;
}

HalfEdge* Mesh::GetEdgeFromLastTwoIndex(Face* f)
{
    unsigned index = mIndices.size();
    std::string key = CreateKeyString(mIndices[index - 2], mIndices[index - 1]);
    HalfEdge* hedge = mHEdgeMap[key];
    if (hedge->mFace == f)
        return hedge;
    else
        return hedge->mTwin;
}

void Mesh::StoreBVInfo()
{
    // AABB
    AxisAllignedBoundingBox(mBVInfo->mAABB);

    // Centroid
    CentroidSphere(mBVInfo->mCentroid);

    // Ritter's
    RitterSphere(mBVInfo->mRitters);

    // Larsson's
    LarssonSphere(mBVInfo->mLarssons);

    // PCA Sphere
    EigenSphere(mBVInfo->mPCASphere);

    // PCA Ellipsoid
    EigenEllipsoid(mBVInfo->mPCAEllipsoid);

    // OBB
    OrientedBoundingBox(mBVInfo->mOBB);
}

void Mesh::AxisAllignedBoundingBox(AABB& box)
{
    box.Centre = (mMax + mMin) * .5f;

    Vector3 v;
    v = Math::Max(Abs(mMax), Abs(mMin));
    float unitScale = Sqrt(3);

    box.HalfExtent = (v - box.Centre) * unitScale;
}

void Mesh::CentroidSphere(Sphere& s)
{
    for (unsigned i = 0; i < mVertices.size(); ++i)
    {
        // Find furthest point from centre
        float radius = mVertices[i].mPos.LengthSq();
        s.Radius = Math::Max(s.Radius, radius);
    }

    s.Centre = (mMax + mMin) * .5f;
    s.Radius = Math::Sqrt(s.Radius);
}

void Mesh::GrowSphere(Sphere& s, Vector3& pt)
{
    Vector3 v = pt - s.Centre;
    float length = v.LengthSq();

    if (length > s.Radius * s.Radius)
    {
        length = Sqrt(length);
        float newRadius = (s.Radius + length) * 0.5f;
        float k = (newRadius - s.Radius) / length;
        s.Radius = newRadius;
        s.Centre += v * k;
    }
}

void Mesh::RitterSphere(Sphere& s)
{
    s.Centre = (mMin + mMax) * .5f;
    s.Radius = 0;

    if (mName == "sphere")
        s.Radius = 1;

    for (unsigned i = 0; i < mVertices.size(); ++i)
        GrowSphere(s, mVertices[i].mPos);
}

void Mesh::LarssonSphere(Sphere& s)
{
    int max = 0,
        min = 0,
        maxProj = 0,
        minProj = 0;
    unsigned numNormals = sizeof(LarssonNormals) / sizeof(Vector3);

    for (unsigned i = 0; i < numNormals; ++i)
    {
        ExtremePointsAlongDirection(LarssonNormals[i], minProj, maxProj);

        if ((mVertices[maxProj].mPos - mVertices[minProj].mPos).LengthSq()
    >(mVertices[max].mPos - mVertices[min].mPos).LengthSq())
        {
            max = maxProj;
            min = minProj;
        }
    }

    s.Centre = (mVertices[max].mPos + mVertices[min].mPos) * .5f;
    s.Radius = (mVertices[max].mPos - mBVInfo->mLarssons.Centre).Length();

    for (unsigned i = 0; i < mVertices.size(); ++i)
        GrowSphere(s, mVertices[i].mPos);
}

void Mesh::EigenSphere(Sphere& s)
{
    Matrix3 covMatrix, jacobiRot;

    CovarianceMatrix(covMatrix);
    JacobiRotation(covMatrix, jacobiRot);

    Vector3 e;
    int maxc = 0;
    float maxe = Abs(covMatrix[0][0]);
    if (Abs(covMatrix[1][1]) > maxe)
    {
        maxc = 1;
        maxe = Abs(covMatrix[1][1]);
    }
    if (Abs(covMatrix[2][2]) > maxe)
    {
        maxc = 2;
        maxe = Abs(covMatrix[2][2]);
    }
    e.x = jacobiRot[0][maxc];
    e.y = jacobiRot[1][maxc];
    e.z = jacobiRot[2][maxc];

    int min, max;
    ExtremePointsAlongDirection(e, min, max);
    Vector3 minpt = mVertices[min].mPos;
    Vector3 maxpt = mVertices[max].mPos;

    s.Radius = (maxpt - minpt).Length() * 0.5f;
    s.Centre = (minpt + maxpt) * 0.5f;

    for (unsigned i = 0; i < mVertices.size(); ++i)
        GrowSphere(s, mVertices[i].mPos);
}

void Mesh::CovarianceMatrix(Matrix3& m)
{
    float average = 1.0f / mVertices.size();
    Vector3 centre;
    float e00, e11, e22, e01, e02, e12;

    // Compute the center of mass (centroid) of the points
    for (unsigned i = 0; i < mVertices.size(); ++i)
        centre += mVertices[i].mPos;
    centre *= average;

    // Compute covariance elements
    e00 = e11 = e22 = e01 = e02 = e12 = 0.0f;
    for (unsigned i = 0; i < mVertices.size(); ++i)
    {
        // Translate points so center of mass is at origin
        Vector3 p = mVertices[i].mPos - centre;
        // Compute covariance of translated points
        e00 += p.x * p.x;
        e11 += p.y * p.y;
        e22 += p.z * p.z;
        e01 += p.x * p.y;
        e02 += p.x * p.z;
        e12 += p.y * p.z;
    }
    // Fill in the covariance matrix elements
    m[0][0] = e00 * average;
    m[1][1] = e11 * average;
    m[2][2] = e22 * average;
    m[0][1] = m[1][0] = e01 * average;
    m[0][2] = m[2][0] = e02 * average;
    m[1][2] = m[2][1] = e12 * average;
}

void Mesh::JacobiRotation(Matrix3& cov, Matrix3& v)
{
    int i, j, n, p, q;
    float prevoff, c, s;
    Matrix3 J, b, t;

    // Initialize v to identify matrix
    v.SetIdentity();

    // Repeat for some maximum number of iterations
    const int iterations = 50;
    for (n = 0; n < iterations; ++n)
    {
        // Find largest off-diagonal absolute element a[p][q]
        p = 0; q = 1;
        for (i = 0; i < 3; ++i)
        {
            for (j = 0; j < 3; ++j)
            {
                if (i == j)
                    continue;

                if (Abs(cov[i][j]) > Abs(cov[p][q]))
                {
                    p = i;
                    q = j;
                }
            }
        }

        // Compute the Jacobi rotation matrix J(p, q, theta)
        CalculateRotation(cov, p, q, c, s);
        for (i = 0; i < 3; ++i)
        {
            J[i][0] = J[i][1] = J[i][2] = 0.0f;
            J[i][i] = 1.0f;
        }

        J[p][p] = c; J[p][q] = s;
        J[q][p] = -s; J[q][q] = c;

        // Cumulate rotations into what will contain the eigenvectors
        v = v * J;

        // Make ’cov?more diagonal, until just eigenvalues remain on diagonal
        cov = (J.Transpose() * cov) * J;

        // Compute "norm" of off-diagonal elements
        float off = 0.0f;
        for (i = 0; i < 3; ++i)
        {
            for (j = 0; j < 3; ++j)
            {
                if (i == j) continue;
                off += cov[i][j] * cov[i][j];
            }
        }

        /* off = sqrt(off); not needed for norm comparison */
        // Stop when norm no longer decreasing
        if (n > 2 && off >= prevoff)
            return;
        prevoff = off;
    }
}

void Mesh::CalculateRotation(Matrix3& a, int p, int q, float &c, float &s)
{
    if (Abs(a[p][q]) > 0.0001f)
    {
        float r = (a[q][q] - a[p][p]) / (2.0f * a[p][q]);
        float t;

        if (r >= 0.0f)
            t = 1.0f / (r + Sqrt(1.0f + r*r));
        else
            t = -1.0f / (-r + Sqrt(1.0f + r*r));

        c = 1.0f / Sqrt(1.0f + t*t);
        s = t * c;
    }

    else
    {
        c = 1.0f;
        s = 0.0f;
    }
}

void Mesh::ExtremePointsAlongDirection(Vector3 dir, int& min, int& max)
{
    float minProj = FLT_MAX, maxProj = FLT_MIN;
    for (unsigned i = 0; i < mVertices.size(); ++i)
    {
        float proj = Dot(mVertices[i].mPos, dir);

        if (proj < minProj)
        {
            minProj = proj;
            min = i;
        }

        if (proj > maxProj)
        {
            maxProj = proj;
            max = i;
        }
    }
}

void Mesh::EigenEllipsoid(Ellipsoid& e)
{
    Matrix3 covMatrix, jacobiRot;

    CovarianceMatrix(covMatrix);
    JacobiRotation(covMatrix, jacobiRot);
    e.JacobiRotation = jacobiRot;

    // Find the component with largest magnitude eigenvalue (largest spread)
    Vector3 eigen[3], maxpt, minpt;
    int min[3], max[3];
    Matrix3 m;
    Vector3 pt;

    eigen[0].x = jacobiRot[0][0];
    eigen[0].y = jacobiRot[1][0];
    eigen[0].z = jacobiRot[2][0];
    eigen[1].x = jacobiRot[0][1];
    eigen[1].y = jacobiRot[1][1];
    eigen[1].z = jacobiRot[2][1];
    eigen[2].x = jacobiRot[0][2];
    eigen[2].y = jacobiRot[1][2];
    eigen[2].z = jacobiRot[2][2];

    pt.x = jacobiRot[0][0];
    pt.y = jacobiRot[1][1];
    pt.z = jacobiRot[2][2];

    for (int i = 0; i < 3; ++i)
    {
        ExtremePointsAlongDirection(eigen[i], min[i], max[i]);
        minpt = mVertices[min[i]].mPos;
        maxpt = mVertices[max[i]].mPos;

        if (maxpt.LengthSq() > minpt.LengthSq())
            e.HalfExtent[i] = maxpt.Length();
        else
            e.HalfExtent[i] = minpt.Length();

        float dist1 = Dot(maxpt, eigen[i]);
        float dist2 = Dot(minpt, eigen[i]);
        e.Centre += (dist1 + dist2) * eigen[i] * .5f;
    }

    GrowEllipsoid(e);
}

void Mesh::OrientedBoundingBox(OBB& b)
{
    Matrix3 covMatrix, jacobiRot;

    CovarianceMatrix(covMatrix);
    JacobiRotation(covMatrix, jacobiRot);
    b.JacobiRotation = jacobiRot;

    Vector3 eigen[3], minpt, maxpt;
    int min[3], max[3];

    eigen[0].x = jacobiRot[0][0];
    eigen[0].y = jacobiRot[1][0];
    eigen[0].z = jacobiRot[2][0];
    eigen[1].x = jacobiRot[0][1];
    eigen[1].y = jacobiRot[1][1];
    eigen[1].z = jacobiRot[2][1];
    eigen[2].x = jacobiRot[0][2];
    eigen[2].y = jacobiRot[1][2];
    eigen[2].z = jacobiRot[2][2];

    float unitScale = Sqrt(3);
    for (int i = 0; i < 3; ++i)
    {
        ExtremePointsAlongDirection(eigen[i], min[i], max[i]);
        minpt = mVertices[min[i]].mPos;
        maxpt = mVertices[max[i]].mPos;

        float dist1, dist2;
        dist1 = Dot(maxpt, eigen[i]);
        dist2 = Dot(minpt, eigen[i]);

        b.HalfExtent[i] = (dist1 - dist2) * eigen[i].Length() * .5f * unitScale;
        b.Centre += (dist1 + dist2) * eigen[i] * .5f;
    }
}

void Mesh::ReOrientate()
{
    // Move centre to 0,0,0 and rescale
    Vector3 centre = (mMax + mMin) * .5f;

    float rescale = FLT_MIN;
    for (unsigned i = 0; i < mVertices.size(); ++i)
    {
        float radius = (mVertices[i].mPos - centre).LengthSq();
        rescale = Math::Max(rescale, radius);
    }
    rescale = 1.f / Sqrt(rescale);

    for (unsigned i = 0; i < mVertices.size(); ++i)
    {
        mVertices[i].mPos -= centre;
        mVertices[i].mPos *= rescale;
    }

    mMax -= centre;
    mMin -= centre;
    mMax *= rescale;
    mMin *= rescale;

    if (mName == "horse")
    {
        Matrix3 rot;
        EulerAngles rotAngles(0, 0, 0, EulerOrders::XYZs);
        rotAngles.Angles.x = Math::DegToRad(-90);
        rotAngles.Angles.y = Math::DegToRad(90);

        Math::ToMatrix3(rotAngles, &rot);

        for (unsigned i = 0; i < mVertices.size(); ++i)
            mVertices[i].mPos = Transform(rot, mVertices[i].mPos);

        for (unsigned i = 0; i < mFaces.size(); ++i)
        {
            mFaces[i]->mFaceNormal = Transform(rot, mFaces[i]->mFaceNormal);
            mFaces[i]->mTangent = Transform(rot, mFaces[i]->mTangent);
            mFaces[i]->mBinormal = Transform(rot, mFaces[i]->mBinormal);
        }

        mMax = Transform(rot, mMax);
        mMin = Transform(rot, mMin);
    }

    if (mName == "happy")
    {
        Matrix3 rot;
        EulerAngles rotAngles(0, 0, 0, EulerOrders::XYZs);
        rotAngles.Angles.x = Math::DegToRad(-90);

        Math::ToMatrix3(rotAngles, &rot);

        for (unsigned i = 0; i < mVertices.size(); ++i)
            mVertices[i].mPos = Transform(rot, mVertices[i].mPos);

        for (unsigned i = 0; i < mFaces.size(); ++i)
        {
            mFaces[i]->mFaceNormal = Transform(rot, mFaces[i]->mFaceNormal);
            mFaces[i]->mTangent = Transform(rot, mFaces[i]->mTangent);
            mFaces[i]->mBinormal = Transform(rot, mFaces[i]->mBinormal);
        }

        mMax = Transform(rot, mMax);
        mMin = Transform(rot, mMin);
    }
}

void Mesh::GrowEllipsoid(Ellipsoid& e)
{
    bool grow = false;
    float delta = .0001f;

    do
    {
        grow = false;
        Vector3 p;

        for (unsigned i = 0; i < mVertices.size(); ++i)
        {
            p = mVertices[i].mPos;
            float res = EllipsoidEquation(e, p);

            if (res > 1)
            {
                grow = true;
                break;
            }
        }

        if (grow)
        {
            e.HalfExtent.x *= 1 + delta;
            e.HalfExtent.y *= 1 + delta;
            e.HalfExtent.z *= 1 + delta;

            p.Normalize();
            e.Centre += p * delta;
        }
    } while (grow);
}

float Mesh::EllipsoidEquation(Ellipsoid &e, Vector3& p)
{
    p = Transform(e.JacobiRotation.Inverted(), p);
    p -= e.Centre;

    return (p.x * p.x) / (e.HalfExtent.x * e.HalfExtent.x) +
        (p.y * p.y) / (e.HalfExtent.y * e.HalfExtent.y) +
        (p.z * p.z) / (e.HalfExtent.z * e.HalfExtent.z);
}

void Mesh::ClearContainers()
{
    for (unsigned i = 0; i < mFaces.size(); ++i)
        delete mFaces[i];

    for (HEdgeIter iter = mHEdgeMap.begin(); iter != mHEdgeMap.end(); ++iter)
        delete iter->second;

    mVertices.clear();
    mIndices.clear();
    mFaces.clear();
    mHEdgeMap.clear();
}
