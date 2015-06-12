#ifndef I_EDITOR_APP_H
#define I_EDITOR_APP_H

namespace BW
{
class App;
}

class IEditorApp
{
public:
	virtual bool isMinimized();
	virtual void onIdle() {};
	virtual bool initDone() { return true; };
	virtual BW::App* mfApp() = 0;
};


#endif //EDITOR_APP_H