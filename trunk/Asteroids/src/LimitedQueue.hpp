#pragma once

#include <deque>

#include "CTypes.hpp"

template< typename T >
class LimitedQueue
{
private: // classes

    typedef std::deque< T > MyContainer;
    typedef typename MyContainer::iterator iterator;
    typedef typename MyContainer::const_iterator const_iterator;

public: // methods

    iterator begin( void );
    iterator end( void );

    const_iterator begin( void ) const;
    const_iterator end( void ) const;

    void push_front( T const &t );
    T const &front( void ) const;

    void SetCapacity( u32 capaicty );
  
private: // members

    MyContainer m_container;
    u32 m_capacity;
};

template< typename T >
typename LimitedQueue< T >::iterator LimitedQueue<T>::begin( void )
{
    return m_container.begin();
}

template< typename T >
typename LimitedQueue< T >::iterator LimitedQueue<T>::end( void )
{
    return m_container.end();
}

template< typename T >
typename LimitedQueue< T >::const_iterator LimitedQueue<T>::begin( void ) const
{
    return m_container.begin();
}

template< typename T >
typename LimitedQueue< T >::const_iterator LimitedQueue<T>::end( void ) const
{
    return m_container.end();
}

template< typename T >
void LimitedQueue<T>::push_front( T const &t )
{
    if( m_capacity == m_container.size() )
        m_container.pop_back();

    m_container.push_front( t );
}

template< typename T >
T const & LimitedQueue<T>::front( void ) const
{
    return m_container.front();
}

template< typename T >
void LimitedQueue<T>::SetCapacity( u32 capacity )
{
    m_capacity = capacity;
}
