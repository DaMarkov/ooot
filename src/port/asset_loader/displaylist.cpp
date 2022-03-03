#include "displaylist.h"



void oot::DisplayList::operator << (const Gfx& Command)
{
	m_DisplayList.emplace_back(Command);
}