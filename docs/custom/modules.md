# Modules

## Functional

With Moonbase Modules it is possible to create a new module entirely from one c++ file

See [Demo](https://moonmodules.org/MoonLight/custom/module/demo/) and [Animations](https://moonmodules.org/MoonLight/custom/module/animations/) as examples

## Technical

* Create a class which inherits from Module e.g. class ModuleDemo : public Module
* Call the Module constructor with the name of the module.
    * This name will be used to set up http endpoints and webserver sockets
* Implement function setupDefinition to create a json document with the datastructure
    * Stored on the file system
    * Used to generate the UI
    * Used to initialy create the module data
* Implement function onUpdate to define what happens if data changes
    * struct UpdatedItem defines the update (parent property, name of property, value and index (in case of multiple records)
    * This is run in the httpd / webserver task. To run it in the main (application task use runInLoopTask - see ModuleAnimations)
* Add the module in main.cpp
* Add the module in menu.svelte (this will be automated in the future)

<img width="400" alt="image" src="https://github.com/user-attachments/assets/fcec054b-a918-4e31-bca4-71b75aa6673c" />

Done by the Modules implementation
* Module.h will generate all the required server code
* Module.svelte will deal with the UI
* MultiInput.svelte is used by Module.svelte to display the right UI widget based on what is defined in the definition json

### Server

[Instances.h](https://github.com/MoonModules/MoonLight/blob/main/lib/framework/Instances.h) and [Instances.cpp](https://github.com/MoonModules/MoonLight/blob/main/lib/framework/Instances.cpp)

### UI

[Instances.svelte](https://github.com/MoonModules/MoonLight/blob/main/interface/src/routes/system/status/Instances.svelte)
