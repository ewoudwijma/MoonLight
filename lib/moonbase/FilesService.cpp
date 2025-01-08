/**
 *   ESP32 SvelteKit
 *
 *   A simple, secure and extensible framework for IoT projects for ESP32 platforms
 *   with responsive Sveltekit front-end built with TailwindCSS and DaisyUI.
 *   https://github.com/theelims/ESP32-sveltekit
 *
 *   Copyright (C) 2018 - 2023 rjwats
 *   Copyright (C) 2023 - 2024 theelims
 *file
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the LGPL v3 license. See the LICENSE file for details.
 **/

#include <FilesService.h>

#include <LittleFS.h>

#define DEFAULT_LED_STATE false

void addFolder(File folder, JsonArray fileArray)
{
	folder.rewindDirectory();
	while (true)
	{
		File file = folder.openNextFile();
		if (!file)
		{
			break;
		}
		else
		{
            JsonObject fileObject = fileArray.add<JsonObject>();
            fileObject["name"] = (char *)file.name(); //enforces copy, solved in latest arduinojson!, see https://arduinojson.org/news/2024/12/29/arduinojson-7-3/
            fileObject["path"] = (char *)file.path(); //enforces copy, solved in latest arduinojson!, see https://arduinojson.org/news/2024/12/29/arduinojson-7-3/
            Serial.printf("file name %s (%d) ", file.name(), file.size());
			if (file.isDirectory())
			{
                // JsonArray filesArray = fileObject["files"].to<JsonArray>();

				addFolder(file, fileObject["files"].to<JsonArray>());
			}
			else
			{
				//delete the file				
                fileObject["size"] = file.size();
                fileObject["time"] = file.getLastWrite();
                // fileObject["contents"] = file.readString();
			}
            // serializeJson(fileObject, Serial);
            Serial.printf("\n");
		}
	}
}

void FilesState::read(FilesState &settings, JsonObject &root)
{
    File folder = LittleFS.open("/");
    root["name"] = "/";
    addFolder(folder, root["files"].to<JsonArray>());
    folder.close();
}

StateUpdateResult FilesState::update(JsonObject &root, FilesState &filesState)
{
    //this must be changed to make it files specific
    boolean newState = root["files_on"] | DEFAULT_LED_STATE;
    if (filesState.filesOn != newState)
    {
        filesState.filesOn = newState;
        return StateUpdateResult::CHANGED;
    }
    return StateUpdateResult::UNCHANGED;
}

FilesService::FilesService(PsychicHttpServer *server,
                                     EventSocket *socket,
                                     SecurityManager *securityManager) : _httpEndpoint(FilesState::read,
                                                                                                         FilesState::update,
                                                                                                         this,
                                                                                                         server,
                                                                                                         "/rest/filesState",
                                                                                                         securityManager,
                                                                                                         AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _eventEndpoint(FilesState::read,
                                                                                                          FilesState::update,
                                                                                                          this,
                                                                                                          socket,
                                                                                                          "files"),
                                                                                           _webSocketServer(FilesState::read,
                                                                                                            FilesState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            "/ws/filesState",
                                                                                                            securityManager,
                                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                             _server(server)
{

    // configure settings service update handler to update state
    addUpdateHandler([&](const String &originId)
                     { onConfigUpdated(); },
                     false);
}

void FilesService::begin()
{
    _httpEndpoint.begin();
    _eventEndpoint.begin();
    _state.filesOn = DEFAULT_LED_STATE;
    onConfigUpdated();

    //setup the file server (easy peasy!)
    _server->serveStatic("/rest/file", LittleFS, "/");

}

void FilesService::onConfigUpdated()
{
    Serial.printf("FilesService::onConfigUpdated\n");
}