﻿#include "IKEFbxLoader.h"

#include <cassert>


using namespace DirectX;

/// <summary>
/// 静的メンバを変数の実体
/// </summary>
const std::string IKEFbxLoader::baseDirectory = "Resources/3d/";
const std::string IKEFbxLoader::texbaseDirectory = "Resources/2d/";
const std::string IKEFbxLoader::defaultTextureFileName = "white1x1.png";

IKEFbxLoader* IKEFbxLoader::GetInstance()
{
	static IKEFbxLoader instance;
	return &instance;
}

void IKEFbxLoader::Initialize(ID3D12Device* device)
{
	//再初期化チェック
	assert(fbxManager == nullptr);
	//引数からメンバ変数に代入
	this->device = device;
	//FBXマネージャの生成
	fbxManager = FbxManager::Create();
	//FBXマネージャの入出力設定
	FbxIOSettings* ios = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ios);
	//FBXインポータの生成
	fbxImporter = FbxImporter::Create(fbxManager, "");
}

void IKEFbxLoader::Finalize()
{
	//各種FBXインスタンスの破棄
	fbxImporter->Destroy();
	fbxManager->Destroy();
}

IKEFBXModel* IKEFbxLoader::LoadModelFromFile(const string& ModelName)
{
	//モデルと同じ名前のファイルから読み込む
	const string directoryPath = baseDirectory + ModelName + "/";
	//拡張子,FBXを付与
	const string fileName = ModelName + ".fbx";
	//連結してフルパスを得る
	const string fullpath = directoryPath + fileName;

	//ファイル名を指定してFBXファイルを読み込む

	if (!fbxImporter->Initialize(fullpath.c_str(), -1, fbxManager->GetIOSettings()))
	{
		assert(0);
	}

	//シーン生成
	FbxScene* fbxScene = FbxScene::Create(fbxManager, "fbxScene");

	//ファイルからロードしたFBXに情報をシーンにインポート
	fbxImporter->Import(fbxScene);

	//モデル生成
	auto fbxModel = new IKEFBXModel();
	fbxModel->name = ModelName;
	//FBXノードの数を取得
	int nodeCount = fbxScene->GetNodeCount();
	//あらかじめ必要数分のメモリを確保することで、アドレスがずれるのを予防
	fbxModel->nodes.reserve(nodeCount);
	//ルートノードから順に解析してもでるに流し込む
	ParseNodeRecursive(fbxModel, fbxScene->GetRootNode());
	//FBXシーン解放
	fbxModel->fbxScene = fbxScene;

	//バッファ生成
	fbxModel->CreateBuffers(device);

	return fbxModel;
}

void IKEFbxLoader::ParseNodeRecursive(IKEFBXModel* IKEFBXModel, FbxNode* fbxNode, Node* parent)
{
	//ノード名を取得
	string name = fbxNode->GetName();
	//モデルにノードを追加（Todo）
	IKEFBXModel->nodes.emplace_back();
	Node& node = IKEFBXModel->nodes.back();
	//ノード名を取得
	node.name = fbxNode->GetName();

	//FBXノードのローカル移動情報
	FbxDouble3 rotation = fbxNode->LclRotation.Get();
	FbxDouble3 scaling = fbxNode->LclScaling.Get();
	FbxDouble3 translation = fbxNode->LclTranslation.Get();

	//形式変換して代入
	node.rotation = {
		static_cast<float>(rotation[0]), static_cast<float>(rotation[1]), static_cast<float>(rotation[2]), 0.0f
	};
	node.scaling = {
		static_cast<float>(scaling[0]), static_cast<float>(scaling[1]), static_cast<float>(scaling[2]), 0.0f
	};
	node.translation = {
		static_cast<float>(translation[0]), static_cast<float>(translation[1]), static_cast<float>(translation[2]), 1.0f
	};

	//スケール、回転平行移動行列の計算
	XMMATRIX matScaling, matRotation, matTranslation;
	matScaling = XMMatrixScalingFromVector(node.scaling);
	matRotation = XMMatrixRotationRollPitchYawFromVector(node.rotation);
	matTranslation = XMMatrixTranslationFromVector(node.translation);

	//ローカル変形行列の計算
	node.transform = XMMatrixIdentity();
	node.transform *= matScaling; //ワールド行列にスケーリングを反映
	node.transform *= matRotation; //ワールド行列に回転を反映
	node.transform *= matTranslation; //ワールド行列に平行移動を反映
	//FBXノードの情報を解析してノードに記録（Todo）

	//グローバル変形行列の計算
	node.globalTransform = node.transform;
	if (parent)
	{
		node.parent = parent;
		//親の変形を乗算
		node.globalTransform *= parent->globalTransform;
	}
	//FBXノードのメッシュ情報を解析（Todo）
	FbxNodeAttribute* fbxNodeAttribute = fbxNode->GetNodeAttribute();

	if (fbxNodeAttribute)
	{
		if (fbxNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			IKEFBXModel->meshNode = &node;
			ParseMesh(IKEFBXModel, fbxNode);
		}
	}

	//子ノードに対して再帰呼び出し
	for (int i = 0; i < fbxNode->GetChildCount(); i++)
	{
		ParseNodeRecursive(IKEFBXModel, fbxNode->GetChild(i));
	}
}

