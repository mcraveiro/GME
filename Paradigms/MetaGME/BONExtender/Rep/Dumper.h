#ifndef DUMPER_H
#define DUMPER_H

#include "list"
#include "vector"
#include "Sheet.h"

class Dumper : public Sheet {
public:
	static const char* m_strBonExtenderOptions;
	static const char* Yes_Str;
	static const char* Only_Str;
	static const char* No_Str;
	static const char* m_strGenVisitor;
	static const char* m_strVisitorSignature;
	static const char* m_strGenInitMethod;
	static const char* m_strGenFiniMethod;
	static const char* m_strGenAcceptWithTraversal;
	static const char* m_strGenAcceptSpecialized;
	static const char* m_strSpecAcceptRetVal;
	static const char* m_strVCVersion6;
	static const char* m_strTemplateGetter;
	static const char* m_strOutputInSeparateFile;
	static const char* m_strTgtNamespace;
protected:
	Dumper();

public:
	static Sheet * getInstance();
	virtual ~Dumper();

	bool build();

	static int selectOutputFiles( const std::string& proj_name, BON::RegistryNode& optionsRegnode);
	static int backup( const std::string& file_name);
	static void getGlobalUserParts( std::string& up1, std::string& up2);

	static std::string xmlFilter( const std::string& in);

	static bool m_bParsePrev;
	static bool m_bGenInit;
	static bool m_bGenFinalize;
	static bool m_bGenAcceptTrave;
	static bool m_bGenAcceptSpeci;
	static bool m_bGenTemplates;
	static bool m_bGenTemplVersion6;
	static bool m_bGenRegular;
	static int  m_iVisitSign;
	static int  m_iSpecAcceptRetVal;
	static bool m_bGenVisitor;
	static bool m_bSafetyBackup;

protected:

	void inheritMoReSeCoFolAsp();

	void showMultipleInheritance();

	void selConf( std::vector<FCO*>& s);

	void createMethods();
	void dumpGlobals( std::vector<FCO*>& s);
	void dumpGlobalMOF();
	void dumpMain( std::vector<FCO*>& s);
	void doDump();
	void initOutFiles( std::vector<FCO*>& s, std::string& resu);
	void finiOutFiles( std::vector<FCO*>& s);

	bool checkAll();
	bool checkAllFCOs();
	bool checkOrphanAttributes();
	bool findInFolders( FCO * fco);

	std::string getNamespaceName();

protected:
	void fetchConfigurationNames( std::vector< std::string>& res);

};


#endif //DUMPER_H
