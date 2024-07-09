#ifndef FSEXPLORER_H
#define FSEXPLORER_H

#include <FS.h>
#include <LittleFS.h>
#include "allDefines.h"
#include "networkStuff.h"
#include "restAPI.h"

// Function prototypes
void setupFSexplorer();
void listDir(fs::FS &fs, const char *dirname, uint8_t levels);
void createDir(fs::FS &fs, const char *path);
void removeDir(fs::FS &fs, const char *path);
void readFile(fs::FS &fs, const char *path);
void writeFile(fs::FS &fs, const char *path, const char *message);
void appendFile(fs::FS &fs, const char *path, const char *message);
void renameFile(fs::FS &fs, const char *path1, const char *path2);
void deleteFile(fs::FS &fs, const char *path);
void testFileIO(fs::FS &fs, const char *path);
void APIlistFiles();
void formatLittleFS();
void handleFileUpload();
void reBootESP();
void updateFirmware();

extern const char* rootPath;
extern bool Verbose = false;

#endif