void IKEFbxLoader::ParseMesh(IKEFBXModel* IKEFBXModel, FbxNode* fbxNode)
{
	//ノードのメッシュを取得
	FbxMesh* fbxMesh = fbxNode->GetMesh();

	//頂点座標読み取り
	ParseMeshVertices(IKEFBXModel, fbxMesh);
	//面を構成するでーたの読み取り
	ParseMeshFaces(IKEFBXModel, fbxMesh);
	//マテリアルの読み取り
	ParseMaterial(IKEFBXModel, fbxNode);
	//スキニング情報の読み取り
	ParseSkin(IKEFBXModel, fbxMesh);
}

void IKEFbxLoader::ConvertMatrixFromFbx(XMMATRIX* dst, const FbxAMatrix& src)
{
	//行
	for (int i = 0; i < 4; i++)
	{
		//列
		for (int j = 0; j < 4; j++)
		{
			//1要素コピー
			dst->r[i].m128_f32[j] = static_cast<float>(src.Get(i, j));
		}
	}
}

void IKEFbxLoader::ParseMeshVertices(IKEFBXModel* IKEFBXModel, FbxMesh* fbxMesh)
{
	auto& vertices = IKEFBXModel->vertices;

	//頂点座標データの数
	const int PolygonVertexCount = fbxMesh->GetPolygonVertexCount();
	//変更前fbxMesh->GetControlPointsCount() : 頂点数 / fbxMesh->GetPolygonVertexCount() : ポリゴン頂点インデックス数 
	fbxMesh->GetPolygonVertexCount();
	//必要数だけ頂点データ配列を確保
	IKEFBXModel::VertexPosNormalUvSkin vert{};
	IKEFBXModel->vertices.resize(PolygonVertexCount, vert);
}

