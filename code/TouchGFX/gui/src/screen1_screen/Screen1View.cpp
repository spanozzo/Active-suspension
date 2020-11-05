#include <gui/screen1_screen/Screen1View.hpp>
#include <touchgfx/Color.hpp>
#include <texts/TextKeysAndLanguages.hpp>

Screen1View::Screen1View()
{
    counter = 0;
}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::handleTickEvent()
{}

void Screen1View::setXYZ(float gyroData[3]) {
    ++counter;
    if(counter == 4) {
        Unicode::snprintf(textArea1Buffer, TEXTAREA1_SIZE, "%d", int(gyroData[1]));
        Unicode::snprintf(textArea2Buffer, TEXTAREA2_SIZE, "%d", int(gyroData[0]));
        Unicode::snprintf(textArea3Buffer, TEXTAREA3_SIZE, "%d", int(gyroData[2]));
        textArea1.invalidate();
        textArea2.invalidate();
        textArea3.invalidate();
        counter = 0;
    }
}

void Screen1View::setAxes(float gyroData[3]) {
    if(changeMode) {
        gyroData[0] = -gyroData[0];
        gyroData[1] = -gyroData[1];
    }
    // fondo scala a -180/+180°
    if(int(gyroData[0]) < -180) {
        gyroData[0] = -180;
    }
    else if(gyroData[0] > 180) {
        gyroData[0] = 180;
    }
    if(int(gyroData[1]) < -180) {
        gyroData[1] = -180;
    }
    else if(gyroData[1] > 180) {
        gyroData[1] = 180;
    }

    int oldRange = 180 - (-180);
    int xNewRange = 200 - 10;
    int yNewRange = 230 - 40;

    int x = int((((gyroData[1] - (-180)) * xNewRange) / oldRange) + 10);
    int y = int((((gyroData[0] - (-180)) * yNewRange) / oldRange) + 40);
    
    circle1.moveTo(x, 20);
    circle2.moveTo(210, y);

    circle1.invalidate();
    circle2.invalidate();
}

void Screen1View::setSuspensions(float suspensionData[4]) {
    touchgfx::colortype color[] = {touchgfx::Color::getColorFrom24BitRGB(0, 255, 22), 
                                        touchgfx::Color::getColorFrom24BitRGB(0, 255, 22),
                                        touchgfx::Color::getColorFrom24BitRGB(0, 255, 22),
                                        touchgfx::Color::getColorFrom24BitRGB(0, 255, 22)};

    for(int i=0; i<4; i++) {
        if(suspensionData[i] > 999) {
            color[i] = touchgfx::Color::getColorFrom24BitRGB(255, 210, 0);
            if(suspensionData[i] > 1999) {
                color[i] = touchgfx::Color::getColorFrom24BitRGB(242, 10, 10);
            }
        }
        else {
            if(suspensionData[i] < -999) {
                color[i] = touchgfx::Color::getColorFrom24BitRGB(9, 64, 240);
            }
        }
    }

    box1.setColor(color[0]);
    box2.setColor(color[1]);
    box3.setColor(color[2]);
    box4.setColor(color[3]);
    
    box1.invalidate();
    box2.invalidate();
    box3.invalidate();
    box4.invalidate();
}

void Screen1View::setChangeMode() {
    changeMode = !changeMode;
}
