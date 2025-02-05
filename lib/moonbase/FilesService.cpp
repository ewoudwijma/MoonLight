/**
 *   ESP32 SvelteKit
 *
 *   A simple, secure and extensible framework for IoT projects for ESP32 platforms
 *   with responsive Sveltekit front-end built with TailwindCSS and DaisyUI.
 *   https://github.com/theelims/ESP32-sveltekit
 *
 *   Copyright (C) 2018 - 2023 rjwats
 *   Copyright (C) 2023 - 2024 theelims
 *   Copyright (C) 2025 - 2025 ewowi
 *
 *   All Rights Reserved. This software may be modified and distributed under
 *   the terms of the LGPL v3 license. See the LICENSE file for details.
 **/

// Error (lowest)
// Warning
// Info
// Debug
// Verbose (highest)


#include <FilesService.h>

#include <ESPFS.h>

//recursively fill a fileArray with all files and folders on the FS
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
            fileObject["isFile"] = !file.isDirectory();
            // Serial.printf("file %s (%d)\n", file.path(), file.size());
			if (file.isDirectory())
			{
				addFolder(file, fileObject["files"].to<JsonArray>());
			}
			else
			{
                fileObject["size"] = file.size();
                fileObject["time"] = file.getLastWrite();
			}
            // serializeJson(fileObject, Serial);
            // Serial.printf("\n");
            file.close();
		}
	}
}

void FilesState::read(FilesState &state, JsonObject &root)
{
    root["name"] = "/";
    //crashes for some reason: ???
    // root["fs_total"] = ESPFS.totalBytes() / 1000;
    // root["fs_used"] = ESPFS.usedBytes() / 1000;
    File folder = ESPFS.open("/");
    addFolder(folder, root["files"].to<JsonArray>());
    folder.close();
    // print->printJson("FilesState::read", root);
    ESP_LOGD("Files", "FilesState::read");
}

//utility function
void extractPath(const char *filepath, char *path) {
    const char *lastSlash = strrchr(filepath, '/');
    if (lastSlash != NULL) {
        size_t pathLength = lastSlash - filepath;
        strncpy(path, filepath, pathLength);
        path[pathLength] = '\0';
    } else {
        // No directory separator found, the entire filepath is the filename
        strcpy(path, "");
    }
}

StateUpdateResult FilesState::update(JsonObject &root, FilesState &state)
{
    bool changed = false;

    JsonArray deletes = root["deletes"].as<JsonArray>();
    if (!deletes.isNull()) {
        for (JsonObject var : deletes) {
            ESP_LOGD("Files", "delete %s %s \n", var["name"].as<const char*>(), var["isFile"]?"File":"Folder");
            // print->printJson("new file", var);
            if (var["isFile"])
                ESPFS.remove(var["path"].as<const char*>());
            else
                ESPFS.rmdir(var["path"].as<const char*>());

            changed = true;
        }
    }

    JsonArray news = root["news"].as<JsonArray>();
    if (!news.isNull()) {
        for (JsonObject var : news) {
            ESP_LOGD("Files", "new %s %s \n", var["name"].as<const char*>(), var["isFile"]?"File":"Folder");
            // print->printJson("new file", var);
            if (var["isFile"]) {
                File file = ESPFS.open(var["path"].as<const char*>(), FILE_WRITE);
                const char *contents = var["contents"];
                if (strlen(contents)) {
                    if (!file.write((byte *)contents, strlen(contents))) { //changed not true as contents is not part of the state
                        Serial.println("Write failed");
                    }
                }
                file.close();
            } else {
                ESPFS.mkdir(var["path"].as<const char*>());
            }
            changed = true;
        }
    }

    JsonArray updates = root["updates"].as<JsonArray>();
    if (!updates.isNull()) {
        for (JsonObject var : updates) {
            ESP_LOGD("Files", "update %s %s \n", var["path"].as<const char*>(), var["isFile"]?"File":"Folder");
            // print->printJson("update file", var);
            File file = ESPFS.open(var["path"].as<const char*>(), FILE_WRITE);
            if (!file) {
                Serial.println("Failed to open file");
            }
            else {
                const char *contents = var["contents"];
                if (!file.write((byte *)contents, strlen(contents))) { //changed not true as contents is not part of the state
                    Serial.println("Write failed");
                }
                file.close();

                char newPath[64];
                extractPath(var["path"], newPath);
                strcat(newPath, "/");
                strcat(newPath, var["name"]);

                ESP_LOGD("Files", "rename %s to %s\n", var["path"].as<const char*>(), newPath);

                if (strcmp(var["path"], newPath) != 0) {
                    ESPFS.rename(var["path"].as<const char*>(), newPath);
                }
                changed = true;
            }
        }
    }

    // JsonDocument fileState;
    // File folder = ESPFS.open("/");
    // addFolder(folder, fileState["files"].to<JsonArray>());

    // //check every file/folder in the filesystem (state) if it is still in root
    // //id nor, delete the file/folder
    // walkThroughJson([root, &changed](JsonObject , JsonObject varState) {
    //     JsonObject foundVar =  walkThroughJson([varState, &changed](JsonObject , JsonObject varRoot) {
    //         if (varRoot["path"] == varState["path"])
    //             return varRoot;
    //         return JsonObject();
    //     }, root, "files");

    //     if (foundVar.isNull()) {
    //         ESP_LOGD("Files", "delete %s as not found in root\n", varState["path"].as<const char*>());
    //         ESPFS.remove(varState["path"].as<const char*>());
    //         changed = true;
    //     }
            
    //     return JsonObject();
    // }, fileState.as<JsonObject>(), "files");

    // print->printJson("FilesState::update", root);
    ESP_LOGD("Files", "FilesState:update %d\n", changed);

    return changed?StateUpdateResult::CHANGED:StateUpdateResult::UNCHANGED;
}

