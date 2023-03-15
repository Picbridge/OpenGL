/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: <BVH.cpp>
Purpose: <BVH topdown,bottomup>
Language: <c++,visual studio2017>
//Platform: <visual studio2017 , win10>
Project: <jihoon.bae_CS350_2>
Author: <jihoon.bae>
Creation date: <22/02/2019>
End Header --------------------------------------------------------*/
#include "BVH.h"
#include "GFX.h"

BVH::BVH() : mRoot_TopDown(0), mRoot_BotUp(0)
{
}

void BVH::ConstructBVH( std::vector<MeshRenderer*>& meshRenderers )
{
  mObjects = &meshRenderers;

  for(unsigned i = 0; i < meshRenderers.size(); ++i)
  {
    std::vector<Vertex> verts = meshRenderers[i]->GetMesh()->GetVertices();

    meshRenderers[i]->transform.Update();
    Matrix4 worldTransform = meshRenderers[i]->transform.GetWorld();
    worldTransform.Transpose();

    for(unsigned j = 0; j < verts.size(); ++j)
    {
      Vector3 v = TransformPoint(worldTransform, verts[j].mPos);
      mVertexSoup.push_back(v);
    }

    verts.clear();
  }

  ConstructTopDownBVH();
  ConstructBotUpBVH();
}

void BVH::ConstructTopDownBVH()
{
  TopDown_Rec(&mRoot_TopDown, 0, mVertexSoup.size(), 0);
}

void BVH::TopDown_Rec( BVHNode** pnode, unsigned b, unsigned e, int height )
{
  *pnode = new BVHNode();
  BVHNode* node = *pnode;

  // Base case - 500 vertices
  if(e - b <= 500)
  {
    SplitAABB(node->mBVHInfo.mAABB, b, e);
    return;
  }

  // Partition - Find split point(median)
  // Text X Y Z Plane - Find smallest child volumes
  unsigned bestPlane;
  float minVolume = FLT_MAX;

  Vector3 max(FLT_MIN), min(FLT_MAX);
  for(unsigned i = b; i < e; ++i)
  {
    max = Max(max, mVertexSoup[i]);
    min = Min(min, mVertexSoup[i]);
  }

  
  for(unsigned i = 0; i < 3; ++i)
  {
    switch(i)
    {
    case 0:
      std::sort(mVertexSoup.begin() + b, mVertexSoup.begin() + e, sortByX());
      break;

    case 1:
      std::sort(mVertexSoup.begin() + b, mVertexSoup.begin() + e, sortByY());
      break;

    case 2:
      std::sort(mVertexSoup.begin() + b, mVertexSoup.begin() + e, sortByZ());
      break;
    }

    float vol = ComputeChildVolumes(b, e);

    if(vol < minVolume)
    {
      bestPlane = i;
      minVolume = vol;
    }
  }

  switch(bestPlane)
  {
  case 0:
    std::sort(mVertexSoup.begin() + b, mVertexSoup.begin() + e, sortByX());
    break;

  case 1:
    std::sort(mVertexSoup.begin() + b, mVertexSoup.begin() + e, sortByY());
    break;

  case 2:
    std::sort(mVertexSoup.begin() + b, mVertexSoup.begin() + e, sortByZ());
    break;
  }

  SplitAABB(node->mBVHInfo.mAABB, b, e);
  node->mBVHInfo.mSphere.Centre = node->mBVHInfo.mAABB.Centre;
  SplitSphere(node->mBVHInfo.mSphere, b, e);

  unsigned median = ((e - b) / 2) + 1 + b;
  TopDown_Rec(&node->left, b, median, height + 1);
  TopDown_Rec(&node->right, median, e, height + 1);
}

void BVH::ConstructBotUpBVH()
{
  std::vector<BVHNode*> tree;
  std::vector<BVHNode*> queue;

  for(unsigned i = 0; i < (*mObjects).size(); ++i)
  {
    BVHNode* node = new BVHNode();
    node->mBVHInfo.mAABB = (*mObjects)[i]->GetBVInfo()->mAABB;
    node->mBVHInfo.mAABB.Centre += (*mObjects)[i]->transform.GetPosition();
    node->mBVHInfo.mAABB.HalfExtent /= Sqrt(3);

    node->mBVHInfo.mSphere = (*mObjects)[i]->GetBVInfo()->mPCASphere;
    node->mBVHInfo.mSphere.Centre += (*mObjects)[i]->transform.GetPosition();
    tree.push_back(node);
  }

  while(tree.size() > 1)
  {
    queue = tree;
    tree.clear();

    while(queue.size())
    {
      if(queue.size() == 1)
      {
        tree.push_back(queue[0]);
        queue.clear();
        break;
      }

      std::pair<unsigned, unsigned> bestpair;
      float dist = FLT_MAX;

      // Find shortest pair
      for(unsigned i = 0; i < queue.size(); ++i)
      {
        for(unsigned j = i + 1; j < queue.size(); ++j)
        {
          Vector3 d = queue[j]->mBVHInfo.mAABB.Centre - queue[i]->mBVHInfo.mAABB.Centre;
          float distSq = d.LengthSq();

          if(distSq < dist)
          {
            dist = distSq;
            bestpair.first = i;
            bestpair.second = j;
          }
        }
      }

      BVHNode* node = new BVHNode();
      node->left = queue[bestpair.first];
      node->right = queue[bestpair.second];

      node->mBVHInfo.mAABB = MergeAABB(queue[bestpair.first]->mBVHInfo.mAABB,
                                       queue[bestpair.second]->mBVHInfo.mAABB);
      node->mBVHInfo.mSphere = MergeSphere(queue[bestpair.first]->mBVHInfo.mSphere,
                                           queue[bestpair.second]->mBVHInfo.mSphere);

      if(bestpair.first > bestpair.second)
      {
        queue.erase(queue.begin() + bestpair.first);
        queue.erase(queue.begin() + bestpair.second);
      }
      else
      {
        queue.erase(queue.begin() + bestpair.second);
        queue.erase(queue.begin() + bestpair.first);
      }

      tree.push_back(node);
    }
  }

  mRoot_BotUp = tree[0];
}

