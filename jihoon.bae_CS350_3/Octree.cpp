///* Start Header -------------------------------------------------------
//Copyright (C) 2018 DigiPen Institute of Technology.
//Reproduction or disclosure of this file or its contents without the prior written
//consent of DigiPen Institute of Technology is prohibited.
//File Name: <Octree.cpp>
//Purpose: <All works related to Octree>
//Language: <c++,visual studio2017>
//Platform: <visual studio2017 , win10>
//Project: <jjihoon.bae_CS300_3>
//Author: <jihoon.bae>
//Creation date: <26/03/2018>
//End Header --------------------------------------------------------*/
#include "Octree.h"
#include "GFX.h"
#include "Face.h"
Octree::Octree(void)
{
    mRoot = new OctreeNode();
}

Octree::~Octree(void)
{
    DeleteNode(mRoot);
}

bool Octree::PointInAABB(Vector3 pt, AABB aabb)
{
    Vector3 min = aabb.Centre - aabb.HalfExtent;
    Vector3 max = aabb.Centre + aabb.HalfExtent;

    //std::cout << min.x << " " << min.y << " " << min.z << " " << std::endl;
    //std::cout << max.x << " " << max.y << " " << max.z << " " << std::endl;
    //std::cout << pt.x << " " << pt.y << " " << pt.z << " " << std::endl;
    if (pt.x > min.x && pt.x < max.x &&
        pt.y > min.y && pt.y < max.y &&
        pt.z > min.z && pt.z < max.z)
        return true;

    //std::cout << pt.x << " " << pt.y << " " << pt.z << " " << std::endl;
    //std::cout << "OUTSIDE" << std::endl;
    return false;
}
bool Octree::TriangleInAABB(RawFace* triangle, AABB aabb)
{
    return PointInAABB(triangle->mVertices[0], aabb) ||
           PointInAABB(triangle->mVertices[1], aabb) ||
           PointInAABB(triangle->mVertices[2], aabb);
}

void Octree::BuildOctree(std::vector<MeshRenderer*>& meshRenderers)
{
    mObjects = &meshRenderers;
    std::vector<RawFace*> triangleSoup;
    Vector3 max(FLT_MIN), min(FLT_MAX);


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
                t->mVertices[index] = TransformPoint(worldTransform, he->mVertex->mPos);
                max = Max(max, t->mVertices[index]);
                min = Min(min, t->mVertices[index]);

                ++index;
                he = he->mNext;
            } while (he != end);

            triangleSoup.push_back(t);
        }

        std::cout << face.size() << " Added to the Octree pool " << std::endl;
        //face.clear();
    }
    std::cout << triangleSoup.size() << " Size of Octree pool " << std::endl;

    Vector3 center = (max + min) * .5f;
    float halfwidth = max.x - center.x;
	float halfheight = max.y - center.y;

    BuildAdaptiveOctree(triangleSoup, center, halfheight<halfwidth? halfwidth: halfheight);

    for (unsigned i = 0; i < triangleSoup.size(); ++i)
        delete triangleSoup[i];
    triangleSoup.clear();
    
    
}
void Octree::DeleteTree()
{
	DeleteNode(mRoot);
}
void Octree::BuildAdaptiveOctree(std::vector<RawFace*>& triangleSoup, Vector3 center, float halfwidth)
{
    int level = 1;

    //Base case
    if (triangleSoup.size() == 0)
        return;
    else
    {
        
        //mRoot->mFaceList.reserve(mFacePool.size());
        // Init the root node
        for (unsigned i = 0; i < triangleSoup.size(); ++i)
        {
            //std::cout << i << " Curr " << std::endl;
            RawFace * ptr = triangleSoup[i];
            mRoot->mFaceList.push_back(ptr);
        }
        std::cout << mRoot->mFaceList.size() << " Size of Octree pool CHECK" << std::endl;
        //mRoot->mAABB.Centre = sceneBounds.Centre;
        //float halfwidth = Max(Max(sceneBounds.HalfExtent.x, sceneBounds.HalfExtent.y), sceneBounds.HalfExtent.z);

        mRoot->mAABB.Centre = center;
        //float halfwidth = Max(Max(sceneBounds.HalfExtent.x, sceneBounds.HalfExtent.y), sceneBounds.HalfExtent.z);

        mRoot->mAABB.HalfExtent = Vector3(halfwidth);
        //mRoot->mCenter = sceneBounds.Centre;
        Vector3 offset;
        float step = halfwidth * 0.5f;


        for (int i = 0; i < 8; ++i)
        {
            //AABB newBounds;
            offset.x = ((i & 1) ? step : -step);
            offset.y = ((i & 2) ? step : -step);
            offset.z = ((i & 4) ? step : -step);
            //newBounds.Centre = mRoot->mAABB.Centre + offset;
            //newBounds.HalfExtent.x = newBounds.HalfExtent.y = newBounds.HalfExtent.z = step;
            //mRoot->mChilds[i] = 
            BuildTopDownAdaptiveOctree(&mRoot->mChilds[i], level + 1, mRoot->mFaceList, center + offset, step);
        }
    }

}
void Octree::BuildTopDownAdaptiveOctree(OctreeNode**pnode, unsigned height, std::vector<RawFace*>& pFacelist, Vector3 center, float halfwidth)
{
    // Child factory
    *pnode = new OctreeNode();
    OctreeNode* node = *pnode;

    node->mAABB.Centre = center;
    node->mAABB.HalfExtent = Vector3(halfwidth);
    node->mLevel = height;
    
    for (unsigned i = 0; i < pFacelist.size(); ++i)
    {
        //if (height == 1)
        //{
        if (TriangleInAABB(pFacelist[i], node->mAABB))
        {
            RawFace * ptr = pFacelist[i];
            node->mFaceList.push_back(ptr);
        }
            
    }
    //std::cout << height << " Height CHECK " << node->mFaceList.size() << std::endl;
    if (node->mFaceList.size() <= 300)
        return;
    else
    {
        //mRoot->mCenter = sceneBounds.Centre;

        Vector3 offset;
        float step = halfwidth * 0.5f;

        for (int i = 0; i < 8; ++i)
        {
            //AABB newBounds;
            offset.x = ((i & 1) ? step : -step);
            offset.y = ((i & 2) ? step : -step);
            offset.z = ((i & 4) ? step : -step);
            //newBounds.Centre = node->mAABB.Centre + offset;
            //newBounds.HalfExtent.x = newBounds.HalfExtent.y = newBounds.HalfExtent.z = step;
            //mRoot->mChilds[i] = 
            BuildTopDownAdaptiveOctree(&node->mChilds[i], height + 1, node->mFaceList, center + offset, step);
        }
    }
}
void Octree::DrawOctree(unsigned shader)
{
    DrawNode(mRoot, GFX::getInstance()->GetOctreeHeight(), shader);
}
void Octree::DrawNode(OctreeNode* node, unsigned height, unsigned shader)
{
    if (height == 0)
        return;

    for (int i = 0; i < 8; ++i)
    {
        if (node == nullptr)
            continue;

        DrawAABB(node->mAABB, GetColor(height - 1));

        DrawNode(node->mChilds[i], height - 1, shader);
    }
}
void Octree::DeleteNode(OctreeNode* node)
{
    if (node)
    {
        for (unsigned i = 0; i < 8; ++i)
            DeleteNode(node->mChilds[i]);
        delete(node);
    }
}