void IKEFbxLoader::ParseMeshFaces(IKEFBXModel* IKEFBXModel, FbxMesh* fbxMesh)
{
	auto& vertices = IKEFBXModel->vertices; //564prin
	auto& indices = IKEFBXModel->indices;

	//頂点座標データ数最大
	const int PolygonVertexCountMax = fbxMesh->GetPolygonVertexCount() - 1;
	//カウント用
	int PolygonVertexCount = 0;

	//1ファイルに複数メッシュのモデルは非対応
	assert(indices.size() == 0);
	//面の数
	const int polygonCount = fbxMesh->GetPolygonCount();
	//UVデータ数
	const int textureUVCount = fbxMesh->GetTextureUVCount();
	//UV名リスト
	FbxStringList uvNames;
	fbxMesh->GetUVSetNames(uvNames);
	//面ごとの情報読み取り
	for (int i = 0; i < polygonCount; i++)
	{
		//面を構成する頂点の数を取得 (3なら三角形ポリゴン)
		const int polygonSize = fbxMesh->GetPolygonSize(i);
		assert(polygonSize <= 4);

		//1頂点ずつ処理
		for (int j = 0; j < polygonSize; j++)
		{
			//FBX頂点配列のインデックス
			int index = fbxMesh->GetPolygonVertex(i, j); //ポリゴン番号(i)と、ポリゴン内の頂点インデックス(j)から、頂点番号を取得する
			assert(index >= 0);

			//頂点法線読み込み
			IKEFBXModel::VertexPosNormalUvSkin& vertex = vertices[PolygonVertexCount];
			FbxVector4 normal;
			if (fbxMesh->GetPolygonVertexNormal(i, j, normal))
			{
				vertex.normal.x = static_cast<float>(normal[0]);
				vertex.normal.y = static_cast<float>(normal[1]);
				vertex.normal.z = static_cast<float>(normal[2]);
			}

			//テクスチャUV読み込み
			if (textureUVCount > 0)
			{
				FbxVector2 uvs;
				bool lUnmappedUV;
				// o番目決め打ちで読み込み
				if (fbxMesh->GetPolygonVertexUV(i, j, uvNames[0], uvs, lUnmappedUV))
				{
					vertex.uv.x = static_cast<float>(uvs[0]);
					vertex.uv.y = 1.0f - static_cast<float>(uvs[1]);
				}
			}

			//FBXメッシュの頂点座標配列を取得
			FbxVector4* pCoord = fbxMesh->GetControlPoints();

			//FBXメッシュの全頂点座標をモデル内の配列にコピーする
			//座標をコピー
			vertex.pos.x = static_cast<float>(pCoord[index][0]);
			vertex.pos.y = static_cast<float>(pCoord[index][1]);
			vertex.pos.z = static_cast<float>(pCoord[index][2]);


			//インデックス配列に頂点インデックス追加
			//3頂点までなら
			if (j < 3)
			{
				//1頂点追加し、他の2頂点と三角形を構築する
				indices.push_back(PolygonVertexCount);
			}
			else //4頂点目
			{
				//3頂点追加し
				//四角形の0,1,2,3の内 2,3,0で三角形を構築する
				int index2 = indices[indices.size() - 1];
				int index3 = index;
				int index0 = indices[indices.size() - 3];
				indices.push_back(index2);
				indices.push_back(index3);
				indices.push_back(index0);
			}

			if (PolygonVertexCount < PolygonVertexCountMax)
			{
				PolygonVertexCount++;
			}
		}
	}
}

void IKEFbxLoader::ParseMaterial(IKEFBXModel* IKEFBXModel, FbxNode* fbxNode)
{
	const int materialCount = fbxNode->GetMaterialCount();
	if (materialCount > 0)
	{
		//先頭のマテリアルを取得
		FbxSurfaceMaterial* material = fbxNode->GetMaterial(0);
		//テクスチャを読み込んだかどうかを表すフラグ
		bool textureLoaded = false;

		if (material)
		{
			if (material->GetClassId().Is(FbxSurfaceLambert::ClassId))
			{
				auto lambert = static_cast<FbxSurfaceLambert*>(material);

				//環境光係数
				FbxPropertyT<FbxDouble3> ambient = lambert->Ambient;
				IKEFBXModel->ambient.x = static_cast<float>(ambient.Get()[0]);
				IKEFBXModel->ambient.y = static_cast<float>(ambient.Get()[1]);
				IKEFBXModel->ambient.z = static_cast<float>(ambient.Get()[2]);

				//拡散反射光係数
				FbxPropertyT<FbxDouble3> diffuse = lambert->Diffuse;
				IKEFBXModel->diffuse.x = static_cast<float>(diffuse.Get()[0]);
				IKEFBXModel->diffuse.y = static_cast<float>(diffuse.Get()[1]);
				IKEFBXModel->diffuse.z = static_cast<float>(diffuse.Get()[2]);

				//ディフューズテクスチャを取り出す
				const FbxProperty diffuseProperty = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
				if (diffuseProperty.IsValid())
				{
					const FbxFileTexture* texture = diffuseProperty.GetSrcObject<FbxFileTexture>();
					if (texture)
					{
						const char* filepath = texture->GetFileName();
						//ファイルパスからファイル名抽出
						string path_str(filepath);
						string name = ExtractFileName(path_str);
						//テクスチャ読み込み
						LoadTexture(IKEFBXModel, baseDirectory + IKEFBXModel->name + "/" + name);
						textureLoaded = true;
					}
				}
			}
		}
		//テクスチャがない場合は白テクスチャを貼る
		if (!textureLoaded)
		{
			LoadTexture(IKEFBXModel, texbaseDirectory + defaultTextureFileName);
		}
	}
}

