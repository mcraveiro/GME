#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"

#include "ConnectionRep.h"
#include "logger.h"
#include "ModelRep.h"
#include "CodeGen.h"

#include "globals.h"
extern Globals global_vars;


ConnectionRep::ConnectionRep( BON::FCO& ptr, BON::FCO& resp_ptr)
	: FCO( ptr, resp_ptr)
{ 
	m_jointList.clear();
}


ConnectionRep::~ConnectionRep()
{
	m_jointList.clear();
}


void ConnectionRep::addJoint( ConnJoint & joint)
{
	m_jointList.push_back( joint);
}


void ConnectionRep::appendJointElements( const ConnJoint & old_joint)
{
	ConnJoint joint( old_joint); // create a copy of the old joint
	joint.setConnectionPtr( this); // set the new container connection

	if (m_jointList.empty())
		m_jointList.push_back( joint);
	else // with some constraint, but still copying
		m_jointList.push_back( joint);
}


void ConnectionRep::inherit()
{
	ModelRepPtrList models = this->modelsIAmPartOfFinal();
	
	// interface inheritance
	ModelRepPtrList_Iterator mod_it = models.begin();
	// for all models i am part of
	for( ; mod_it != models.end(); ++mod_it )
	{
		ModelRep* mod_ptr = *mod_it;

		// for all connJoints it has
		std::list<ConnJoint>::iterator joint_it = m_jointList.begin();
		for( ; joint_it != m_jointList.end(); ++joint_it )
		{
			joint_it->intInherit( mod_ptr); // creates map containing src and dst roles
		}
	}

	// implementation inheritance
	// not needed since the inheritance provides the needed functionality
}


std::string ConnectionRep::doDump()
{
	std::string h_file, c_file;

	dumpPre( h_file, c_file);

	dumpFCO( h_file, c_file);

	if ( !m_connMethods.empty())
		h_file += CodeGen::indent(1) + "//\n" + CodeGen::indent(1) + "// connectionEnd getters\n";

	MethodLexicographicSort lex;
	std::sort( m_connMethods.begin(), m_connMethods.end(), lex);

	std::vector<Method>::iterator i = m_connMethods.begin();
	for( ; i != m_connMethods.end(); ++i)
	{
		h_file += i->getHeader() + "\n";
		c_file += i->getSource() + "";
	}

	h_file += hideAndExpose();

	dumpPost( h_file, c_file);

	sendOutH( h_file);//DMP_H( h_file);
	sendOutS( c_file);//DMP_S( c_file);

	return "";
}


bool ConnectionRep::checkConnectionTargets()
{
	bool res = true;
	std::list<ConnJoint>::iterator joint_it = m_jointList.begin();
	for( ; joint_it != m_jointList.end(); ++joint_it )
	{
		res = res && joint_it->checkElements( getName());
	}
	return res;
}


bool ConnectionRep::calcLCD()
{
	std::list<ConnJoint>::iterator joint_it = m_jointList.begin();
	for( ; joint_it != m_jointList.end(); ++joint_it )
		joint_it->calcLCD();

	return true;
}

bool ConnectionRep::createEndGetters()
{
	std::list<ConnJoint>::iterator joint_it = m_jointList.begin();
	for( ; joint_it != m_jointList.end(); ++joint_it )
	{
		joint_it->createLinkGetters();
		joint_it->createSrcDstGetters();
		joint_it->createEndGetters();
	}
	return true;
}

