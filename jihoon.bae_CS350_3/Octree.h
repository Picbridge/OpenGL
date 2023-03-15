///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <Octree.h>
//Purpose: <All works related to Octree>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017 , win10>
//Project: <jjihoon.bae_CS300_3>
//Author: <jihoon.bae>
//Creation date: <26/03/2018>
//End Header --------------------------------------------------------*/
#pragma once

#include "MeshRenderer.h"
#include "MeshFactory.h"

#include <list>

struct OctreeNode {
    AABB mAABB;
    OctreeNode * mChilds[8];
    std::vector<RawFace*> mFaceList;
    int mLevel; 

    OctreeNode(void)
    {
        for (int i = 0; i < 8; ++i)
            mChilds[i] = nullptr;
    }
};

class Octree
{
public:
    Octree(void);
    ~Octree(void);

    bool PointInAABB(Vector3 pt, AABB aabb);
    bool TriangleInAABB(RawFace* triangle, AABB aabb);
    void BuildOctree(std::vector<MeshRenderer*>& meshRenderers);
    void BuildAdaptiveOctree(std::vector<RawFace*>& triangleSoup, Vector3 center, float halfwidth);
    void BuildTopDownAdaptiveOctree(OctreeNode**node, unsigned height, std::vector<RawFace*>& pFacelist, Vector3 center, float halfwidth);
    void DrawOctree(unsigned shader = 0);
    void DrawNode(OctreeNode* node, unsigned height, unsigned shader);
    void DrawOctreeNode(AABB& box, Vector4 color);
	void DeleteTree();
private:
    void DeleteNode(OctreeNode* node);
    OctreeNode* mRoot;

    std::vector<RawFace> mFacePool;
    std::vector<MeshRenderer*>* mObjects;
};
