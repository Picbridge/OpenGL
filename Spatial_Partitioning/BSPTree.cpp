/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: <BSPTree.cpp>
Purpose: <All works related to BSP tree>
Language: <c++,visual studio2017>
//Platform: <visual studio2017 , win10>
Project: <Spatial_Partitioning>
Author: <jihoon.bae>
Creation date: <28/03/2019>
End Header --------------------------------------------------------*/
#include "BSPTree.h"
#include "GFX.h"

BSPTree::BSPTree() : mRoot(0)
{
}
BSPTree::~BSPTree()
{
    DestroyNode(mRoot);
}
void BSPTree::ConstructTree(std::vector<MeshRenderer*>& meshRenderers)
{
    mObjects = &meshRenderers;

    for (unsigned i = 0; i < meshRenderers.size(); ++i)
    {
        std::vector<Face*> face = meshRenderers[i]->GetMesh()->GetFaces();

        meshRenderers[i]->transform.Update();
        Matrix4 worldTransform = meshRenderers[i]->transform.GetWorld();
        worldTransform.Transpose();

        for (unsigned j = 0; j < face.size(); ++j)
        {
            RawFace* t = new RawFace();

            HalfEdge* he = (face[j])->mHEdge;
            HalfEdge* end = he;
            unsigned index = 0;

            do
            {
                t->mVertices[index++] = TransformPoint(worldTransform, he->mVertex->mPos);
                he = he->mNext;
            } while (he != end);

            mTriangleSoup.push_back(t);
        }
        std::cout << face.size() << " Added to the BSPTree pool " << std::endl;
        //meshRenderers[i]->GetMesh()->ClearContainers();
    }        
  
    std::cout << mTriangleSoup.size() << " Size of BSPTree pool " << std::endl;

    //std::cout << "Constructing BSP Tree ";
    BuildTopDownBSPTree(&mRoot, mTriangleSoup, 0);
    //std::cout << "Done!\n";
}

void BSPTree::BuildTopDownBSPTree(BSPNode** node, std::vector<RawFace*>& triangleSoup, int height)
{
    if (triangleSoup.empty())
        return;

    *node = new BSPNode();

    //std::cout << height << " " << std::endl;
    if (triangleSoup.size() <= 700)
    {
        (*node)->triangleSoup = triangleSoup;
        return;
    }

    
    Vector4 splitPlane = GetSplitPlane(triangleSoup);

    std::vector<RawFace*> frontList, backList;


    for (unsigned i = 0; i < triangleSoup.size(); ++i)
    {
        RawFace* t = triangleSoup[i], *frontPart, *backPart;

        switch (TestPolygonToPlane(t, splitPlane))
        {
        case R_COPLANAR:

            (*node)->triangleSoup.push_back(t);
            break;

        case R_INFRONT:
            frontList.push_back(t);
            break;

        case R_BEHIND:
            backList.push_back(t);
            break;

        case R_STRADDLING:
            // Split polygon to plane and send a part to each side of the plane
            SplitPolygon(t, splitPlane, &frontPart, &backPart);
            frontList.push_back(frontPart);
            backList.push_back(backPart);
            break;
        }
    }

    // Recursively build child subtrees
    BuildTopDownBSPTree(&(*node)->mLeft, frontList, height + 1);
    BuildTopDownBSPTree(&(*node)->mRight, backList, height + 1);
}

void BSPTree::DrawTree()
{
    DrawNode(mRoot, 0);
}

void BSPTree::DrawNode(BSPNode* node, unsigned height)
{
    if (node)
    {
        DrawNode(node->mLeft, height + 1);
        DrawNode(node->mRight, height + 1);

        DrawPolygon(node->triangleSoup, GetColor(height) + Vector4(0, 0, 0, .4f));
    }
}

void BSPTree::DestroyNode(BSPNode* node)
{
    if (node)
    {
        DestroyNode(mRoot->mLeft);
        DestroyNode(mRoot->mRight);
        delete node;
    }
}

