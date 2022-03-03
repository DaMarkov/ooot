#include "collada_loader.h"
#include <../tinyxml2/tinyxml2.h>
#include "ultra64.h"
#include <iostream>
#include <string>
#include <sstream>



std::string oot::ColladaAsset::FindAttribute(tinyxml2::XMLElement* Element, const char* Attribute)
{
	//Parse all attributes
	for (auto attribute = Element->FirstAttribute(); attribute; attribute = attribute->Next())
	{
		if (strcmp(attribute->Name(), Attribute) == 0)
			return std::string(attribute->Value());
	}

	return std::string();
}



int32_t oot::ColladaAsset::FindAttributeInt(tinyxml2::XMLElement* Element, const char* Attribute)
{
	//Parse all attributes
	for (auto attribute = Element->FirstAttribute(); attribute; attribute = attribute->Next())
	{
		if (strcmp(attribute->Name(), Attribute) == 0)
			return attribute->IntValue();
	}

	return -1;
}



void oot::ColladaAsset::ForAllChilden(tinyxml2::XMLElement* Element,
									  const char* ChildName,
									  std::function<void(tinyxml2::XMLElement*)> onElement)
{
	if (!onElement)
		return;
	for (auto child = Element->FirstChildElement(ChildName); child; child = child->NextSiblingElement())
		onElement(child);
}



template <typename T>
void ParseValues(tinyxml2::XMLElement* Element, T* pData, int Max)
{
	auto string_of_data = std::string(Element->GetText());
	std::stringstream ss(string_of_data);

	std::string s;
	uint32_t i = 0;
	while (std::getline(ss, s, ' ') && i < Max)
	{
		std::stringstream f(s);
		f >> pData[i++];
	}
}



oot::ColladaAsset::Mesh::FloatArray::FloatArray(tinyxml2::XMLElement* Source)
{
	if (!Source)
		return;

	//Parse all attributes
	m_ID = FindAttribute(Source, "id");

	auto data = Source->FirstChildElement("float_array");

	if (!data)
		return;

	m_Count = FindAttributeInt(data, "count");

	//Read data
	if (m_Count <= 0)
		return;

	pData = new float[m_Count];
	bool error = false;

	ParseValues(data, pData, m_Count);

	/*auto string_of_floats = std::string(data->GetText());
	std::stringstream ss(string_of_floats); 

	std::string s;
	uint32_t i = 0;
	while (std::getline(ss, s, ' ') && i < m_Count)
	{
		std::stringstream f(s);
		f >> pData[i++];
	}*/

	/*for (uint32_t i = 0; i < m_Count; ++i)
	{
		//Read float value
		if (data->QueryFloatText(&pData[i]) != 0)
		{
			error = true;
			break;
		}
	}*/

	m_Valid = !error;
}



oot::ColladaAsset::Mesh::FloatArray::~FloatArray()
{
	delete[] pData;
	pData = nullptr;
	m_Count = 0;
	m_Valid = false;
}



