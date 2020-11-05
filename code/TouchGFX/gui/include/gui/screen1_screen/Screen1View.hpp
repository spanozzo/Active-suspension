#ifndef SCREEN1VIEW_HPP
#define SCREEN1VIEW_HPP

#include <gui_generated/screen1_screen/Screen1ViewBase.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

class Screen1View : public Screen1ViewBase
{
public:
    Screen1View();
    virtual ~Screen1View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    virtual void setXYZ(float gyroData[3]);
    virtual void setSuspensions(float suspensionData[4]);
    virtual void setAxes(float gyroData[3]);
    virtual void setChangeMode();
protected:
    int counter;
    bool changeMode;
};

#endif // SCREEN1VIEW_HPP
