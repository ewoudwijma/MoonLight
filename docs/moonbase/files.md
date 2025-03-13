# Files

## Functional

* Navigate through folders
* create files and folders
* edit and upload files (see FileEdit)

## Technical

* filesState: all files on FS
* folderList: all files in a folder
* editableFile: current file
* getState / postFilesState: get filesState and post changes to files (update, delete, new)
* addFile / addFolder: create new items
* breadcrumbs(String): folder path as string array and as string, also stored as cookie
* folderListFromBreadCrumbs: create folderList of current folder
* handleEdit: when edit button pressed: navigate back and forward through folders, edit current file
* confirmDelete: when delete button pressed
* socket files / handleFileState (->folderListFromBreadCrumbs)

Using component FileEdit, see [Components]()

### Server

[Files.h](https://github.com/MoonModules/MoonLight/blob/main/lib/framework/Files.h) and [Files.cpp](https://github.com/MoonModules/MoonLight/blob/main/lib/framework/Files.cpp)

### UI

[Files.svelte](https://github.com/MoonModules/MoonLight/blob/main/interface/src/routes/system/status/Files.svelte)