void BSPTree::DrawPolygon(std::vector<RawFace*>& triangleSoup, Vector4& color)
{
    for (unsigned i = 0; i < triangleSoup.size(); ++i)
    {
        Vector4 plane = GetPlaneFromTriangle(triangleSoup[i]);
        Vector3 n;
        n.x = plane.x;
        n.y = plane.y;
        n.z = plane.z;

        Vector3 camPos = GFX::getInstance()->GetCamera()->GetPosition();

        if ((camPos - triangleSoup[i]->mVertices[0]).Dot(n) >= 0)
        {
            Vector3* pts = triangleSoup[i]->mVertices;

            glColor4f(color.x, color.y, color.z, color.w);

            glBegin(GL_LINES);

            glVertex3f(pts[0].x, pts[0].y, pts[0].z);
            glVertex3f(pts[1].x, pts[1].y, pts[1].z);
            glVertex3f(pts[1].x, pts[1].y, pts[1].z);
            glVertex3f(pts[2].x, pts[2].y, pts[2].z);
            glVertex3f(pts[0].x, pts[0].y, pts[0].z);
            glVertex3f(pts[2].x, pts[2].y, pts[2].z);

            glEnd();
        }
    }
}

Vector4 BSPTree::GetSplitPlane(std::vector<RawFace*>& triangleSoup)
{
    int plane = rand() % triangleSoup.size();
    return GetPlaneFromTriangle(triangleSoup[plane]);
}

PolygonResult BSPTree::TestPolygonToPlane(RawFace* t, Vector4& plane)
{
    int numInFront = 0, numBehind = 0;

    for (int i = 0; i < 3; ++i)
    {
        switch (TestPointToPlane(t->mVertices[i], plane))
        {
        case P_INFRONT:
            ++numInFront;
            break;

        case P_BEHIND:
            ++numBehind;
            break;
        }
    }

    if (numBehind != 0 && numInFront != 0)
        return R_STRADDLING;

    if (numInFront != 0)
        return R_INFRONT;

    if (numBehind != 0)
        return R_BEHIND;


    return R_COPLANAR;
}

PointResult BSPTree::TestPointToPlane(Vector3& pt, Vector4& plane)
{
    float dist = plane.Dot(ToVector4(pt, 1));

    if (dist > FLT_EPSILON)
        return P_INFRONT;
    if (dist < -FLT_EPSILON)
        return P_BEHIND;

    return P_ON;
}

Vector4 BSPTree::GetPlaneFromTriangle(RawFace* t)
{
    Vector3 v1 = t->mVertices[1] - t->mVertices[0];
    Vector3 v2 = t->mVertices[2] - t->mVertices[0];

    Vector3 n = v1.Cross(v2);
    n.Normalize();

    float d = n.Dot(t->mVertices[0]);

    Vector4 plane = ToVector4(n, -d);

    return plane;
}

void BSPTree::SplitPolygon(RawFace* t, Vector4& plane, RawFace** front, RawFace** back)
{
    int numFront = 0, numBack = 0;
    Vector3 frontVerts[6], backVerts[6];


    int numVerts = 3;
    Vector3 a = t->mVertices[numVerts - 1];
    int aSide = TestPointToPlane(a, plane);

    for (int i = 0; i < numVerts; ++i)
    {
        Vector3 b = t->mVertices[i];
        int bSide = TestPointToPlane(b, plane);

        if (bSide == P_INFRONT)
        {
            if (aSide == P_BEHIND)
            {
                Vector3 intersect = IntersectEdgeAgainstPlane(b, a, plane);
                frontVerts[numFront++] = backVerts[numBack++] = intersect;
            }

            frontVerts[numFront++] = b;
        }

        else if (bSide == P_BEHIND)
        {
            if (aSide == P_INFRONT)
            {

                Vector3 intersect = IntersectEdgeAgainstPlane(a, b, plane);
                frontVerts[numFront++] = backVerts[numBack++] = intersect;
            }
            else if (aSide == P_ON)
            {

                backVerts[numBack++] = a;
            }
            backVerts[numBack++] = b;
        }

        else
        {

            frontVerts[numFront++] = b;
            if (aSide == P_BEHIND)
                backVerts[numBack++] = b;
        }

        a = b;
        aSide = bSide;
    }

    
    *front = new RawFace();
    *back = new RawFace();

    for (int i = 0; i < numVerts; ++i)
    {
        (*front)->mVertices[i] = frontVerts[i];
        (*back)->mVertices[i] = backVerts[i];
    }
}

Math::Vector3 BSPTree::IntersectEdgeAgainstPlane(Vector3& p1, Vector3& p2, Vector4& plane)
{
    Vector3 v;
    v.x = plane.x;
    v.y = plane.y;
    v.z = plane.z;

    float denominator = v.Dot(p1 - p2);

    float u;

    if (denominator > -FLT_EPSILON && denominator < FLT_EPSILON)
        u = 0;
    else
        u = plane.Dot(ToVector4(p1, 1)) / denominator;

    v = p1 + (p2 - p1) * u;

    return v;
}
