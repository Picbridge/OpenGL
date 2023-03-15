/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: <BVH.h>
Purpose: <BVH topdown,bottomup>
Language: <c++,visual studio2017>
//Platform: <visual studio2017 , win10>
Project: <Spatial_Partitioning>
Author: <jihoon.bae>
Creation date: <22/02/2019>
End Header --------------------------------------------------------*/
#pragma once

#include "MeshRenderer.h"
#include "MeshFactory.h"

enum BVHOPTION
{
  BVH_NA,
  BVH_TopDown,
  BVH_BotUp,  
  BVH_Octree,
  BVH_BSPTree
};

enum BVHDRAW
{
  BVH_AABB,
  BVH_SPHERE,

};

enum BVHTERMINATION
{
  BVH_TOPDOWN_VERTICES,
  BVH_TOPDOWN_HEIGHT
};

struct sortByX
{
  bool operator()( const Vector3& lhs, const Vector3& rhs ) const
  {
    return lhs.x < rhs.x;
  }
};

struct sortByY
{
  bool operator()( const Vector3& lhs, const Vector3& rhs ) const
  {
    return lhs.y < rhs.y;
  }
};

struct sortByZ
{
  bool operator()( const Vector3& lhs, const Vector3& rhs ) const
  {
    return lhs.z < rhs.z;
  }
};

struct BVHInfo
{
  AABB mAABB;
  Sphere mSphere;
};

struct BVHNode
{
public:

  BVHNode(): left(0), right(0)
  {
    mVolume = new MeshRenderer();
    mVolume->SetMesh(MeshFactory::getInstance()->GetMesh(MESH_SPHERE));
  }

  ~BVHNode()
  {
    if(mVolume)
      delete mVolume;
  }

  BVHNode* left;
  BVHNode* right;

  MeshRenderer* mVolume;
  BVHInfo mBVHInfo;
};

class BVH
{
public:

  BVH();
  void ConstructBVH(std::vector<MeshRenderer*>& meshRenderers);
  void ConstructTopDownBVH();
  void ConstructBotUpBVH();
  void DrawBVH(unsigned shader = 0);
  void Destroy();
  AABB GetSceneBounds() { return mRoot_TopDown->mBVHInfo.mAABB; }
private:

  void TopDown_Rec(BVHNode** node, unsigned b, unsigned e, int height);
  void BotUp_Rec(BVHNode* node, int height);
  void SplitAABB(AABB& box, unsigned b, unsigned e);
  void SplitSphere(Sphere& s, unsigned b, unsigned e);
  float ComputeChildVolumes(unsigned b, unsigned e);
  void DestroyNode(BVHNode* node);
  void DrawNode(BVHNode* node, unsigned height, unsigned shader);
  void DrawAABB(AABB& box, Vector4 color); 
  void DrawSphere(BVHNode* node, Vector4 color, unsigned shader);
  Vector4 GetColor(unsigned index);
  AABB MergeAABB(AABB& b0, AABB& b1);
  Sphere MergeSphere(Sphere& s0, Sphere& s1);

  BVHNode* mRoot_TopDown;
  BVHNode* mRoot_BotUp;
  std::vector<Vector3> mVertexSoup;
  std::vector<MeshRenderer*>* mObjects;
};