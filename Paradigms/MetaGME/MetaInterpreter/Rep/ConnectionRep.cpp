#include "stdafx.h"

#include "BON.h"
#include "BONImpl.h"

#include "ConnectionRep.h"
#include "logger.h"
#include "ModelRep.h"

#include "globals.h"
extern Globals global_vars;

/*static*/ const std::string ConnectionRep::Color_str = "Color";
/*static*/ const std::string ConnectionRep::ConnLineEnd_str = "ConnLineEnd";
/*static*/ const std::string ConnectionRep::ConnLineStart_str = "ConnLineStart";
/*static*/ const std::string ConnectionRep::ConnLineType_str = "ConnLineType";
/*static*/ const std::string ConnectionRep::LabelFormatStr_str = "LabelFormatStr";
/*static*/ const std::string ConnectionRep::SrcAttrLabel1_str = "SrcAttrLabel1";
/*static*/ const std::string ConnectionRep::SrcAttrLabel2_str = "SrcAttrLabel2";
/*static*/ const std::string ConnectionRep::DstAttrLabel1_str = "DstAttrLabel1";
/*static*/ const std::string ConnectionRep::DstAttrLabel2_str = "DstAttrLabel2";

ConnectionRep::ConnectionRep( BON::FCO& ptr, BON::FCO& resp_ptr)
	: FCO( ptr, resp_ptr)
{ 
	m_jointList.clear();
}


ConnectionRep::~ConnectionRep()
{
	m_jointList.clear();
}


