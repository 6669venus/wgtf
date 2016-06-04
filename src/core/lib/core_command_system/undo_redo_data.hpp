#ifndef UNDO_REDO_DATA_HPP
#define UNDO_REDO_DATA_HPP

class UndoRedoData
{
public:
	virtual ~UndoRedoData() {};

	virtual void undo() = 0;
	virtual void redo() = 0;
};

#endif // UNDO_REDO_DATA_HPP