void BVH::BotUp_Rec( BVHNode* node, int height )
{

}

void BVH::SplitAABB( AABB& box, unsigned b, unsigned e)
{
  Vector3 max(FLT_MIN), min(FLT_MAX);
  for(unsigned i = b; i < e; ++i)
  {
    max = Max(max, mVertexSoup[i]);
    min = Min(min, mVertexSoup[i]);
  }
  
  box.Centre = (max + min) * .5f;
  box.HalfExtent = (max - box.Centre);
}

void BVH::SplitSphere( Sphere& s, unsigned b, unsigned e )
{
  for(unsigned i = b; i < e; ++i)
  {
    // Find furthest point from centre
    float radius = (mVertexSoup[i] - s.Centre).LengthSq();
    s.Radius = Math::Max(s.Radius, radius);
  }

  s.Radius = Math::Sqrt(s.Radius);
}

float BVH::ComputeChildVolumes( unsigned b, unsigned e)
{
  float volume = 0;
  Vector3 max(FLT_MIN), min(FLT_MAX);

  unsigned median = ((e - b) / 2) + 1 + b;

  for(unsigned i = b; i < median; ++i)
  {
    max = Max(max, mVertexSoup[i]);
    min = Min(min, mVertexSoup[i]);
  }

  max -= min;
  volume += max.x * max.y * max.z;

  max.Splat(FLT_MIN);
  min.Splat(FLT_MAX);

  for(unsigned i = median; i < e; ++i)
  {
    max = Max(max, mVertexSoup[i]);
    min = Min(min, mVertexSoup[i]);
  }

  max -= min;
  volume += max.x * max.y * max.z;

  return volume;
}

void BVH::Destroy()
{
  DestroyNode(mRoot_TopDown);
  DestroyNode(mRoot_BotUp);
}

void BVH::DestroyNode( BVHNode* node )
{
  if(node)
  {
    DestroyNode(node->left);
    DestroyNode(node->right);
    delete(node);
  }
}

void BVH::DrawBVH(unsigned shader)
{
  BVHOPTION opt = GFX::getInstance()->GetBVHOption();
  
  switch (opt)
  {
  case BVH_TopDown:
    DrawNode(mRoot_TopDown, 0, shader);
    break;

  case BVH_BotUp:
    DrawNode(mRoot_BotUp, 0, shader);
    break;
  }
    
}

void BVH::DrawNode( BVHNode* node, unsigned height, unsigned shader )
{
  if(GFX::getInstance()->GetBVHTermination() == BVH_TOPDOWN_HEIGHT &&
     height > 7)
     return;

  if(node)
  {
    DrawNode(node->left, height + 1, shader);
    DrawNode(node->right, height + 1, shader);

    if(GFX::getInstance()->GetBVHDraw() == BVH_AABB)
    {
      if(GFX::getInstance()->GetTreeHeight() < 0)
        DrawAABB(node->mBVHInfo.mAABB, GetColor(height) + Vector4(0,0,0,.4f)); 

      else if(height == GFX::getInstance()->GetTreeHeight())
        DrawAABB(node->mBVHInfo.mAABB, GetColor(height) + Vector4(0,0,0,.4f));    
    }
    else
    {
      if(GFX::getInstance()->GetTreeHeight() < 0)
        DrawSphere(node, GetColor(height), shader); 

      else if(height == GFX::getInstance()->GetTreeHeight())
        DrawSphere(node, GetColor(height), shader);
    }
  }  
}

