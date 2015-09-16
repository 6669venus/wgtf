#ifndef PYOBJECT_POINTER_HPP
#define PYOBJECT_POINTER_HPP

#include "cstdmf/smartpointer.hpp"


BW_BEGIN_NAMESPACE

typedef SmartPointer<PyObject> PyObjectPtr;

// -----------------------------------------------------------------------------
// Section: SmartPointer glue
// -----------------------------------------------------------------------------


/**
 *	This template specialisation allows us to have a SmartPointer to
 *	ordinary an PyObject (PyObjectPlus works fine without these)
 */
template <>
inline void incrementReferenceCount( const PyObject & Q )
{
	Py_INCREF( const_cast<PyObject*>( &Q ) );	// Q guaranteed non-null
}

/**
 *	This template specialisation allows us to have a SmartPointer to
 *	ordinary an PyObject (PyObjectPlus works fine without these)
 */
template <>
inline void decrementReferenceCount( const PyObject & Q )
{
	Py_DECREF( const_cast<PyObject*>( &Q ) );	// Q guaranteed non-null
}

/**
 *	This template specialisation allows us to have a SmartPointer to
 *	an ordinary PyObject (PyObjectPlus works fine without these)
 */
template <>
inline bool hasZeroReferenceCount( const PyObject & Q )
{
	return (const_cast<PyObject*>( &Q )->ob_refcnt == 0);	// Q guaranteed non-null
}

/**
 *	This template specialisation allows us to have a SmartPointer to
 *	an ordinary PyObject (PyObjectPlus works fine without these)
 */
template <>
inline int32 incrementReferenceCountIfNotZero( const PyObject &Q )
{
	//The BWEntity project requires that this function is defined but
	// it should never be called. CentOS5 does not have Py_REFCNT
	// see incRefIfNotZero in smartpointer.hpp for non-specialisation
	MF_ASSERT( false );
	return 0;
}

BW_END_NAMESPACE

#endif // PYOBJECT_POINTER_HPP
