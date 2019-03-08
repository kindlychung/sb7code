#include "sb7.h"

class my_application : public sb7::application {
   public:
    void render(double currentTime) {
        static const float red[]{.8f, 0.2f, 0.5f, 1.f};
        glClearBufferfv(GL_COLOR, 0, red);
    }
};

DECLARE_MAIN(my_application);