void BVH::DrawAABB( AABB& box, Vector4 color )
{
  Vector3 topLeftF, topRightF, botLeftF, botRightF, 
          topLeftB, topRightB, botLeftB, botRightB;

  topLeftF = topRightF = botLeftF = botRightF = 
  topLeftB = topRightB = botLeftB = botRightB = box.Centre;

  topRightF += box.HalfExtent;
  botLeftB -= box.HalfExtent;

  topLeftF.x -= box.HalfExtent.x;
  topLeftF.y += box.HalfExtent.y;
  topLeftF.z += box.HalfExtent.z;

  botLeftF.x -= box.HalfExtent.x;
  botLeftF.y -= box.HalfExtent.y;
  botLeftF.z += box.HalfExtent.z;

  botRightF.x += box.HalfExtent.x;
  botRightF.y -= box.HalfExtent.y;
  botRightF.z += box.HalfExtent.z;

  topLeftB.x -= box.HalfExtent.x;
  topLeftB.y += box.HalfExtent.y;
  topLeftB.z -= box.HalfExtent.z;

  topRightB.x += box.HalfExtent.x;
  topRightB.y += box.HalfExtent.y;
  topRightB.z -= box.HalfExtent.z;

  botRightB.x += box.HalfExtent.x;
  botRightB.y -= box.HalfExtent.y;
  botRightB.z -= box.HalfExtent.z;
  
  glColor4f(color.x, color.y, color.z, color.w);

  glBegin(GL_LINES);

  glVertex3f(topRightF.x, topRightF.y, topRightF.z);
  glVertex3f(topLeftF.x, topLeftF.y, topLeftF.z);
  glVertex3f(topRightF.x, topRightF.y, topRightF.z);
  glVertex3f(topRightB.x, topRightB.y, topRightB.z);
  glVertex3f(topRightF.x, topRightF.y, topRightF.z);
  glVertex3f(botRightF.x, botRightF.y, botRightF.z);

  glVertex3f(topLeftB.x, topLeftB.y, topLeftB.z);
  glVertex3f(botLeftB.x, botLeftB.y, botLeftB.z);
  glVertex3f(topLeftB.x, topLeftB.y, topLeftB.z);
  glVertex3f(topLeftF.x, topLeftF.y, topLeftF.z);
  glVertex3f(topLeftB.x, topLeftB.y, topLeftB.z);
  glVertex3f(topRightB.x, topRightB.y, topRightB.z);

  glVertex3f(botLeftF.x, botLeftF.y, botLeftF.z);
  glVertex3f(botLeftB.x, botLeftB.y, botLeftB.z);
  glVertex3f(botLeftF.x, botLeftF.y, botLeftF.z);
  glVertex3f(topLeftF.x, topLeftF.y, topLeftF.z);
  glVertex3f(botLeftF.x, botLeftF.y, botLeftF.z);
  glVertex3f(botRightF.x, botRightF.y, botRightF.z);

  glVertex3f(botRightB.x, botRightB.y, botRightB.z);
  glVertex3f(botLeftB.x, botLeftB.y, botLeftB.z);
  glVertex3f(botRightB.x, botRightB.y, botRightB.z);
  glVertex3f(topRightB.x, topRightB.y, topRightB.z);
  glVertex3f(botRightB.x, botRightB.y, botRightB.z);
  glVertex3f(botRightF.x, botRightF.y, botRightF.z);

  glEnd();
}

Vector4 BVH::GetColor( unsigned index )
{
  unsigned totalCol = sizeof(Colors) / sizeof(Vector4);
  return Colors[index % totalCol];
}

AABB BVH::MergeAABB( AABB& b0, AABB& b1 )
{
  AABB newBox;

  Vector3 max = Max(b0.Centre + b0.HalfExtent, b1.Centre + b1.HalfExtent);
  Vector3 min = Min(b0.Centre - b0.HalfExtent, b1.Centre - b1.HalfExtent);

  newBox.Centre = (max + min) *.5f;
  newBox.HalfExtent = max - newBox.Centre;

  return newBox;
}

Sphere BVH::MergeSphere( Sphere& s0, Sphere& s1 )
{
  Sphere newSphere;

  Vector3 d = s1.Centre - s0.Centre;
  float dist2 = d.LengthSq();

  if (Pow(s1.Radius - s0.Radius, 2) >= dist2) 
  {
    if (s1.Radius >= s0.Radius)
      newSphere = s1;
    else
      newSphere = s0;
  } 

  else 
  {
    float dist = d.Length();

    newSphere.Radius = (dist + s0.Radius + s1.Radius) * 0.5f;
    newSphere.Centre = s0.Centre;
    if (dist > 0.0001f)
      newSphere.Centre += ((newSphere.Radius - s0.Radius) / dist) * d;
  }

  return newSphere;
}

void BVH::DrawSphere( BVHNode* node, Vector4 color, unsigned shader )
{
  node->mVolume->transform.SetPosition(node->mBVHInfo.mSphere.Centre);
  node->mVolume->transform.SetScale(node->mBVHInfo.mSphere.Radius);
  node->mVolume->SetColor(color);
  node->mVolume->transform.Update();

  node->mVolume->BindMesh(shader);
  node->mVolume->GetMesh()->DrawMesh();
}
