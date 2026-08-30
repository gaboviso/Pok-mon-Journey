#include "engine/App.hpp"

int main() {
    journey::App app;

    if (!app.initialize()) {
        return 1;
    }

    app.run();

    return 0;
}
