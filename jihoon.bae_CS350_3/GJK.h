/* Start Header -------------------------------------------------------
Copyright (C) 2018 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: <GJK.h>
Purpose: <Applying GJK to application>
Language: <c++,visual studio2017>
//Platform: <visual studio2017 , win10>
Project: <jihoon.bae_CS350_4>
Author: <jihoon.bae>
Creation date: <25/04/2019>
End Header --------------------------------------------------------*/
#pragma once

#include "MeshRenderer.h"
#include "MeshFactory.h"

#define GJK_MAX 64

class GJK
{
public:

	bool GJK(Collider* coll1, Collider* coll2, vec3* mtv = NULL);
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