oot::ColladaAsset::Mesh::Mesh(tinyxml2::XMLElement* Mesh)
{
	if (!Mesh)
		return;

	bool error = false;

	//For all sources
	for (auto source = Mesh->FirstChildElement("source"); source; source = source->NextSiblingElement())
	{
		FloatArray geometry_data(source);

		if (!geometry_data)
		{
			//error = true;
			continue;
		}

		m_Data.emplace_back(std::move(geometry_data));
	}

	//Read vertex data
	std::string position_id;
	std::string vertex_id;
	std::string normal_id;
	std::string texcoord_id;

	auto vertices = Mesh->FirstChildElement("vertices");
	if (!vertices)
		error = true;
	else
	{
		auto input = vertices->FirstChildElement("input");
		position_id = FindAttribute(input, "source");
		//TODO read other attributes
	}

	auto polylist = Mesh->FirstChildElement("polylist");
	if (!polylist)
	{
		error = true;
		return;
	}


	//Find references
		ForAllChilden(polylist, "input", [&](auto element) {
			auto semantic = FindAttribute(element, "semantic");

			if (semantic == "VERTEX")
				vertex_id = FindAttribute(element, "source");
			else if (semantic == "NORMAL")
				normal_id = FindAttribute(element, "source");
			else if (semantic == "TEXCOORD")
				texcoord_id = FindAttribute(element, "source");
		});


	//Part 3 - Start to build vertices

	position_id.erase(0, 1);//removes the first character
	normal_id.erase(0, 1);//removes the first character
	texcoord_id.erase(0, 1);//removes the first character

	//Search if we have these IDs in our dataset
	for (auto& data : m_Data)
	{
		if (data.GetID() == position_id)
			data.SetType(FloatArray::Type::Position);
		else if (data.GetID() == normal_id)
			data.SetType(FloatArray::Type::Normal);
		else if (data.GetID() == texcoord_id)
			data.SetType(FloatArray::Type::TexCoord);
	}

	//TODO: in debug build if we have unassigned data and report this as an error

	//Reserve space for vertices, we need to read the indices first
	m_VerticesNum = 0;
	m_pVertices = new Vtx[FindType(FloatArray::Type::Normal)->GetCount() * 30];//TODO

	auto position_data = FindType(FloatArray::Type::Position)->GetData();
	auto normal_data   = FindType(FloatArray::Type::Normal)->GetData();
	auto texcoord_data = FindType(FloatArray::Type::TexCoord)->GetData();


	//Part 3 - Read indices
	auto vcount = polylist->FirstChildElement("vcount");
	auto count  = FindAttributeInt(polylist, "count");

	int* vcounts = new int[count];
	ParseValues(vcount, vcounts, count);

	int indices_loaded_num = 0;
	for (int i = 0; i < count; i++)
		indices_loaded_num += vcounts[i];

	indices_loaded_num *= 3;//For position, normal, texcoord

	int* indices_loaded = new int[indices_loaded_num];

	auto indices = polylist->FirstChildElement("p");
	ParseValues(indices, indices_loaded, indices_loaded_num);


	m_IndicesNum = 0;
	m_pIndices = new int[count*30];//TODO FIX

#define VTX(x,y,z,s,t,crnx,cgny,cbnz,a) { { { x, y, z }, 0, { s, t }, { crnx, cgny, cbnz, a } } }
		
	int k = 0;
	int l = 0;
	for (uint32_t i = 0; i < count; ++i)//For every polygon
	{
		struct VertexIndices//This is not what you think it is
		{
			int position_index;
			int normal_index;
			int texcoord_index;
		};

		std::vector<VertexIndices> polygon;

		for (uint32_t j = 0; j < vcounts[i]; ++j)//For every vertex of that polygon
		{
			VertexIndices vi;
			vi.position_index = indices_loaded[l++];
			vi.normal_index   = indices_loaded[l++];
			vi.texcoord_index = indices_loaded[l++];

			polygon.push_back(vi);
		}

		if (polygon.size() <= 2)
			continue;

		/*while (polygon.size() > 3)
		{//Check if the polygon is degenerate and try to fix it
			bool could_remove = false;

			for (int n = 0; n < polygon.size(); n++)
			{
				for (int m = n + 1; m < polygon.size(); m++)
				{
					if (polygon[n] == polygon[m])
					{
						polygon.erase(polygon.begin() + m);
						could_remove = true;
						break;
					}
				}

				if (could_remove)
					break;
			}

			if (!could_remove)
				break;
		}*/

		if (polygon.size() == 3)
		{
			for (const auto& vi : polygon)
			{
				//Generate vertex data
				int16_t x = (int16_t)(position_data[vi.position_index*3 + 0] * 1000.0f);
				int16_t y = (int16_t)(position_data[vi.position_index*3 + 1] * 1000.0f);
				int16_t z = (int16_t)(position_data[vi.position_index*3 + 2] * 1000.0f);

				int16_t tu = (int16_t)((texcoord_data[vi.texcoord_index*2 + 0] + 1.0f) * 10000.0f);
				int16_t tv = (int16_t)((texcoord_data[vi.texcoord_index*2 + 1] + 1.0f) * 10000.0f);

				m_pVertices[m_VerticesNum] = VTX(x, y, z,
					tu, tv,
					(uint8_t)(rand()%230), (uint8_t)(rand()%50), (uint8_t)(rand()%100), 255);

				m_pIndices[m_IndicesNum] = m_VerticesNum;//Yes this is stupid

				m_VerticesNum++;
				m_IndicesNum++;
			}
		}

		else if (polygon.size() == 4)
		{
			const int vertex_offset = m_VerticesNum;

			for (const auto& vi : polygon)
			{
				//Generate vertex data
				int16_t x = (int16_t)(position_data[vi.position_index * 3 + 0] * 1000.0f);
				int16_t y = (int16_t)(position_data[vi.position_index * 3 + 1] * 1000.0f);
				int16_t z = (int16_t)(position_data[vi.position_index * 3 + 2] * 1000.0f);

				int16_t tu = (int16_t)((texcoord_data[vi.texcoord_index*2 + 0] + 1.0f) * 10000.0f);
				int16_t tv = (int16_t)((texcoord_data[vi.texcoord_index*2 + 1] + 1.0f) * 10000.0f);

				m_pVertices[m_VerticesNum] = VTX(x, y, z,
					tu, tv,
					(uint8_t)(rand()%230), (uint8_t)(rand()%50), (uint8_t)(rand()%100), 255);
					//230, 50, 100, 255);

				m_VerticesNum++;
			}


			m_pIndices[m_IndicesNum++] = vertex_offset + 0;
			m_pIndices[m_IndicesNum++] = vertex_offset + 1;
			m_pIndices[m_IndicesNum++] = vertex_offset + 2;

			m_pIndices[m_IndicesNum++] = vertex_offset + 1;
			m_pIndices[m_IndicesNum++] = vertex_offset + 2;
			m_pIndices[m_IndicesNum++] = vertex_offset + 3;

			m_pIndices[m_IndicesNum++] = vertex_offset + 2;
			m_pIndices[m_IndicesNum++] = vertex_offset + 3;
			m_pIndices[m_IndicesNum++] = vertex_offset + 0;
		}

		else
		{
			//TODO triangulate
		}
	}

	delete[] indices_loaded;
	delete[] vcounts;




	for (int i = 0; i < m_VerticesNum; ++i)
	{
		int16_t x = (int16_t)(position_data[i*3 + 0] * 1000.0f);
		int16_t y = (int16_t)(position_data[i*3 + 1] * 1000.0f);
		int16_t z = (int16_t)(position_data[i*3 + 2] * 1000.0f);

		//int16_t z = (int16_t)(texcoord_data[i*2 + 2] * 1000.0f);

		//m_pVertices[i] = VTX(x, y, z,
							//(int16_t)rand(), (int16_t)rand(),
							//255, 255, 255, 255);
	}

	//m_pVertices[0] = VTX(-5, 22, 11, 0, 0, 37, 85, 76, 255);
	//m_pVertices[1] = VTX(5, 11, 5, 0, 0, 191, 29, 160, 255);
	//m_pVertices[2] = VTX(-5, 22, 5, 0, 0, 25, 57, 154, 255);

	m_Valid = !error;
}



