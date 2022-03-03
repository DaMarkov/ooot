#include <string>
#include <vector>
#include <functional>



namespace tinyxml2
{
	class XMLElement;
}


union Vtx;
union Gfx;


namespace oot
{
	class ColladaAsset
	{
	public:
		ColladaAsset() = default;
		ColladaAsset(const char* Filename);

		Gfx* CreateDisplayList();
		int AppendToDisplayList(Gfx* DisplayList);

		operator bool () const { return m_Valid; }

		static std::string FindAttribute(   tinyxml2::XMLElement* Element, const char* Attribute);
		static int32_t     FindAttributeInt(tinyxml2::XMLElement* Element, const char* Attribute);

		static void ForAllChilden(tinyxml2::XMLElement* Element, const char* ChildName, std::function<void(tinyxml2::XMLElement*)> onElement);

	//private:
		class Mesh
		{
		public:
			Mesh(tinyxml2::XMLElement* Mesh);
			Mesh(Mesh&) = delete;
			Mesh(const Mesh&) = delete;
			Mesh(Mesh&& src) noexcept : m_Data(std::move(src.m_Data))
			{
				m_pVertices   = src.m_pVertices;
				m_VerticesNum = src.m_VerticesNum;
				m_pIndices    = src.m_pIndices;
				m_IndicesNum  = src.m_IndicesNum;

				src.m_pVertices = nullptr;
				src.m_pIndices  = nullptr;

				m_Valid = src.m_Valid;
				src.m_Valid = false;
			}

			~Mesh()
			{
				delete[] m_pVertices;
				delete[] m_pIndices;
				m_Valid = false;
			}

			const Vtx* GetVB()     const { return m_pVertices; }
			uint32_t   GetVBSize() const { return m_VerticesNum; }

			const int* GetIB()     const { return m_pIndices; }
			uint32_t   GetIBSize() const { return m_IndicesNum; }

			operator bool () const { return m_Valid; }

		private:
			class FloatArray
			{
				friend class Mesh;

			public:
				enum class Type
				{
					None, Position, Normal, TexCoord
				};

				FloatArray(tinyxml2::XMLElement* Source);
				FloatArray(FloatArray&) = delete;
				FloatArray(const FloatArray&) = delete;
				FloatArray(FloatArray&& src) noexcept : m_ID(std::move(src.m_ID))
				{
					m_Count = src.m_Count;
					pData   = src.pData;
					m_Valid = src.m_Valid;
					src.m_Count = 0;
					src.pData   = nullptr;
					src.m_Valid = false;
				}
				~FloatArray();

				auto GetID()     const { return m_ID; }
				auto GetCount()  const { return m_Count; }
				Type GetType()   const { return m_Type; }
				operator bool () const { return m_Valid; }

			private:
				void SetType(Type NewType) { m_Type = NewType; }
				const float* GetData() const { return pData; }

				std::string m_ID;
				Type m_Type = Type::None;

				uint32_t m_Count = 0;
				float* pData = nullptr;
				bool m_Valid = false;
			};

			FloatArray* FindType(FloatArray::Type Type);
			int AddVertex(int PositionIndex, int NormalIndex = -1, int TexcoordIndex = -1);

			std::vector<FloatArray> m_Data;

			Vtx* m_pVertices = nullptr;
			uint32_t m_VerticesNum = 0;

			int* m_pIndices = nullptr;
			uint32_t m_IndicesNum = 0;

			bool m_Valid = false;
		};

		std::vector<Mesh> m_Meshes;

		bool m_Valid = false;
	};
}