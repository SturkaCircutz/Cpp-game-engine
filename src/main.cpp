#include "app/Application.h"

int main()
{
    Application application;
    if (!application.Initialize())
    {
        return 1;
    }

    application.Run();
    application.Shutdown();
    return 0;
}
