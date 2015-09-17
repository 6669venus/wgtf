// py_script_list.ipp

/**
 *	This method creates a new list, of size len
 *	@return A new list of size len
 */
/* static */ inline ScriptList ScriptList::create( Py_ssize_t len )
{
	PyObject * pList = PyList_New( len );
	assert( pList );
	return ScriptList( pList, ScriptObject::FROM_NEW_REFERENCE );
}


/**
 *	This method appends an object onto the end of the list
 *	@param object	The object to append to the list
 */
inline bool ScriptList::append( const ScriptObject & object ) const
{
	int result = PyList_Append( this->get(), object.get() );
	assert( result != -1 );
	return result == 0;
}


/**
 *	This method gets an item at a position
 *	@param pos	The position to get the item from
 *	@return		The item at position pos
 */
inline ScriptObject ScriptList::getItem( ScriptList::size_type pos ) const
{
	assert( pos < PyList_GET_SIZE( this->get() ) );
	PyObject * pItem = PyList_GET_ITEM( this->get(), pos );
	return ScriptObject( pItem, ScriptObject::FROM_BORROWED_REFERENCE );
}


/**
 *	This method sets an item at a position
 *	@param pos		The position to set the item att
 *	@param item		The item to set a position pos
 *	@return			This is always true
 */
inline bool ScriptList::setItem( ScriptList::size_type pos, ScriptObject item ) const
{
	assert( pos < PyList_GET_SIZE( this->get() ) );
	// This steals a reference to the item, so must create a new ref for it
	PyList_SET_ITEM( this->get(), pos, item.newRef() );

	return true;
}


/**
 *	This method gets the size of the list
 *	@return		The size of the list
 */
inline ScriptList::size_type ScriptList::size() const
{
	return PyList_GET_SIZE( this->get() );
}


/**
 *	This method returns a new ScriptList which is a slice of this
 *	ScriptList including indexes [begin, end), as per the Python
 *	o[begin:end] syntax.
 *	@param begin The index of the first element of the slice
 *	@param end The index of the first element after the slice
 *	@note negative indexes are treated as 0
 */
template <class ERROR_HANDLER>
ScriptList ScriptList::getSlice( Py_ssize_t begin, Py_ssize_t end,
	const ERROR_HANDLER & errorHandler ) const
{
	PyObject * pResult = PyList_GetSlice( this->get(), begin, end );
	errorHandler.checkPtrError( pResult );
	return ScriptList( pResult, ScriptList::FROM_NEW_REFERENCE );
}


