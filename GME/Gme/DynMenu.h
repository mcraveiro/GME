#ifndef GME_DYNMENU_H
#define GME_DYNMENU_H

class CDynMenuItem {
	friend class CDynMenu;
public:
	CDynMenuItem(int iden, CString&& lab, CString&& dName, CString&& hmsg) : id(iden), label(std::move(lab)), displayName(std::move(dName)), helpMsg(std::move(hmsg)) {}
protected:
	int id;
	CString label;		// contains RoleNames actually
	CString helpMsg;
	CString displayName;
public:
	CString& GetLabel()		{ return label; }
	CString& GetHelpMsg()	{ return helpMsg; }
};

class CDynMenu {
public:
	CDynMenu(int iden, TCHAR *nm);
	~CDynMenu();
protected:
	int id;
	int minID, maxID;
	CString name;
	std::vector<std::unique_ptr<CDynMenuItem> > items;
	CMenu menu;
public:
	void SetName(CString nm)	{ name = nm; }
	void SetID(int i)			{ id = i; }
	CMenu& GetMenu()			{ return menu; }
	int GetMinID() const		{ return minID; }
	int GetMaxID() const		{ return maxID; }
	bool IsEmpty() const		{ return items.size() == 0; }
	int GetCount() const		{ return items.size(); }

	void AddItem(int id, CString&& roleName, CString&& displayName, CString&& helpMsg);
	CDynMenuItem* FindItem(int id);
	void Sort();
};

#endif // whole file

