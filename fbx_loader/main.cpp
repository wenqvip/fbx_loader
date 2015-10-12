#include <stdio.h>
#include <stdlib.h>
#include <fbxsdk.h>
#include <string>
using namespace std;

#pragma comment(lib, "libfbxsdk-md.lib")

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
void PrintAttribute(FbxNodeAttribute* pAttribute) {
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
		for (int i = 0; i < pMesh->GetControlPointsCount(); i++)
		{
			PrintFbxVector4(IControlPoints[i]);
		}
		--numTabs;

		PrintTabs();
		printf("Mesh Index: \n");
		++numTabs;
		int* pIndices = pMesh->GetPolygonVertices();
		for (int i = 0; i < pMesh->GetPolygonVertexCount(); ++i)
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
			for (int i = 0; i < pNormal->mDirectArray->GetCount(); i++)
			{
				PrintFbxVector4((*pNormal->mDirectArray)[i]);
			}
			--numTabs;

			FbxLayerElementUV* pUV = pLayer->GetUVs();
			PrintTabs();
			printf("UVs:\n");
			++numTabs;
			for (int i = 0; i < pUV->mDirectArray->GetCount(); i++)
			{
				PrintFbxVector2((*pUV->mDirectArray)[i]);
			}
			--numTabs;
		}
		--numTabs;
	}
}

void PrintNode(FbxNode* pNode) {
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
		PrintAttribute(pNode->GetNodeAttributeByIndex(i));

	// Recursively print the children.
	for (int j = 0; j < pNode->GetChildCount(); j++)
		PrintNode(pNode->GetChild(j));

	numTabs--;
	PrintTabs();
	printf("</node>\n");
}

int main()
{
	freopen("sphere.txt", "w", stdout);
	const char* lFilename = "sphere.fbx";
	FbxManager* lSdkManager = FbxManager::Create();
	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	if (!lImporter->Initialize(lFilename, -1, lSdkManager->GetIOSettings())) {
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
			PrintNode(lRootNode->GetChild(i));
	}
	lSdkManager->Destroy();
	return 0;
}