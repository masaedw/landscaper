#ifndef _MESH_H_
#define _MESH_H_

#include "matrixobj.h"

namespace space
{

//メッシュ定義しすぎ
namespace mesh
{

	//RGB
	struct ColorRGB
	{
		double red,green,blue;
	};

	//RGBA
	struct ColorRGBA public ColorRGB
	{
		double alpha;
	};

	//インデックスつき
	struct IndexColor : public ColorRGBA
	{
		unsigned int index;
	};

	//テクスチャファイル名
	struct TextureFileName
	{
		std::string texturefilename;
	};

	//メッシュのうちの一つの面
	struct MeshFace
	{
		std::vector<unsigned int> facevertexindices;
	};

	//面法線
	struct MeshNormals
	{
		std::vector< Matrix31<double> > normals;
		std::vector< MeshFace > meshfaces;
	};

	//マテリアル
	struct Material
	{
		ColorRGBA faceColor;
		double power;
		ColorRGB specularColor;
		ColorRGB emissiveColor;
		TextureFileName texfile;
	};

	//テクスチャ座標
	struct MeshTextureCoords
	{
		std::vector< Matrix21<double> > texturecoords;
	};

	//マテリアルリスト
	struct MeshMaterialList
	{
		std::vector<Material> materials;
		std::vector<unsigned int> faceindexes;
	};

	//カラーリスト
	struct MeshVertexColors
	{
		std::vector<IndexColor> indexcolors;
	};

	//メッシュ
	struct Mesh
	{
		std::vector< Matrix31<double> > vertices;
		std::vector< MeshFace > faces;
		
		MeshTextureCoords texcoords;
		MeshNormals normals;
		MeshMaterialList materials;
		MeshVertexColors vertexcolors;

		Mesh(){}
	};

}//end of namespace mesh

}//end of namespace space

#endif
