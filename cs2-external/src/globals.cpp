#include "globals.h"

namespace config
{
    namespace esp
    {
        bool bEnable = false;
        bool bBoxes = false;
        bool bSnaplines = false;
        bool bHealthBar = false;
        bool bHealthInt = false;
        bool bHeadCircles = false;
        float crossSize = false;
        float fovSlider = 13.f;
        float bRounding = false;
        float bLineout = false;
        bool bShowTeam = false;
        bool bSkeleton = false;
        bool bWeapon = false;
        bool bName = false;
        bool bDemo = false;
        bool bRadar = false;

        float boxColor[4] = { 1.f, 1.f, 1.f, 1.f };
        float SnaplineColor[4] = { 1.f, 1.f, 1.f, 1.f };
        float TextColor[4] = { 1.f, 1.f, 1.f, 1.f };
        float headCirclesColor[4] = { 1.f, 1.f, 1.f, 1.f };
        float skeletonColor[4] = { 1.f, 1.f, 1.f, 1.f };
        float fovCircleColor[4] = { 0.5f, 1.f, 1.f, 1.f };
    }

    namespace aim
    {
        bool bAimbot = false;
        bool bFovCircle = false;
        bool bSemirage = false;
        bool bFillc = false;
        bool bTrigger = false;
        bool bAutofire = false;
        float fovSlider = 40.f;
        float fovtrigger = 5.f;
        float triggerdelay = 1.f;
        float smoothness = 0.5f;
        int smoothingSlider = 5;
    }

    namespace misc
    {
        bool bCros = false;
    }
}

namespace globals
{
    const int windowWidth = 1920;
    const int windowHeight = 1080;
    uintptr_t base_module;
}

namespace gui
{
    bool isRunning = true;
    bool isOpen = true;

    bool showVisuals = false;
    bool showMisc = false;
    bool showAim = false;
    bool showConfig = false;
    bool showAbout = false;

}

namespace vars
{
    std::chrono::seconds remainingTime = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::seconds(0));
}