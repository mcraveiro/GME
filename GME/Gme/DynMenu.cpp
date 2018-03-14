#include "stdafx.h"
#include "GMEStd.h"
#include "DynMenu.h"

////////////////////////////// CDynMenu /////////////////////////////

CDynMenu::CDynMenu(int iden, TCHAR* nm) : id(iden), name(nm)
{
	menu.CreatePopupMenu();
	minID = maxID = -1;
}

CDynMenu::~CDynMenu()
{
}

void CDynMenu::AddItem(int id, CString&& roleName, CString&& displayName, CString&& helpMsg)
{
	if(minID < 0)
		minID = id;
	maxID = id;
	std::unique_ptr<CDynMenuItem> item(new CDynMenuItem(id, std::move(roleName), std::move(displayName), std::move(helpMsg)));
	items.push_back(std::move(item));
}

void CDynMenu::Sort()
{
	std::sort(items.begin(), items.end(),
		[](const std::unique_ptr<CDynMenuItem>& a, const std::unique_ptr<CDynMenuItem>& b)
	{ return a->GetLabel() < b->GetLabel(); }
	);
	for (auto it = items.begin(); it != items.end(); it++)
	{
		menu.AppendMenu(MF_ENABLED | MF_UNCHECKED | MF_STRING, (**it).id, (**it).displayName);
	}
}

CDynMenuItem* CDynMenu::FindItem(int id)
{
	for (auto it = items.begin(); it != items.end(); it++) {
		CDynMenuItem* item = it->get();
		if(item->id == id)
			return item;
	}
	return 0;
}