void IKEFbxLoader::LoadTexture(IKEFBXModel* IKEFBXModel, const std::string& fullpath)
{
	HRESULT result = S_FALSE;
	//WICテクスチャのロード
	TexMetadata& metadata = IKEFBXModel->metadata;
	ScratchImage& scratchImg = IKEFBXModel->scratchImg;
	//ユニコード文字列に変換
	wchar_t wfilepath[128];
	MultiByteToWideChar(CP_ACP, 0, fullpath.c_str(), -1, wfilepath, _countof(wfilepath));
	result = LoadFromWICFile(wfilepath, WIC_FLAGS_NONE, &metadata, scratchImg);
	if (FAILED(result))
	{
		assert(0);
	}
}

void IKEFbxLoader::ParseSkin(IKEFBXModel* IKEFBXModel, FbxMesh* fbxMesh)
{
	auto fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(0, FbxDeformer::eSkin));
	//スキニング情報がなければ終了
	if (fbxSkin == nullptr)
	{
		//各頂点についての処理
		for (int i = 0; i < IKEFBXModel->vertices.size(); i++)
		{
			//最初のボーン(単位行列)の影響100%にする
			IKEFBXModel->vertices[i].boneIndex[0] = 0;
			IKEFBXModel->vertices[i].boneWeight[0] = 1.0f;
		}
		return;
	}

	//ボーン配列の参照
	std::vector<IKEFBXModel::Bone>& bones = IKEFBXModel->bones;

	//ボーンの数
	int clusterCount = fbxSkin->GetClusterCount();
	bones.reserve(clusterCount);

	//全てのボーンについて
	for (int i = 0; i < clusterCount; i++)
	{
		//FBXボーン情報
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);

		//ボーン自体のノードの名前を取得
		const char* boneName = fbxCluster->GetLink()->GetName();

		//新しくボーンを追加し、追加したボーンの参照を得る
		bones.emplace_back(IKEFBXModel::Bone(boneName));
		IKEFBXModel::Bone& bone = bones.back();
		//自作ボーンとFBXのボーンを紐づける
		bone.fbxCluster = fbxCluster;
		//FBXから初期姿勢行列を取得する
		FbxAMatrix fbxMat;
		fbxCluster->GetTransformLinkMatrix(fbxMat);

		//XMMatrix型に変換する
		XMMATRIX initialPose;
		ConvertMatrixFromFbx(&initialPose, fbxMat);

		//初期姿勢逆行列を得る
		bone.invInitialPose = XMMatrixInverse(nullptr, initialPose);
	}

	//ボーン番号とスキニングウェイトのペア
	struct WeightSet
	{
		UINT index;
		float weight;
	};

	int verticesMax = fbxMesh->GetControlPointsCount();

	//二次元配列（ジャック配列）
	//list:頂点が影響を受けるボーンの全リスト
	//vector:それを全頂点分
	std::vector<std::list<WeightSet>> weightLists(verticesMax);

	//全てのボーンについて
	for (int i = 0; i < clusterCount; i++)
	{
		//FBXボーン情報
		FbxCluster* fbxCluster = fbxSkin->GetCluster(i);
		//このボーンに影響を受ける頂点の数
		int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();
		//このボーンに影響を受ける頂点の配列
		int* controlPointIndices = fbxCluster->GetControlPointIndices();
		double* controlPointWeights = fbxCluster->GetControlPointWeights();

		//影響を受ける全頂点について
		for (int j = 0; j < controlPointIndicesCount; j++)
		{
			//頂点番号
			int controlPointIndex = controlPointIndices[j];
			//スキンウェイト
			float weight = static_cast<float>(controlPointWeights[j]);
			//その頂点の影響を受けるボーンリストに、ボーンとウェイトのペアを追加
			weightLists[controlPointIndex].emplace_back(WeightSet{static_cast<UINT>(i), weight});
		}
	}

	//再び面の情報を取得する必要がある
	//面の数
	const int polygonCount = fbxMesh->GetPolygonCount();
	//カウント用
	int PolygonVertexCount = 0;
	//頂点座標データ数最大
	const int PolygonVertexCountMax = fbxMesh->GetPolygonVertexCount();
	//各頂点のコントロールポイント
	std::vector<int> vertexControlPoint(PolygonVertexCountMax);
	//配列で持つ
	for (int i = 0; i < polygonCount; i++)
	{
		//面を構成する頂点の数を取得 (3なら三角形ポリゴン)
		const int polygonSize = fbxMesh->GetPolygonSize(i);
		assert(polygonSize <= 4);

		//1頂点ずつ処理
		for (int j = 0; j < polygonSize; j++)
		{
			//FBX頂点配列のインデックス
			int index = fbxMesh->GetPolygonVertex(i, j); //ポリゴン番号(i)と、ポリゴン内の頂点インデックス(j)から、頂点番号を取得する
			assert(index >= 0);
			vertexControlPoint[PolygonVertexCount] = index;
			PolygonVertexCount++;
		}
	}

	//頂点配列書き換え用の参照
	auto& vertices = IKEFBXModel->vertices;

	//各頂点についての処理
	for (int i = 0; i < verticesMax; i++)
	{
		//頂点のウェイトから最も大きい4つを選択
		auto& weightList = weightLists[i];
		//大小比較用のラムダ式を指定して降順にソート
		weightList.sort(
			[](const auto& lhs, const auto& rhs)
			{
				//左の要素の方が大きければture　そうでなければfalseを返す
				return lhs.weight > rhs.weight;
			});

		int weightArrrayIndex = 0;
		//降順ソート済みのウェイトリストから
		for (auto& WeightSet : weightList)
		{
			for (int j = 0; j < PolygonVertexCountMax; j++)
			{
				//対応する頂点があれば
				if (vertexControlPoint[j] == i)
				{
					//頂点データに書き込み
					vertices[j].boneIndex[weightArrrayIndex] = WeightSet.index;
					vertices[j].boneWeight[weightArrrayIndex] = WeightSet.weight;
				}
			}
			//4つに達したら終了
			if (++weightArrrayIndex >= IKEFBXModel::MAX_BONE_INDICES)
			{
				float weight = 0.0f;
				//2番目以降のウェイトを合計
				for (int j = 1; j < IKEFBXModel::MAX_BONE_INDICES; j++)
				{
					weight += vertices[i].boneWeight[j];
				}
				//合計で1.0f（100%）になるように調整
				vertices[i].boneWeight[0] = 1.0f - weight;
				break;
			}
		}
	}
}

std::string IKEFbxLoader::ExtractFileName(const std::string& path)
{
	size_t pos1;
	//区切り文字　'\\'　が出てくる一番最後の部分を検索
	pos1 = path.rfind('\\');
	if (pos1 != string::npos)
	{
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}
	//区切り文字　'/'　が出てくる一番最後の部分を検索
	pos1 = path.rfind('/');
	if (pos1 != string::npos)
	{
		return path.substr(pos1 + 1, path.size() - pos1 - 1);
	}

	return path;
}
