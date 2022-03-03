#include <vector>
#include "ultra64.h"



namespace oot
{
	class DisplayList
	{
	public:
		void operator << (const Gfx& Command);

		operator const Gfx* () const { return &m_DisplayList[0]; }

	private:		
		std::vector<Gfx> m_DisplayList;
	};
}