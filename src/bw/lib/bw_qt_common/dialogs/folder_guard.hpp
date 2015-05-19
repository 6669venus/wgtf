#ifndef FOLDER_GUARD_HPP
#define FOLDER_GUARD_HPP

class FolderSetter
{
private:
	static const int MAX_PATH_SIZE = 8192;
	wchar_t envFolder_[ MAX_PATH_SIZE ];
	wchar_t curFolder_[ MAX_PATH_SIZE ];

public:
	FolderSetter();
	void enter();
	void leave();
};


class FolderGuard
{
	FolderSetter setter_;
public:
	FolderGuard();
	~FolderGuard();
};

#endif // FOLDER_GUARD_HPP