FilesService::FilesService(PsychicHttpServer *server,
                                     ESP32SvelteKit *sveltekit) : _httpEndpoint(FilesState::read,
                                                                                                         FilesState::update,
                                                                                                         this,
                                                                                                         server,
                                                                                                         "/rest/filesState",
                                                                                                         sveltekit->getSecurityManager(),
                                                                                                         AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                           _eventEndpoint(FilesState::read,
                                                                                                          FilesState::update,
                                                                                                          this,
                                                                                                          sveltekit->getSocket(),
                                                                                                          "files"),
                                                                                           _webSocketServer(FilesState::read,
                                                                                                            FilesState::update,
                                                                                                            this,
                                                                                                            server,
                                                                                                            "/ws/filesState",
                                                                                                            sveltekit->getSecurityManager(),
                                                                                                            AuthenticationPredicates::IS_AUTHENTICATED),
                                                                                            _socket(sveltekit->getSocket()),
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
    onConfigUpdated();

    // uploadHandler = new PsychicUploadHandler();

    // uploadHandler->onUpload([&](PsychicRequest *request, const String& filename, uint64_t index, uint8_t *data, size_t len, bool last) {
    //     File file;
    //     String path = "/" + filename;

    //     Serial.printf("Writing %d/%d bytes to: %s\n", (int)index+(int)len, request->contentLength(), path.c_str());

    //     if (last) {
    //         Serial.printf("%s is finished. Total bytes: %d\n", path.c_str(), (int)index+(int)len);
    //         update([&](FilesState& state) {
    //             return StateUpdateResult::CHANGED; // notify StatefulService by returning CHANGED
    //         }, "timer");
    //     }

    //     //our first call?
    //     if (!index)
    //         file = ESPFS.open(path, FILE_WRITE);
    //     else
    //         file = ESPFS.open(path, FILE_APPEND);
        
    //     if(!file) {
    //         Serial.println("Failed to open file");
    //         return ESP_FAIL;
    //     }

    //     if(!file.write(data, len)) {
    //         Serial.println("Write failed");
    //         return ESP_FAIL;
    //     }

    //     return ESP_OK;
    // });

    // //gets called after upload has been handled
    // uploadHandler->onRequest([](PsychicRequest *request)
    // {
    //     String url = "/" + request->getFilename();
    //     String output = "<a href=\"" + url + "\">" + url + "</a>";

    //     Serial.printf("uploadHandler->onRequest\n", output.c_str());

    //     return request->reply(output.c_str());
    // });

    // Serial.printf("server->on upload %d\n", _server->count());
    // Serial.printf("server->on upload\n");

    // //serve uploads
    // _server->on("/rest/upload/*", HTTP_POST, uploadHandler);

    //setup the file server
    _server->serveStatic("/rest/file", ESPFS, "/");
}

void FilesService::onConfigUpdated()
{
    Serial.printf("FilesService::onConfigUpdated\n");
}