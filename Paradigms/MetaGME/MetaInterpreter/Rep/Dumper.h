#ifndef DUMPER_H
#define DUMPER_H

#include "list"
#include "vector"
#include "Sheet.h"
#include "functional"

class Dumper : public Sheet {

protected:
	Dumper();

public:
	static Sheet * getInstance();
	virtual ~Dumper();

	bool build();


	static std::string makeValidFileName( const std::string& pInFile);
	static int selectOutputFiles( const BON::Project&, const std::string& proj_name, const std::string& project_path);
	static int selectOptions( const BON::Project&);
	static void registerIt( const std::string& f_name);
	static bool doParseCardinality
		(const std::string& cardinality, 
		const std::string &target, 
		const std::string &contextDesc, 
		std::string &expr);
	
	static std::string xmlFilter( const std::string& in);

protected:

	void createConstraints();
	void inheritMoReSeCoFolAsp();
	void inheritAspConsAttr();

	void doAspectPartsMap();
	// creates elements from element hierarchies

	
	void doDump();
	bool sortPtrs();
	bool aspectOrderSel(); // aspect Order Selection
	bool am(); // aSPECT mAPPING
	bool checkUniqueAspectNames();
	bool checkAll();
	bool checkRootFolder() const;
	bool checkAllFCOs();
	bool checkOrphanAttributes();
	bool checkEmptyFolders() const;
	bool findInFolders( FCO * fco);
	void findMaxOfMetaRefs();

protected:

	void inheritConstraintsAndAttributes( FCO * ptr, FCO * c_ptr);
	std::vector<AspectRep *> findAspectBasedOnName(const std::string & name_to_find) const;
};

#endif //DUMPER_H
