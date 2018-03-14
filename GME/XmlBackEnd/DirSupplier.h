#pragma once

class Dir256Iterator;
class Dir16Iterator;

class DirSupplier
{
	bool    m_hashed;
	int     m_algo;
public:
	DirSupplier( bool p_hashed, int p_algorithm);
	Dir256Iterator begin256() const;
	Dir256Iterator end256() const;

	Dir16Iterator  begin16() const;
	Dir16Iterator  end16() const;
};

class Dir256Iterator
{
	static const int m_max = 16;
	int m_i;
	int m_j;
	int m_algo;
	Dir256Iterator( bool p_endIterator = false);
public:
	static Dir256Iterator*  createIteratorBeg();
	static Dir256Iterator*  createIteratorEnd();
	std::string             operator *( ) const;
	bool                    operator!=( const Dir256Iterator& p_peer) const;
	Dir256Iterator&         operator++( );
	
	                        operator bool( ) const;
	bool                    operator()   ( ) const;
};

class Dir16Iterator
{
	static const int m_max = 16;
	int m_i;
	int m_algo;
	Dir16Iterator( bool p_endIterator = false);
public:
	static Dir16Iterator*   createIteratorBeg();
	static Dir16Iterator*   createIteratorEnd();
	std::string             operator * ( ) const;
	bool                    operator !=( const Dir16Iterator& p_peer) const;
	Dir16Iterator&          operator ++( );
	
	                        operator bool( ) const;
	bool                    operator()   ( ) const;
};
