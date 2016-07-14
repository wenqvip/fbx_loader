#pragma comment(lib, "libfbxsdk.lib")
#define FBXSDK_SHARED
#include <fbxsdk.h>

#include <stdio.h>
#include <stdlib.h>
#include <string>

using namespace std;
/* Tab character ("\t") counter */
int numTabs = 0;

/**
* Print the required number of tabs.
*/
void PrintTabs() {
	for (int i = 0; i < numTabs; i++)
		printf("\t");
}

/**
* Return a string-based representation based on the attribute type.
*/
FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) {
	switch (type) {
	case FbxNodeAttribute::eUnknown: return "unidentified";
	case FbxNodeAttribute::eNull: return "null";
	case FbxNodeAttribute::eMarker: return "marker";
	case FbxNodeAttribute::eSkeleton: return "skeleton";
	case FbxNodeAttribute::eMesh: return "mesh";
	case FbxNodeAttribute::eNurbs: return "nurbs";
	case FbxNodeAttribute::ePatch: return "patch";
	case FbxNodeAttribute::eCamera: return "camera";
	case FbxNodeAttribute::eCameraStereo: return "stereo";
	case FbxNodeAttribute::eCameraSwitcher: return "camera switcher";
	case FbxNodeAttribute::eLight: return "light";
	case FbxNodeAttribute::eOpticalReference: return "optical reference";
	case FbxNodeAttribute::eOpticalMarker: return "marker";
	case FbxNodeAttribute::eNurbsCurve: return "nurbs curve";
	case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface";
	case FbxNodeAttribute::eBoundary: return "boundary";
	case FbxNodeAttribute::eNurbsSurface: return "nurbs surface";
	case FbxNodeAttribute::eShape: return "shape";
	case FbxNodeAttribute::eLODGroup: return "lodgroup";
	case FbxNodeAttribute::eSubDiv: return "subdiv";
	default: return "unknown";
	}
}

void PrintFbxVector4(FbxVector4 fv4)
{
	PrintTabs();
	printf("(%f, %f, %f, %f)\n", fv4.mData[0], fv4.mData[1], fv4.mData[2], fv4.mData[3]);
}

void PrintFbxVector2(FbxVector2 fv4)
{
	PrintTabs();
	printf("(%f, %f)\n", fv4.mData[0], fv4.mData[1]);
}

