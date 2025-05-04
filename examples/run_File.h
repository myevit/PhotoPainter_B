#ifndef _RUN_FILE_H_
#define _RUN_FILE_H_

#include "DEV_Config.h"
#include <stdbool.h> // For bool type

#define fileNumber 100
#define fileLen 100

// Settings structure
typedef struct
{
    int mode;         // Display mode (0-3)
    int timeInterval; // Time interval in minutes
    int currentIndex; // Current image index
} Settings_t;

char sdTest(void);
void sdInitTest(void);

void run_mount(void);
void run_unmount(void);

void file_cat(void);

void sdScanDir(void);

char isFileExist(const char *path);
void setFilePath(void);

void updatePathIndex(void);
void file_sort();

// Get random image index for Mode 3
int getRandomImageIndex(void);

// Settings file functions
char loadSettings(Settings_t *settings);
void saveSettings(Settings_t *settings);
void createDefaultSettings(void);

#endif
