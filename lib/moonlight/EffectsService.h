/**
    @title     MoonLight
    @file      EffectsService.h
    @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
    @Authors   https://github.com/MoonModules/MoonLight/commits/main
    @Doc       https://moonmodules.org/MoonLight/moonlight/effects/
    @Copyright © 2025 Github MoonLight Commit Authors
    @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
    @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
**/

#ifndef EffectsService_h
#define EffectsService_h

#include <EventSocket.h>
#include <HttpEndpoint.h>
#include <EventEndpoint.h>
#include <WebSocketServer.h>
#include <PsychicHttp.h>
#include <FSPersistence.h>
#include "FixtureService.h"

class EffectsState
{
public:

    uint16_t effect = UINT16_MAX;
    uint16_t projection = UINT16_MAX;

    static void read(EffectsState &state, JsonObject &root);

    static StateUpdateResult update(JsonObject &root, EffectsState &state);
};

class EffectsService : public StatefulService<EffectsState>
{
public:
    EffectsService(PsychicHttpServer *server,
                      ESP32SvelteKit *sveltekit, FixtureService *fixtureService);

    void begin();

protected:
    EventSocket *_socket;

private:
    HttpEndpoint<EffectsState> _httpEndpoint;
    EventEndpoint<EffectsState> _eventEndpoint;
    WebSocketServer<EffectsState> _webSocketServer;
    FSPersistence<EffectsState> _fsPersistence;

    FixtureService *_fixtureService;

    void onConfigUpdated();
};

#endif