/*virtual*/ void ConnectionRep::initAttributes()
{
	//
	// Connections have their namePos attribute ignored! <!>
	//

	m_isInRootFolder = m_isInRootFolder || m_ptr->getAttribute( InRootFolder_str)->getBooleanValue();

	// isAbstract: true iff all values (which are set) are true
	//m_isAbstract = m_isAbstract && m_ptr->getAttribute( IsAbstract_str)->getBooleanValue();
	bool isabs = true; // if any of the set values is false it will change
	bool isabs_set = false;
	if( m_ptr->getAttribute( IsAbstract_str)->getStatus() >= BON::AS_Here)
	{
		isabs = isabs && m_ptr->getAttribute( IsAbstract_str)->getBooleanValue();
		isabs_set = true;
	}


	// general pref
	bool isgenpref_set = false;
	if( m_ptr->getAttribute( GeneralPreferences_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrGenPref = m_ptr->getAttribute( GeneralPreferences_str)->getStringValue();
		isgenpref_set = true;
	}

	// displayed name
	bool isdispname_set = false;
	if( m_ptr->getAttribute( DisplayedName_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrDispName = m_ptr->getAttribute( DisplayedName_str)->getStringValue();
		isdispname_set = true;
	}

	
	bool iscolor_set = false;
	//if( m_ptr->getAttribute( Color_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrColor = m_ptr->getAttribute( Color_str)->getStringValue();
		iscolor_set = true;
	}
	bool isconnlineend_set = false;
	//if( m_ptr->getAttribute( ConnLineEnd_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrConnLineEnd = m_ptr->getAttribute( ConnLineEnd_str)->getStringValue();
		isconnlineend_set = true;
	}
	bool isconnlinestart_set = false;
	//if( m_ptr->getAttribute( ConnLineStart_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrConnLineStart = m_ptr->getAttribute( ConnLineStart_str)->getStringValue();
		isconnlinestart_set = true;
	}
	
	bool isconnlinetype_set = false;
	//if( m_ptr->getAttribute( ConnLineType_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrConnLineType = m_ptr->getAttribute( ConnLineType_str)->getStringValue();
		isconnlinetype_set = true;
	}

	bool islabelformat_set = false;
	if( m_ptr->getAttribute( LabelFormatStr_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrLabelFormatStr = m_ptr->getAttribute( LabelFormatStr_str)->getStringValue();
		islabelformat_set = true;
	}

	bool issrclabel1_set = false;
	if( m_ptr->getAttribute( SrcAttrLabel1_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrSrcAttrLabel1 = m_ptr->getAttribute( SrcAttrLabel1_str)->getStringValue();
		issrclabel1_set = true;
	}
	bool issrclabel2_set = false;
	if( m_ptr->getAttribute( SrcAttrLabel2_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrSrcAttrLabel2 = m_ptr->getAttribute( SrcAttrLabel2_str)->getStringValue();
		issrclabel2_set = true;
	}
	bool isdstlabel1_set = false;
	if( m_ptr->getAttribute( DstAttrLabel1_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrDstAttrLabel1 = m_ptr->getAttribute( DstAttrLabel1_str)->getStringValue();
		isdstlabel1_set = true;
	}
	bool isdstlabel2_set = false;
	if( m_ptr->getAttribute( DstAttrLabel2_str)->getStatus() >= BON::AS_Here)
	{
		m_sAttrDstAttrLabel2	= m_ptr->getAttribute( DstAttrLabel2_str)->getStringValue();
		isdstlabel2_set = true;
	}

	std::set< BON::FCO >::const_iterator it = m_equivs.begin();
	for ( ; it != m_equivs.end(); ++it)
	{
		if ( *it == m_ptr) continue;

		// --the following 4 attributes are applicable to proxies as well--
		// InRootFolder: true if one is at least true
		m_isInRootFolder = m_isInRootFolder || (*it)->getAttribute( InRootFolder_str)->getBooleanValue();

		// isAbstract: true if all objects are true
		//m_isAbstract = m_isAbstract && (*it)->getAttribute( IsAbstract_str)->getBooleanValue();
		if( (*it)->getAttribute( IsAbstract_str)->getStatus() >= BON::AS_Here) // if set by the user
		{
			isabs = isabs && (*it)->getAttribute( IsAbstract_str)->getBooleanValue();
			isabs_set = true;
		}

#if(1)
		// general pref
		if( !isgenpref_set && (*it)->getAttribute( GeneralPreferences_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrGenPref = (*it)->getAttribute( GeneralPreferences_str)->getStringValue();
			isgenpref_set = true;
		}
		

		// displayed name
		if( !isdispname_set && (*it)->getAttribute( DisplayedName_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrDispName = (*it)->getAttribute( DisplayedName_str)->getStringValue();
			isdispname_set = true;
		}
		

		// --applicable to non proxies only--
		if ((*it)->getObjectMeta().name().find("Proxy") != std::string::npos) continue;

		if( !iscolor_set && (*it)->getAttribute( Color_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrColor = (*it)->getAttribute( Color_str)->getStringValue();
			iscolor_set = true;
		}

		if( !isconnlineend_set && (*it)->getAttribute( ConnLineEnd_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrConnLineEnd = (*it)->getAttribute( ConnLineEnd_str)->getStringValue();
			isconnlineend_set = true;
		}
		
		if( !isconnlinestart_set && (*it)->getAttribute( ConnLineStart_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrConnLineStart = (*it)->getAttribute( ConnLineStart_str)->getStringValue();
			isconnlinestart_set = true;
		}
		if( !isconnlinetype_set && (*it)->getAttribute( ConnLineType_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrConnLineType = (*it)->getAttribute( ConnLineType_str)->getStringValue();
			isconnlinetype_set = true;
		}

		if( !islabelformat_set && (*it)->getAttribute( LabelFormatStr_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrLabelFormatStr = (*it)->getAttribute( LabelFormatStr_str)->getStringValue();
			islabelformat_set = true;
		}

		if( !issrclabel1_set && (*it)->getAttribute( SrcAttrLabel1_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrSrcAttrLabel1 = (*it)->getAttribute( SrcAttrLabel1_str)->getStringValue();
			issrclabel1_set = true;
		}
		if( !issrclabel2_set && (*it)->getAttribute( SrcAttrLabel2_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrSrcAttrLabel2 = (*it)->getAttribute( SrcAttrLabel2_str)->getStringValue();
			issrclabel2_set = true;
		}
		if( !isdstlabel1_set && (*it)->getAttribute( DstAttrLabel1_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrDstAttrLabel1 = (*it)->getAttribute( DstAttrLabel1_str)->getStringValue();
			isdstlabel1_set = true;
		}
		if( !isdstlabel2_set && (*it)->getAttribute( DstAttrLabel2_str)->getStatus() >= BON::AS_Here)
		{
			m_sAttrDstAttrLabel2 = (*it)->getAttribute( DstAttrLabel2_str)->getStringValue();
			isdstlabel2_set = true;
		}
#endif
	}

	if( isabs_set) m_isAbstract = isabs;
}


void ConnectionRep::addJoint( ConnJoint & joint)
{
	m_jointList.push_back( joint);
}


void ConnectionRep::appendJointElements( const ConnJoint & joint_of_ancestor)
{
	ConnJoint a_joint( joint_of_ancestor); // create a copy of the joint
	a_joint.setConnectionPtr( this); // set the new container connection

	if (m_jointList.empty())
		m_jointList.push_back( a_joint);
	else // with some constraint, but still copying (in error case write a log - like a constraint)
	{
		m_jointList.push_back( a_joint);

		bool desc = false;

		std::list<ConnJoint>::iterator it = m_jointList.begin();
		for( ; !desc && it != m_jointList.end(); ++it)
			desc = desc || it->descendantsOf( a_joint);

		if ( !desc)
			global_vars.err << MSG_ERROR << "Design error at connection \"" << m_ptr << "\". Derived connection is valid only if defined between fcos derived from the fcos connected by the base connection.\n";
	}
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
	std::vector<FCO*> conn_descendants;
	this->getImpDescendants( conn_descendants);
	//conn_descendants.push_back( this); nn because the owner will be first filled

	std::vector<FCO*>::iterator conn_it = conn_descendants.begin();
	for( ; conn_it != conn_descendants.end(); ++conn_it)
	{
		std::list<ConnJoint>::iterator joint_it = m_jointList.begin();
		for( ; joint_it != m_jointList.end(); ++joint_it )
		{
			ConnectionRep * conn_rep = 0;
			if ( (*conn_it)->getMyKind() == Any::CONN)
				conn_rep = dynamic_cast<ConnectionRep *>( *conn_it);
			if (conn_rep)
				conn_rep->appendJointElements( *joint_it);
		}
	}
}


std::string ConnectionRep::dumpConnDetails()
{
	std::string mmm;
	//if ( this->m_ptr)
	{
		mmm = indStr() + "<regnode name = \"color\" value =\"" + (m_sAttrColor.empty()?"0x000000":m_sAttrColor) + "\"></regnode>\n";

		if( !m_sAttrConnLineEnd.empty())
			mmm += indStr() + "<regnode name = \"dstStyle\" value =\"" + m_sAttrConnLineEnd + "\"></regnode>\n";

		if( !m_sAttrConnLineStart.empty())
			mmm += indStr() + "<regnode name = \"srcStyle\" value =\"" + m_sAttrConnLineStart + "\"></regnode>\n";

		if( !m_sAttrConnLineType.empty())
			mmm += indStr() + "<regnode name = \"lineType\" value =\"" + m_sAttrConnLineType + "\"></regnode>\n";

		// TODO? resolve also labelFormatStr macros???
		if( !m_sAttrLabelFormatStr.empty())
			mmm += indStr() + "<regnode name = \"labelFormatStr\" value =\"" + m_sAttrLabelFormatStr + "\"></regnode>\n";

		std::string * which[] = 
		{
			&m_sAttrSrcAttrLabel1,
			&m_sAttrSrcAttrLabel2,
			&m_sAttrDstAttrLabel1,
			&m_sAttrDstAttrLabel2
		};

		std::string reg_label[] =
		{
			"srcLabel1",
			"srcLabel2",
			"dstLabel1",
			"dstLabel2"
		};

		for( int i = 0; i < 4 ; ++ i)
		{
			std::string &label = *which[i];
			if( !label.empty()) {
				// the label may or may not contain the % sign
				// but if we find an attribute with that name, we place the % sign arount it
				// if the attribute is not found it is considered to be an ordinary label
				if ( label.length() > 1 && label[0] == '%' && label[ label.length() - 1] == '%')
				{
					bool attr = findFinalAttributeBasedOnName( label.substr( 1, label.length() - 2));
					if( attr ) 
						mmm += indStr() + "<regnode name= \"" + reg_label[i] + "\" value=\"" + label + "\"/>\n";
					else
						global_vars.err << MSG_ERROR << "Attribute \"" << label.substr( 1, label.length() - 2) << "\" not found for connection \"" << m_ptr << "\". Wrong label specification.\n";
				}
				else
				{
					bool attr = findFinalAttributeBasedOnName( label);
					if( attr ) 
						mmm += indStr() + "<regnode name= \"" + reg_label[i] + "\" value=\"%" + label + "%\"/>\n";
					else
						mmm += indStr() + "<regnode name= \"" + reg_label[i] + "\" value=\"" + label + "\"/>\n";
						// previously: global_vars.err << "Attribute \"" << label << "\" not found for connection \"" << getName() << "\". Wrong label specification.\n";
				}
			}
		}
	}
	return mmm;
}


std::string ConnectionRep::doDump()
{
	std::string m_ref = askMetaRef();

	std::string mmm = indStr() + "<connection name = \"" + getName()  + "\" metaref = \"" + m_ref + "\"";

	mmm += dumpAttributeList();

	mmm +=" >\n";
	++ind;
	mmm += dumpDispName();
	++ind;
	mmm += dumpGeneralPref();
	mmm += dumpConnDetails();

	--ind;
	mmm += dumpConstraints();
	mmm += dumpAttributes();

	ModelRepPtrList models = this->modelsIAmPartOfFinal();

	std::list<ConnJoint>::iterator joint_it = m_jointList.begin();
	for( ; joint_it != m_jointList.end(); ++joint_it )
	{
		std::string nnn = joint_it->dumpElements( models);
		mmm += nnn;
	}
	--ind;
	mmm += indStr() + "</connection>\n";
	
	return mmm;
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


void ConnectionRep::createConstraints(Sheet * s)
{
	std::list<ConnJoint>::iterator joint_it = m_jointList.begin();
	for( ; joint_it != m_jointList.end(); ++joint_it )
		joint_it->createConstraints(s, getName());
}

