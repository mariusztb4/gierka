#pragma once
#include "Loading.h"
#include "SceneType.h"

class LoadingManager
{
public:
    // Konfiguruje Loading scene z zadaniami dla danej docelowej sceny
    static void ConfigureLoadingFor(Loading* loading, SceneType targetScene);

private:
    // Konfiguracje dla ró¿nych scen
    static void SetupGameLoading(Loading* loading);
    static void SetupMenuLoading(Loading* loading);
};