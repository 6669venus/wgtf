#ifndef FILEDIALOG_OPTIONS_HPP
#define FILEDIALOG_OPTIONS_HPP


struct FileDialogOptions
{
	bool			isOpenDialog;			//true = open dialog, false = save dialog

	bool			isDirectoryDialog;		//true = opening directoy, false = opening file
	const char *	startingDirectory;		//path to directory the dialog will start at. Null means the current working directory
	const char *	defaultFilename;		//if saving a file, the default name of the file. Can be null
	const char *	defaultSuffix;			//if saving a file, the default type the file will be. Can be null
	const char *	typeFilter;				//The filetypes the user can choose from
	uint32_t		flags;					//see FileDialogFlags
};


typedef std::vector<std::string> DialogStringList;
struct FileDialogOutput
{
	DialogStringList files;					//the list of file paths that were selected by the user
};


#endif //FILEDIALOG_OPTIONS_HPP