oot::ColladaAsset::Mesh::FloatArray* oot::ColladaAsset::Mesh::FindType(FloatArray::Type Type)
{
	for (auto& data_array : m_Data)
	{
		if (data_array.GetType() == Type)
			return &data_array;
	}
	return nullptr;
}



oot::ColladaAsset::ColladaAsset(const char* Filename)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile(Filename);

	if (doc.ErrorID())
		return;

	auto collada = doc.FirstChildElement("COLLADA");
	if (!collada)
		return;

	auto geometries = collada->FirstChildElement("library_geometries");
	if (!geometries)
		return;

	bool error = false;

	for (auto geometry = geometries->FirstChildElement("geometry"); geometry; geometry = geometry->NextSiblingElement())\
	{
		//For all meshes
		for (auto mesh = geometry->FirstChildElement("mesh"); mesh; mesh = mesh->NextSiblingElement())
		{
			Mesh mesh_data(mesh);

			if (!mesh_data)
			{
				error = true;
				continue;
			}

			m_Meshes.emplace_back(std::move(mesh_data));
		}
	}

	m_Valid = !error;//File is good if no error occured
}



Gfx* oot::ColladaAsset::CreateDisplayList()
{
	static Gfx* display_list = nullptr;

	if (!display_list)
		display_list = new Gfx[1000];

	int count = 0;
	display_list[count++] = gsDPPipeSync();
	display_list[count++] = gsDPSetTextureLUT(G_TT_NONE);
	display_list[count++] = gsSPTexture(0x0ED8, 0x0ED8, 0, G_TX_RENDERTILE, G_ON);
	//display_list[count++] = gsDPLoadTextureBlock(NULL, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 2, 1);
	display_list[count++] = gsDPSetCombineLERP(TEXEL0, PRIMITIVE, ENV_ALPHA, TEXEL0, 0, 0, 0, 0, PRIMITIVE, ENVIRONMENT, COMBINED, ENVIRONMENT, 0, 0, 0, 1);
	display_list[count++] = gsDPSetRenderMode(G_RM_PASS, G_RM_AA_ZB_OPA_SURF2);
	display_list[count++] = gsSPClearGeometryMode(G_FOG);
	display_list[count++] = gsSPSetGeometryMode(G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR);
	display_list[count++] = gsDPSetPrimColor(0, 0, 255, 170, 255, 255);
	display_list[count++] = gsDPSetEnvColor(150, 0, 50, 128);
	display_list[count++] = gsSPVertex(m_Meshes[0].GetVB(), m_Meshes[0].GetVBSize(), 0);

	for (int i = 0; i < m_Meshes[0].GetIBSize(); i+=3)
		display_list[count++] = gsSP1Triangle(m_Meshes[0].GetIB()[i], m_Meshes[0].GetIB()[i+1], m_Meshes[0].GetIB()[i+2], 0);

	display_list[count++] = gsSPEndDisplayList();

	return display_list;
}



