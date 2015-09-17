// py_script_sequence.ipp


/*
 *	This method gets an item from the sequence
 *	@param pos		The position to get the item from
 *	@param errorHandler The type of error handling to use if this method
 *		fails
 *	@return			The value at pos
 */
template <class ERROR_HANDLER>
inline ScriptObject ScriptSequence::getItem( Py_ssize_t pos,
	const ERROR_HANDLER & errorHandler ) const
{
	PyObject * pResult = PySequence_GetItem( this->get(), pos );
	errorHandler.checkPtrNoException( pResult );
	return ScriptObject( pResult, ScriptObject::FROM_NEW_REFERENCE );
}


/*
 *	This method sets an item in the sequence
 *	@param pos		The position to set the item at
 *	@param item		The item to store at pos
 *	@param errorHandler The type of error handling to use if this method
 *		fails
 *	@return			True on success, false otherwise
 */
template <class ERROR_HANDLER>
inline bool ScriptSequence::setItem( Py_ssize_t pos, const ScriptObject & item,
	const ERROR_HANDLER & errorHandler ) const
{
	int result = PySequence_SetItem( this->get(), pos, item.get() );
	errorHandler.checkMinusOne( result );
	return result == 0;
}


/*
 *	This method gets the size of the sequence
 *	@return			The size of the sequence
 */
inline ScriptSequence::size_type ScriptSequence::size() const
{
	// TODO: Handle -1
	return PySequence_Size( this->get() );
}


/**
 *	This method returns a new ScriptSequence which is a slice of this
 *	ScriptSequence including indexes [begin, end), as per the Python
 *	o[begin:end] syntax.
 *	@param begin The index of the first element of the slice
 *	@param end The index of the first element after the slice
 */
template <class ERROR_HANDLER>
ScriptSequence ScriptSequence::getSlice( Py_ssize_t begin, Py_ssize_t end,
	const ERROR_HANDLER & errorHandler ) const
{
	PyObject * pResult = PySequence_GetSlice( this->get(), begin, end );
	errorHandler.checkPtrError( pResult );
	return ScriptSequence( pResult, ScriptSequence::FROM_NEW_REFERENCE );
}