/**
* Print an attribute.
*/
void PrintAttribute(FbxNodeAttribute* pAttribute, bool detail) {
	if (!pAttribute) return;

	FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
	FbxString attrName = pAttribute->GetName();
	PrintTabs();
	// Note: to retrieve the character array of a FbxString, use its Buffer() method.
	printf("<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
	if (string("mesh") == typeName.Buffer())
	{
		FbxMesh* pMesh = (FbxMesh*)pAttribute;

		PrintTabs();
		printf("Mesh Control Points: \n");
		FbxVector4* IControlPoints = pMesh->GetControlPoints();
		++numTabs;
		int count = detail ? pMesh->GetControlPointsCount() : 
			(pMesh->GetControlPointsCount() < 3 ? pMesh->GetControlPointsCount() : 3);
		for (int i = 0; i < count; i++)
		{
			PrintFbxVector4(IControlPoints[i]);
		}
		--numTabs;

		PrintTabs();
		printf("Mesh Index: \n");
		++numTabs;
		int* pIndices = pMesh->GetPolygonVertices();
		count = detail ? pMesh->GetPolygonVertexCount() : 
			(pMesh->GetPolygonVertexCount() < 3 ? pMesh->GetPolygonVertexCount() : 3);
		for (int i = 0; i < count; ++i)
		{
			PrintTabs();
			printf("%d\n", pIndices[i]);
		}
		--numTabs;

		PrintTabs();
		printf("Mesh Layer Info: \n");
		++numTabs;
		for (int i = 0; i < pMesh->GetLayerCount(); i++)
		{
			FbxLayer* pLayer = pMesh->GetLayer(i);
			FbxLayerElementNormal* pNormal = pLayer->GetNormals();

			PrintTabs();
			printf("Normals:\n");
			++numTabs;
			count = detail ? pNormal->mDirectArray->GetCount() : 
				(pNormal->mDirectArray->GetCount() < 3 ? pNormal->mDirectArray->GetCount() : 3);
			for (int i = 0; i < count; i++)
			{
				PrintFbxVector4((*pNormal->mDirectArray)[i]);
			}
			--numTabs;

			FbxLayerElementUV* pUV = pLayer->GetUVs();
			PrintTabs();
			printf("UVs:\n");
			++numTabs;
			count = detail ? pUV->mDirectArray->GetCount() : 
				(pUV->mDirectArray->GetCount() < 3 ? pUV->mDirectArray->GetCount() : 3);
			for (int i = 0; i < count; i++)
			{
				PrintFbxVector2((*pUV->mDirectArray)[i]);
			}
			--numTabs;

			FbxLayerElementTangent* pTangent = pLayer->GetTangents();
			if (pTangent)
			{
				PrintTabs();
				printf("Tangents:\n");
				++numTabs;
				count = detail ? pTangent->mDirectArray->GetCount() : 
					(pTangent->mDirectArray->GetCount() < 3 ? pTangent->mDirectArray->GetCount() : 3);
				for (int i = 0; i < count; i++)
				{
					PrintFbxVector4((*pTangent->mDirectArray)[i]);
				}
				--numTabs;
			}

			for (int i = FbxLayerElement::eTextureDiffuse; i < FbxLayerElement::eTypeCount; i++)
			{
				FbxLayerElementTexture* pTextures = pLayer->GetTextures((FbxLayerElement::EType)i);
				if (pTextures)
				{
					PrintTabs();
					printf("Textures:\n");
					++numTabs;
					count = detail ? pTextures->mDirectArray->GetCount() : 
						pTextures->mDirectArray->GetCount() < 3 ? pTextures->mDirectArray->GetCount() : 3;
					for (int j = 0; j < count; j++)
					{
						printf("%d:%s\n", i, (*pTextures->mDirectArray)[j]->GetUrl());
					}
					--numTabs;
				}
			}
		}
		--numTabs;
	}
}

void PrintNode(FbxNode* pNode, bool detail) {
	PrintTabs();
	const char* nodeName = pNode->GetName();
	FbxDouble3 translation = pNode->LclTranslation.Get();
	FbxDouble3 rotation = pNode->LclRotation.Get();
	FbxDouble3 scaling = pNode->LclScaling.Get();

	// Print the contents of the node.
	printf("<node name='%s' translation='(%f, %f, %f)' rotation='(%f, %f, %f)' scaling='(%f, %f, %f)'>\n",
		nodeName,
		translation[0], translation[1], translation[2],
		rotation[0], rotation[1], rotation[2],
		scaling[0], scaling[1], scaling[2]
		);
	numTabs++;

	// Print the node's attributes.
	for (int i = 0; i < pNode->GetNodeAttributeCount(); i++)
		PrintAttribute(pNode->GetNodeAttributeByIndex(i), detail);

	// Recursively print the children.
	for (int j = 0; j < pNode->GetChildCount(); j++)
		PrintNode(pNode->GetChild(j), detail);

	numTabs--;
	PrintTabs();
	printf("</node>\n");
}

void PrintAnimation(FbxScene* lScene, bool detail)
{
	printf("\n---Animation Informations---\n");
	int numStacks = lScene->GetSrcObjectCount<FbxAnimStack>();
	printf("There are %d animation stack(s)\n", numStacks);
	for (int i = 0; i < numStacks; i++)
	{
		FbxAnimStack* pAnimStack = FbxCast<FbxAnimStack>(lScene->GetSrcObject<FbxAnimStack>());
		printf("--%s\n", pAnimStack->GetName());
		int numLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();
		numTabs++;
		for (int j = 0; j < numLayers; j++)
		{
			FbxAnimLayer* lAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(j);
			PrintTabs();
			printf("%s\n", lAnimLayer->GetName());
		}
		numTabs--;
	}
}

int main(int argc, char* argv[])
{
	string filename = "zhankuang.fbx";
	bool detail = false;
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			int pn = strlen(argv[i]);
			for (int j = 1; j < pn; j++)
			{
				if (argv[i][j] == 'd' || argv[i][j] == 'D')
				{
					detail = true;
				}
			}
		}
		else
		{
			filename = argv[i];
		}
	}
	string outfile = filename + ".txt";
	freopen(outfile.c_str(), "w", stdout);

	FbxManager* lSdkManager = FbxManager::Create();
	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	if (!lImporter->Initialize(filename.c_str(), -1, lSdkManager->GetIOSettings())) {
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		exit(-1);
	}

	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");
	lImporter->Import(lScene);
	lImporter->Destroy();

	FbxNode* lRootNode = lScene->GetRootNode();
	if (lRootNode) {
		for (int i = 0; i < lRootNode->GetChildCount(); i++)
			PrintNode(lRootNode->GetChild(i), detail);
	}
	PrintAnimation(lScene, detail);

	lSdkManager->Destroy();
	return 0;
}