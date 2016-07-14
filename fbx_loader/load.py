import FbxCommon

(M, S) = FbxCommon.InitializeSdkObjects()
ret = FbxCommon.LoadScene(M, S, "sphere_anim.fbx")
print(ret)