#include "stdafx.h"
#include "DirSupplier.h"


// DirSupplier

DirSupplier::DirSupplier( bool p_hashed, int p_algorithm)
: m_hashed( p_hashed)
, m_algo( p_algorithm)
{
}

Dir256Iterator DirSupplier::begin256() const
{
	return * Dir256Iterator::createIteratorBeg();
}

Dir256Iterator DirSupplier::end256() const
{
	return * Dir256Iterator::createIteratorEnd();
}


Dir16Iterator DirSupplier::begin16() const
{
	return * Dir16Iterator::createIteratorBeg();
}

Dir16Iterator DirSupplier::end16() const
{
	return * Dir16Iterator::createIteratorEnd();
}

Dir256Iterator::Dir256Iterator( bool p_endIterator /*= false*/)
: m_i(0)
, m_j(0)
, m_algo( 0)
{
	if( p_endIterator)
	{
		m_i = m_j = m_max;
	}
}

//static 
Dir256Iterator* Dir256Iterator::createIteratorBeg()
{
	return new Dir256Iterator();
}

//static
Dir256Iterator* Dir256Iterator::createIteratorEnd()
{
	return new Dir256Iterator( true);
}

std::string Dir256Iterator::operator *() const
{
	static const char ans[] = "0123456789abcdef";
	char       lev[3]  = { 'z', 't', 0 };

	if( m_i > (int) sizeof( ans)/sizeof(char) || m_i < 0
		|| m_j > (int) sizeof( ans)/sizeof(char) || m_j < 0)
		return lev;

	lev[0] = ans[ m_i]; // form a name
	lev[1] = ans[ m_j];

	return lev;
}

Dir256Iterator& Dir256Iterator::operator++()
{
	if( ++m_j >= m_max)
	{
		m_j = 0;
		++m_i;
	}

	return *this;
}

Dir256Iterator::operator bool() const
{
	return m_i < m_max
		&& m_j < m_max;
}

bool Dir256Iterator::operator()() const
{
	return m_i < m_max
		&& m_j < m_max;
}

bool Dir256Iterator::operator !=( const Dir256Iterator& p_peer) const
{
	if( &p_peer == this)
		return false;

	bool me_valid = (*this);
	bool peer_valid = p_peer;

	if( me_valid && peer_valid) // examine details only if both are valid
	{
		if( m_i == p_peer.m_i
			&& m_j == p_peer.m_j)
		{
			return false;
		}
	}
	else if( !me_valid && !peer_valid) // both invalid
		return false;

	return true;
}

Dir16Iterator::Dir16Iterator( bool p_endIterator /*= false*/)
: m_i(0)
, m_algo( 0)
{
	if( p_endIterator)
	{
		m_i = m_max;
	}
}

//static 
Dir16Iterator* Dir16Iterator::createIteratorBeg()
{
	return new Dir16Iterator( );
}

//static
Dir16Iterator* Dir16Iterator::createIteratorEnd()
{
	return new Dir16Iterator( true);
}

std::string Dir16Iterator::operator *() const
{
	// we will create 16 dirs like 0, 1, 2, ..., f
	static const char ans[] = "0123456789abcdef";
	char       lev[2]  = { 'z', 0 };

	if( m_i > (int) sizeof( ans)/sizeof(char) || m_i < 0)
		return lev;

	lev[0] = ans[ m_i]; // form a name
	return lev;
}

Dir16Iterator& Dir16Iterator::operator++()
{
	++m_i;

	return *this;
}

Dir16Iterator::operator bool() const
{
	return m_i < m_max;
}

bool Dir16Iterator::operator()() const
{
	return m_i < m_max;
}

bool Dir16Iterator::operator !=( const Dir16Iterator& p_peer) const
{
	if( &p_peer == this)
		return false;

	bool me_valid = (*this);
	bool peer_valid = p_peer;

	if( me_valid && peer_valid) // examine details only if both are valid
	{
		if( m_i == p_peer.m_i)
			return false;
	}
	else if( !me_valid && !peer_valid) // both invalid
		return false;

	return true;
}
