#include <stdio.h>
#include <stdlib.h>
#include <fbxsdk.h>
#include <string>
using namespace std;

#pragma comment(lib, "libfbxsdk.lib")

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

void ConvertMapping(FbxLayerElementUV* pUVs, FbxMesh* pMesh)
{
	if (pUVs->GetMappingMode() == FbxLayerElement::eByPolygonVertex)
	{
		if (pUVs->GetReferenceMode() == FbxLayerElement::eIndexToDirect)
		{
			FbxArray<int> lNewIndexToDirect;
			FbxArray<FbxVector2> lNewUVs;

			// keep track of the processed control points
			
			FbxSet<FbxHandle> lProcessedCP;
			lProcessedCP.Reserve(pMesh->GetControlPointsCount());
			//for (int i = 0; i < pMesh->GetControlPointsCount(); i++)
				//lProcessedCP.Insert((FbxHandle)i);

			// visit each polygon and polygon vertex
			for (int p = 0; p < pMesh->GetPolygonCount(); p++)
			{
				for (int pv = 0; pv < pMesh->GetPolygonSize(p); pv++)
				{
					int lCP = pMesh->GetPolygonVertex(p, pv);

					// check if we already processed this control point
					if (lProcessedCP.Find((FbxHandle)lCP) == NULL)
					{
						FbxVector2 uv = pUVs->GetDirectArray().GetAt(lCP);
						lNewUVs.Add(uv);
						lNewIndexToDirect.Add(lCP);
						lProcessedCP.Insert((FbxHandle)lCP);
					}
				}
			}

			// change the content of the index array and its mapping
			pUVs->SetMappingMode(FbxLayerElement::eByControlPoint);
			pUVs->GetIndexArray().Clear();
			pUVs->GetIndexArray().Resize(lNewIndexToDirect.GetCount());
			for (int i = 0; i < lNewIndexToDirect.GetCount(); i++)
				pUVs->mIndexArray->SetAt(i, lNewIndexToDirect.GetAt(i));

			// and the content of the direct array
			pUVs->GetDirectArray().Clear();
			pUVs->GetDirectArray().Resize(lNewUVs.GetCount());
			for (int j = 0; j < lNewUVs.GetCount(); j++)
				pUVs->mDirectArray->SetAt(j, lNewUVs.GetAt(j));
		}
	}
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
			if (pNormal)
			{
				FbxLayerElement::EReferenceMode rnmode = pNormal->GetReferenceMode();

				PrintTabs();
				printf("Normals:\n");
				++numTabs;
				for (int i = 0; i < pNormal->mDirectArray->GetCount(); i++)
				{
					PrintFbxVector4((*pNormal->mDirectArray)[i]);
				}
				--numTabs;
			}

			FbxLayerElementUV* pUV = pLayer->GetUVs();
			if (pUV)
			{
				FbxLayerElement::EMappingMode mode = pUV->GetMappingMode();
				FbxLayerElement::EReferenceMode rmode = pUV->GetReferenceMode();
				PrintTabs();
				printf("UVs:\n");
				++numTabs;
				for (int i = 0; i < pUV->mDirectArray->GetCount(); i++)
				{
					PrintFbxVector2((*pUV->mDirectArray)[i]);
				}
				--numTabs;
				PrintTabs();
				printf("UV indices:\n");
				++numTabs;
				for (int i = 0; i < pUV->mIndexArray->GetCount(); i++)
				{
					PrintTabs();
					printf("%d\n", (*pUV->mIndexArray)[i]);
					//PrintFbxVector2((*pUV->mIndexArray)[i]);
				}
				--numTabs;
			}
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

std::string FBX_NAME = "HumanoidIdle";
std::string FBX_OUT = FBX_NAME + ".txt";
std::string FBX_IN = FBX_NAME + ".fbx";

int main()
{
	freopen(FBX_OUT.c_str(), "w", stdout);

	FbxManager* lSdkManager = FbxManager::Create();
	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

	if (!lImporter->Initialize(FBX_IN.c_str(), -1, lSdkManager->GetIOSettings()))
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		exit(-1);
	}

	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");
	lImporter->Import(lScene);
	lImporter->Destroy();

	FbxNode* lRootNode = lScene->GetRootNode();
	if (lRootNode)
	{
		for (int i = 0; i < lRootNode->GetChildCount(); i++)
			PrintNode(lRootNode->GetChild(i));
	}
	lSdkManager->Destroy();
	return 0;
}