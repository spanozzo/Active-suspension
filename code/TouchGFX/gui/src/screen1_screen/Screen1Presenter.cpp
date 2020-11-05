#include <gui/screen1_screen/Screen1View.hpp>
#include <gui/screen1_screen/Screen1Presenter.hpp>

Screen1Presenter::Screen1Presenter(Screen1View& v)
    : view(v)
{

}

void Screen1Presenter::activate()
{

}

void Screen1Presenter::deactivate()
{

}

void Screen1Presenter::notifyXYZChanged(float gyroData[3]) {
    view.setXYZ(gyroData);
    view.setAxes(gyroData);
}

void Screen1Presenter::notifySuspensionChanged(float suspensionData[4]) {
    view.setSuspensions(suspensionData);
}

void Screen1Presenter::notifyChangeMode() {
    view.setChangeMode();
}
