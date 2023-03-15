/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: <BSPTree.h>
Purpose: <All works related to BSP tree>
Language: <c++,visual studio2017>
//Platform: <visual studio2017 , win10>
Project: <jihoon.bae_CS350_3>
Author: <jihoon.bae>
Creation date: <28/03/2019>
End Header --------------------------------------------------------*/
#pragma once

#include "MeshRenderer.h"

enum PolygonResult
{
    R_COPLANAR,
    R_INFRONT,
    R_BEHIND,
    R_STRADDLING
};

enum PointResult
{
    P_INFRONT,
    P_BEHIND,
    P_ON
};

struct BSPNode
{
    BSPNode() : mLeft(0), mRight(0) {}

    std::vector<RawFace*> triangleSoup;
    BSPNode* mLeft;
    BSPNode* mRight;
};

class BSPTree
{
public:

    BSPTree();
    ~BSPTree();
    void ConstructTree(std::vector<MeshRenderer*>& meshRenderers);
    void DrawTree();
    void BuildTopDownBSPTree(BSPNode** node, std::vector<RawFace*>& triangleSoup, int height);
    void DrawNode(BSPNode* node, unsigned height);
    void DestroyNode(BSPNode* node);
    void DrawPolygon(std::vector<RawFace*>& p, Vector4& color);
    Vector4 GetPlaneFromTriangle(RawFace* t);
    Vector4 GetSplitPlane(std::vector<RawFace*>& triangleSoup);
    PolygonResult TestPolygonToPlane(RawFace* t, Vector4& plane);
    PointResult TestPointToPlane(Vector3& pt, Vector4& plane);
    void SplitPolygon(RawFace* t, Vector4& plane, RawFace** front, RawFace** back);
    Vector3 IntersectEdgeAgainstPlane(Vector3& a, Vector3& b, Vector4& plane);

private:
    BSPNode* mRoot;
    std::vector<RawFace*> mTriangleSoup;
    std::vector<MeshRenderer*>* mObjects;
};