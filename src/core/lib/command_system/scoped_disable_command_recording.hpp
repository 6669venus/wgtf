#ifndef SCOPED_DISABLE_COMMAND_RECORDING_HPP
#define SCOPED_DISABLE_COMMAND_RECORDING_HPP


/**
 *	Only use this class if you do not want to support undo/redo.
 *	
 *	Create an instance of this class in the scope of any reflected property
 *	that you want to set, but do not want to record history.
 */
class ScopedDisableCommandRecording
{
public:
	ScopedDisableCommandRecording();
	~ScopedDisableCommandRecording();

	static bool isIgnored();
};

#endif // SCOPED_DISABLE_COMMAND_RECORDING_HPP

