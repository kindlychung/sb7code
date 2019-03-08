#include "sb7.h"

class my_application : public sb7::application {
   private:
    float color[3] = {1.f, .5f, .5f};

   public:
    my_application();
    void render(double currentTime);
};

my_application::my_application() {}

void my_application::render(double currentTime) {
    float red = ((float)sin(currentTime) + 1.f) / 2.f;
    color[0] = red;
    printf("Time: %f\n", currentTime);
    printf("Red: %f\n", red);
    glClearBufferfv(GL_COLOR, 0, color);
}

DECLARE_MAIN(my_application);