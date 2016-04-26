#ifndef I_HISTORY_PANEL
#define I_HISTORY_PANEL

class IHistoryPanel
{
public:
    virtual void setClearButtonVisible (bool show) = 0;
    virtual void setMakeMacroButtonVisible(bool show) = 0;
};

#endif