int oot::ColladaAsset::AppendToDisplayList(Gfx* DisplayList)
{
	int count = 0;
	for (const auto& mesh : m_Meshes)
	{
		//DisplayList[count++] = gsSPVertex(mesh.GetVB(), mesh.GetVBSize(), 0);
		DisplayList[count++] = gsSPVertex(mesh.GetVB(), 80, 0);

		for (int i = 0; i < mesh.GetIBSize(); i += 3)
		{
			int index1 = mesh.GetIB()[i+0];
			int index2 = mesh.GetIB()[i+1];
			int index3 = mesh.GetIB()[i+2];

			//find minimum
			int offset = index1;
			if (index2 < offset)
				offset = index2;
			if (index3 < offset)
				offset = index3;

			int index1_fixed = index1 - offset;
			int index2_fixed = index2 - offset;
			int index3_fixed = index3 - offset;

			if (index1_fixed >= 80 || index2_fixed >= 80 || index3_fixed >= 80)
				continue;

			DisplayList[count++] = gsSPVertex(&mesh.GetVB()[offset], 80, 0);
			DisplayList[count++] = gsSP1Triangle(index1_fixed, index2_fixed, index3_fixed, 0);
			//DisplayList[count++] = gsSP1Triangle(index1, index2, index3, 0);
		}


		/*for (int i = 0; i < mesh.GetIBSize(); i += 6)
		{
			int index1 = mesh.GetIB()[i + 0];
			int index2 = mesh.GetIB()[i + 1];
			int index3 = mesh.GetIB()[i + 2];
			int index4 = mesh.GetIB()[i + 3];
			int index5 = mesh.GetIB()[i + 4];
			int index6 = mesh.GetIB()[i + 5];

			DisplayList[count++] = gsSP2Triangles(index1, index2, index3, 1, index4, index5, index6, 1);
		}*/
	}

	DisplayList[count++] = gsSPEndDisplayList();
	return count;
}