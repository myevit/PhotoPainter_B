#include "run_File.h"
#include "f_util.h"
#include "pico/stdlib.h"
#include "hw_config.h"

#include <stdio.h>
#include <stdlib.h> // malloc() free()
#include <string.h>
#include <stdlib.h> // for rand() and srand()
#include <time.h> // for time()
#include "hardware/regs/rosc.h"  // For RP2040 hardware RNG
#include "hardware/regs/addressmap.h"  // For RP2040 hardware RNG
extern int Mode; // From main.c

const char *fileList = "fileList.txt";          // Picture names store files
const char *fileListNew = "fileListNew.txt";    // Sort good picture name temporarily store file
char pathName[fileLen];                         // The name of the picture to display
int scanFileNum = 0;                            // The number of images scanned

static sd_card_t *sd_get_by_name(const char *const name) {
    for (size_t i = 0; i < sd_get_num(); ++i)
        if (0 == strcmp(sd_get_by_num(i)->pcName, name)) return sd_get_by_num(i);
    // DBG_PRINTF("%s: unknown name %s\n", __func__, name);
    return NULL;
}

static FATFS *sd_get_fs_by_name(const char *name) {
    for (size_t i = 0; i < sd_get_num(); ++i)
        if (0 == strcmp(sd_get_by_num(i)->pcName, name)) return &sd_get_by_num(i)->fatfs;
    // DBG_PRINTF("%s: unknown name %s\n", __func__, name);
    return NULL;
}

/* 
    function: 
        Mount an sd card
    parameter: 
        none
*/
void run_mount() {
    const char *arg1 = strtok(NULL, " ");
    if (!arg1) arg1 = sd_get_by_num(0)->pcName;
    FATFS *p_fs = sd_get_fs_by_name(arg1);
    if (!p_fs) {
        printf("Unknown logical drive number: \"%s\"\n", arg1);
        return;
    }
    FRESULT fr = f_mount(p_fs, arg1, 1);
    if (FR_OK != fr) {
        printf("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
        return;
    }
    sd_card_t *pSD = sd_get_by_name(arg1);
    // myASSERT(pSD);
    pSD->mounted = true;
}

/* 
    function: 
        Uninstalling an sd card
    parameter: 
        none
*/
void run_unmount() {
    const char *arg1 = strtok(NULL, " ");
    if (!arg1) arg1 = sd_get_by_num(0)->pcName;
    FATFS *p_fs = sd_get_fs_by_name(arg1);
    if (!p_fs) {
        printf("Unknown logical drive number: \"%s\"\n", arg1);
        return;
    }
    FRESULT fr = f_unmount(arg1);
    if (FR_OK != fr) {
        printf("f_unmount error: %s (%d)\n", FRESULT_str(fr), fr);
        return;
    }
    sd_card_t *pSD = sd_get_by_name(arg1);
    // myASSERT(pSD);
    pSD->mounted = false;
}

/* 
    function: 
        Query file content
    parameter: 
        path: File path
*/
static void run_cat(const char *path) {
    // char *arg1 = strtok(NULL, " ");
    if (!path) 
    {
        printf("Missing argument\n");
        return;
    }
    FIL fil;
    FRESULT fr = f_open(&fil, path, FA_READ);
    if (FR_OK != fr) 
    {
        printf("f_open error: %s (%d)\n", FRESULT_str(fr), fr);
        return;
    }
    char buf[256];
    int i=0;
    while (f_gets(buf, sizeof buf, &fil)) 
    {
        printf("%5d,%s", ++i,buf);
    }

    printf("The number of file names read is %d",i);
    scanFileNum = i;

    fr = f_close(&fil);
    if (FR_OK != fr) 
        printf("f_open error: %s (%d)\n", FRESULT_str(fr), fr);
}

/* 
    function: 
        Query files in the corresponding directory
    parameter: 
        dir: Directory path
*/
void ls(const char *dir) {
    char cwdbuf[FF_LFN_BUF] = {0};
    FRESULT fr; /* Return value */
    char const *p_dir;
    if (dir[0]) {
        p_dir = dir;
    } else {
        fr = f_getcwd(cwdbuf, sizeof cwdbuf);
        if (FR_OK != fr) {
            printf("f_getcwd error: %s (%d)\n", FRESULT_str(fr), fr);
            return;
        }
        p_dir = cwdbuf;
    }
    printf("Directory Listing: %s\n", p_dir);
    DIR dj;      /* Directory object */
    FILINFO fno; /* File information */
    memset(&dj, 0, sizeof dj);
    memset(&fno, 0, sizeof fno);
    fr = f_findfirst(&dj, &fno, p_dir, "*");
    if (FR_OK != fr) {
        printf("f_findfirst error: %s (%d)\n", FRESULT_str(fr), fr);
        return;
    }
    while (fr == FR_OK && fno.fname[0]) { /* Repeat while an item is found */
        /* Create a string that includes the file name, the file size and the
         attributes string. */
        const char *pcWritableFile = "writable file",
                   *pcReadOnlyFile = "read only file",
                   *pcDirectory = "directory";
        const char *pcAttrib;
        /* Point pcAttrib to a string that describes the file. */
        if (fno.fattrib & AM_DIR) {
            pcAttrib = pcDirectory;
        } else if (fno.fattrib & AM_RDO) {
            pcAttrib = pcReadOnlyFile;
        } else {
            pcAttrib = pcWritableFile;
        }
        /* Create a string that includes the file name, the file size and the
         attributes string. */
        printf("%s [%s] [size=%llu]\n", fno.fname, pcAttrib, fno.fsize);

        fr = f_findnext(&dj, &fno); /* Search for next item */
    }
    f_closedir(&dj);
}

/* 
    function: 
        Query the images in the directory and save their names to the appropriate file
    parameter: 
        dir: Directory path
        path: File path
*/
void ls2file(const char *dir, const char *path) {
    char cwdbuf[FF_LFN_BUF] = {0};
    FRESULT fr; /* Return value */
    char const *p_dir;
    if (dir[0]) {
        p_dir = dir;
    } else {
        fr = f_getcwd(cwdbuf, sizeof cwdbuf);
        if (FR_OK != fr) {
            printf("f_getcwd error: %s (%d)\n", FRESULT_str(fr), fr);
            return;
        }
        p_dir = cwdbuf;
    }
    printf("Directory Listing: %s\n", p_dir);
    DIR dj;      /* Directory object */
    FILINFO fno; /* File information */
    memset(&dj, 0, sizeof dj);
    memset(&fno, 0, sizeof fno);
    fr = f_findfirst(&dj, &fno, p_dir, "*");
    if (FR_OK != fr) {
        printf("f_findfirst error: %s (%d)\n", FRESULT_str(fr), fr);
        return;
    }

    int filNum=0;
    FIL fil;
    fr =  f_open(&fil, path, FA_CREATE_ALWAYS | FA_WRITE);
    if(FR_OK != fr && FR_EXIST != fr)
        panic("f_open(%s) error: %s (%d) \n", path, FRESULT_str(fr), fr);
    // f_printf(&fil, "{");
    while (fr == FR_OK && fno.fname[0]) { /* Repeat while an item is found */
        /* Create a string that includes the file name, the file size and the
         attributes string. */
        const char *pcWritableFile = "writable file",
                   *pcReadOnlyFile = "read only file",
                   *pcDirectory = "directory";
        const char *pcAttrib;
        /* Point pcAttrib to a string that describes the file. */
        if (fno.fattrib & AM_DIR) {
            pcAttrib = pcDirectory;
        } else if (fno.fattrib & AM_RDO) {
            pcAttrib = pcReadOnlyFile;
        } else {
            pcAttrib = pcWritableFile;
        }
        /* Create a string that includes the file name, the file size and the
         attributes string. */
        if(fno.fname) {
            // f_printf(&fil, "%d %s\r\n", filNum, fno.fname);
            f_printf(&fil, "pic/%s\r\n", fno.fname);
            filNum++;
        }
        fr = f_findnext(&dj, &fno); /* Search for next item */
    }
    // f_printf(&fil, "}");
    // printf("The number of file names written is: %d\n" ,filNum);
    // scanFileNum = filNum;
    fr = f_close(&fil);
    if (FR_OK != fr) {
        printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    }
    f_closedir(&dj);
}

/* 
    function: 
        TF card and file system initialization and testing
    parameter: 
        none
*/
void sdInitTest(void)
{
    puts("Hello, world!");

    // See FatFs - Generic FAT Filesystem Module, "Application Interface",
    // http://elm-chan.org/fsw/ff/00index_e.html
    sd_card_t *pSD = sd_get_by_num(0);
    FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
    if (FR_OK != fr) panic("f_mount error: %s (%d)\n", FRESULT_str(fr), fr);
    FIL fil;
    const char* const filename = "filename.txt";
    fr = f_open(&fil, filename, FA_OPEN_APPEND | FA_WRITE);
    if (FR_OK != fr && FR_EXIST != fr)
        panic("f_open(%s) error: %s (%d)\n", filename, FRESULT_str(fr), fr);
    if (f_printf(&fil, "Hello, world!\n") < 0) {
        printf("f_printf failed\n");
    }
    fr = f_close(&fil);
    if (FR_OK != fr) {
        printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    }
    f_unmount(pSD->pcName);

    puts("Goodbye, world!");
}

/* 
    function: 
        TF card mounting test
    parameter: 
        none
*/
char sdTest(void)
{
    printf("111111111\n");
    sd_card_t *pSD = sd_get_by_num(0);
    printf("222222222\n");
    FRESULT fr = f_mount(&pSD->fatfs, pSD->pcName, 1);
    printf("333333333\n");
    if(FR_OK != fr) {
        printf("4444444444\n");
        return 1;
    }
    else {
        printf("555555555\n");
        f_unmount(pSD->pcName);
        return 0;
    }
}


/* 
    function: 
        Gets the contents of the list file
    parameter: 
        none
*/
void file_cat(void)
{
    run_mount();

    FRESULT fr; /* Return value */
    FIL fil;
    fr = f_open(&fil, fileList, FA_READ);
    if (FR_OK != fr) 
    {
        printf("f_open error: %s (%d)\n", FRESULT_str(fr), fr);
        run_unmount();
        return;
    }

    // First count the number of lines in the file
    char line[fileLen];
    int i = 0;
    const int MAX_FILES = 100000; // Set a reasonable limit to prevent memory issues
    
    while (f_gets(line, sizeof(line), &fil) && i < MAX_FILES) 
    {
        i++;
    }
    
    if (i >= MAX_FILES) {
        printf("Warning: File limit of %d reached. Some files may be ignored.\n", MAX_FILES);
    }
    
    printf("The number of file names read is %d\n", i);
    scanFileNum = i;
    
    f_close(&fil);
    run_unmount();
}

/* 
    function: 
        Scan the image directory and save the results directly, regardless of whether the list file exists
    parameter: 
        none
*/
void sdScanDir(void)
{   
    run_mount();

    ls2file("0:/pic", fileList);
    printf("ls %s\r\n", fileList);
    run_cat(fileList);

    run_unmount();
}

/* 
    function: 
        Read the name of the image to be refreshed and store it in an array for use by this program
    parameter: 
        none
*/
void fil2array(int index)
{
    run_mount();

    FRESULT fr; /* Return value */
    FIL fil;

    fr =  f_open(&fil, fileList, FA_READ);
    if(FR_OK != fr && FR_EXIST != fr) {
        printf("fil2array open error\r\n");
        run_unmount();
        return;
    }

    // printf("ls array path\r\n");
    for(int i=0; i<index; i++) {
        if(f_gets(pathName, 999, &fil) == NULL) {
            break;
        }
        // printf("%s", pathName[i]);
    }

    f_close(&fil);
    run_unmount();
}

/* 
    function: 
        Set the image index number, will be written to the index file
    parameter: 
        index: Picture index number
*/
static void setPathIndex(int index)
{
    FRESULT fr; /* Return value */
    FIL fil;
    UINT br;

    run_mount();

    fr =  f_open(&fil, "index.txt", FA_OPEN_ALWAYS | FA_WRITE);
    if(FR_OK != fr && FR_EXIST != fr) {
        printf("setPathIndex open error\r\n");
        run_unmount();
        return;
    }
    f_printf(&fil, "%d\r\n", index);
    printf("set index is %d\r\n", index);

    f_close(&fil);
    run_unmount();
}

/* 
    function: 
        Gets the current index number from the index file
    parameter: 
        none
    return: 
        Picture index number
*/
static int getPathIndex(void)
{
    int index = 0;
    char indexs[10];
    FRESULT fr; /* Return value */
    FIL fil;

    run_mount();

    fr =  f_open(&fil, "index.txt", FA_READ);
    if(FR_OK != fr && FR_EXIST != fr) {
        printf("getPathIndex open error\r\n");
        run_unmount();
        return 0;
    }
    f_gets(indexs, 10, &fil);
    sscanf(indexs, "%d", &index);   // char to int
    if(index > scanFileNum) 
    {
        index = 1;
        printf("get index over scanFileNum\r\n");    
    }
    if(index < 1)
    {
        index = 1;
        printf("get index over one\r\n");  
    }
    printf("get index is %d\r\n", index);
    
    f_close(&fil);
    run_unmount();
    
    return index;
}

/* 
    function: 
        Set the image path according to the current image index number
    parameter: 
        none
*/
void setFilePath(void)
{
    int index = 1;

    if(isFileExist("index.txt")) {
        printf("index.txt is exist\r\n");
        index = getPathIndex();
    }
    else {
        printf("creat and set Index 0\r\n");    
        setPathIndex(1);
    }
    
    fil2array(index);
    printf("setFilePath is %s\r\n", pathName);
}

/* 
    function: 
        Get random image index for Mode 3
    parameter: 
        none
    return: 
        Random image index
*/
int getRandomImageIndex(void)
{
    int index = 1;
    
    if(scanFileNum <= 1) {
        return 1; // If there's only one file or none, return 1
    }
    
    // Use RP2040 hardware to generate better random numbers
    // Get true random seed from RP2040 ring oscillator
    uint32_t random_seed = 0;
    
    // Use free-running oscillator for randomness
    volatile uint32_t *rnd_reg = (uint32_t *)(ROSC_BASE + ROSC_RANDOMBIT_OFFSET);
    
    // Mix in some hardware noise
    for (int i = 0; i < 32; i++) {
        // Wait a bit
        for (int j = 0; j < 30; j++) {
            asm volatile("nop");
        }
        // Get one random bit
        random_seed = (random_seed << 1) | (*rnd_reg & 1);
    }
    
    srand(random_seed);
    
    // Generate random number between 1 and scanFileNum
    index = rand() % scanFileNum + 1;
    
    printf("Random index selected: %d (from seed: %lu)\r\n", index, random_seed);
    return index;
}

/* 
    function: 
        Update the image index. Update the image index after the image refresh is successful
    parameter: 
        none
*/
void updatePathIndex(void)
{
    int index = 1;

    index = getPathIndex();
    
    if(Mode == 3) {
        // For Mode 3, get a random index
        index = getRandomImageIndex();
    } else {
        // For other modes, increment the index
        index++;
        if(index > scanFileNum)
            index = 1;
    }
    
    setPathIndex(index);
    printf("updatePathIndex index is %d\r\n", index);
}

/* 
    function: 
        Checks if the file exists
    parameter: 
        none
    return: 
        0: inexistence
        1: exist
*/
char isFileExist(const char *path)
{
    FRESULT fr; /* Return value */
    FIL fil;

    run_mount();

    fr =  f_open(&fil, path, FA_READ);
    if(FR_OK != fr && FR_EXIST != fr) {
        printf("%s is not exist\r\n", path);
        run_unmount();
        return 0;
    }
    
    f_close(&fil);
    run_unmount();

    return 1;
}


// Compare function for qsort
int compare_strings(const char *a, const char *b) {
    return strcmp(a, b);
}


/* 
    function: 
        Custom quick sort for sorting a 2D array of strings
    parameter: 
        arr: The array to sort
        left: Sort starting point
        right: End of the order, notice minus one
*/
void custom_qsort(char arr[fileNumber][fileLen], int left, int right) {
    if (left >= right) {
        return;
    }

    int pivot_index = left;
    char pivot[100];
    strcpy(pivot, arr[pivot_index]);

    int i = left;
    int j = right;

    while (i <= j) {
        while (compare_strings(arr[i], pivot) < 0) {
            i++;
        }
        while (compare_strings(arr[j], pivot) > 0) {
            j--;
        }
        if (i <= j) {
            char temp[100];
            strcpy(temp, arr[i]);
            strcpy(arr[i], arr[j]);
            strcpy(arr[j], temp);
            i++;
            j--;
        }
    }

    custom_qsort(arr, left, j);
    custom_qsort(arr, i, right);
}


/* 
    function: 
        Array copy and sort
*/
void file_copy(char temp[fileNumber][fileLen], char templist[fileNumber/2][fileLen], char templistnew[fileNumber/2][fileLen], char count)
{
    memcpy(temp, templist, fileNumber/2*fileLen);
    memcpy(temp[fileNumber/2], templistnew, count*fileLen);
    custom_qsort(temp, 0, fileLen/2 + count -1);
    memcpy(templist, temp, fileNumber/2*fileLen);
    memcpy(templistnew, temp[fileNumber/2], count*fileLen);
}

/* 
    function: 
        Array copy
*/
void file_copy1(char temp[fileNumber][fileLen], char templist[fileNumber/2][fileLen])
{
    memcpy(templist, temp, fileNumber/2*fileLen);
}

void file_copy2(char temp[fileNumber][fileLen], char templist[fileNumber/2][fileLen])
{
    memcpy(templist, temp[fileNumber/2], fileNumber/2*fileLen);
}


/* 
    function: 
        Read the contents of the file, write them into an array, and sort
    parameter: 
        temp: Array to be written
        count: The amount to write
        fil: File pointer
    return: 
        Returns the number of arrays written
*/
char file_gets(char temp[][fileLen], char count, FIL* fil)
{
    for(char i=0; i<count; i++)
    {
        strcpy(temp[i], "");
    }

    char i=0;
    for(i=0; i<count; i++)
    {
        if(f_gets(temp[i], 999, fil) == NULL) 
        {
            custom_qsort(temp, 0, i-1);
            return i;
            break;
        }
    }
    custom_qsort(temp, 0, count-1);
    return i;
}

/* 
    function: 
        Read the contents saved in a temporary file
    parameter: 
        temp: Array to be written
        path: Temporary file name
    return: 
        Returns the number of arrays written
*/
char file_temporary_gets(char temp[][fileLen], const char *path)
{
    for(char i=0; i<50; i++)
    {
        strcpy(temp[i], "");
    }

    FRESULT fr; /* Return value */
    FIL fil;
    char i=0;

    fr =  f_open(&fil, path, FA_READ);
    if(FR_OK != fr && FR_EXIST != fr) {
        printf("Error opening temporary file\r\n");
        printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
        run_unmount();
        return -1;
    }

    for(i=0; i<fileNumber/2; i++)
    {
        if(f_gets(temp[i], 999, &fil) == NULL) 
        {
            f_close(&fil);
            return i;
        }
    }
    f_close(&fil);
    return i;
}


/* 
    function: 
        Write count string of data to temporary file
    parameter: 
        temp: What to write
        count: Number of writes
        path: Temporary file name
*/
void file_temporary_puts(char temp[][fileLen], char count, const char *path)
{
    FRESULT fr; /* Return value */
    FIL fil;
    fr =  f_open(&fil, path, FA_CREATE_ALWAYS | FA_WRITE);
    if(FR_OK != fr && FR_EXIST != fr)
        panic("f_open(%s) error: %s (%d) \n", path, FRESULT_str(fr), fr);

    for(char i=0; i<count; i++)
        f_puts(temp[i], &fil);

    f_close(&fil);
}

/* 
    function: 
        Write count string of data to file
    parameter: 
        temp: What to write
        count: Number of writes
        fil: File pointer
*/
void file_puts(char temp[][fileLen], char count, FIL* fil)
{
    for(char i=0; i<count; i++)
        f_puts(temp[i], fil);
}



/* 
    function: 
        The name of the file that created the temporary file
    parameter: 
        temp: File name storage array
        count: Number of pictures
    return: 
        Generate the number of temporary file names
*/
int Temporary_file(char temp[][10], int count)
{
    int k = 0;
    int i = 0;
    char str1[10] = "ls";
    char str2[10];
    k = (count % 50) ? (count / 50) : (count / 50 - 1);
    for (i = 0 ; i < k; i++)
    {
        memcpy(temp[i], str1, sizeof(str1));
        sprintf(str2, "%d", i);
        strcat(temp[i], str2);
    }
    printf("Total number of temporary file names generated: %d\r\n",k);
    return i;
}

/* 
    function: 
        Delete the temporary file name and rename the sorted file
    parameter: 
        temp: File name storage array
        count: Number of temporary files
*/
void file_rm_ren(char temp[][10], int count)
{
    FRESULT fr; /* Return value */

    printf("remove temporary file\r\n");
    for(int i=0; i<count; i++)
    {
        fr = f_unlink(temp[i]);
        if (FR_OK != fr) 
        {
            printf("f_unlink error: %s (%d)\n", FRESULT_str(fr), fr);
        }
    }

    printf("remove fileList\r\n");
    fr = f_unlink(fileList);
    if (FR_OK != fr) {
        printf("f_unlink error: %s (%d)\n", FRESULT_str(fr), fr);
    }

    printf("rename fileListNew to fileList\r\n");
    fr = f_rename(fileListNew, fileList);
    if (FR_OK != fr) {
        printf("f_rename error: %s (%d)\n", FRESULT_str(fr), fr);
    }
}

/* 
    Sort the image names in the file
*/
void file_sort()
{
    char temp[fileNumber][fileLen];
    char templist1[fileNumber/2][fileLen];
    char templist2[fileNumber/2][fileLen];
    char Temporary_file_name[1000][10];
    int file_count, file_count1;

    file_count = Temporary_file(Temporary_file_name, scanFileNum);  
      
    run_mount();

    FRESULT fr; /* Return value */
    FIL fil, fil1;

    fr =  f_open(&fil, fileList, FA_READ);
    if(FR_OK != fr && FR_EXIST != fr) {
        printf("file open error2\r\n");
        run_unmount();
        return;
    }

    int scanFileNum1=0;
    int scanFileNum2=0;
    for(char i=0; i<fileNumber; i++)
    {
        if(f_gets(temp[i], 999, &fil) == NULL) 
        {
            scanFileNum1 = i;
            break;
        }
        if(i == 99)
            scanFileNum1 = 100;
    }

    if(file_count < 2)
    {
        custom_qsort(temp, 0, scanFileNum1-1);
        fr = f_close(&fil);
        if (FR_OK != fr) {
            printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
        }

        fr =  f_open(&fil, fileList, FA_CREATE_ALWAYS | FA_WRITE);
        if(FR_OK != fr && FR_EXIST != fr)
            panic("f_open1(%s) error: %s (%d) \n", fileList, FRESULT_str(fr), fr);

        file_puts(temp, scanFileNum1, &fil);

        fr = f_close(&fil);
        if (FR_OK != fr) {
            printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
        }
    }
    else
    {
        file_count1 = 0; 
        custom_qsort(temp, 0, fileLen-1);
        file_copy1(temp, templist1);
        file_copy2(temp, templist2);

        file_temporary_puts(templist2, fileNumber/2, Temporary_file_name[file_count1++]);

        do
        {
            scanFileNum1 = file_gets(templist2, fileNumber/2, &fil);
            if(!(strcmp(templist1[fileNumber/2-1], templist2[0]) < 0))
            {
                file_copy(temp, templist1, templist2, scanFileNum1);
            }
            file_temporary_puts(templist2, scanFileNum1, Temporary_file_name[file_count1++]);
        }while(scanFileNum1 == fileNumber/2);

        fr = f_close(&fil);
        if (FR_OK != fr) {
            printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
        }

        fr =  f_open(&fil, fileListNew, FA_CREATE_ALWAYS | FA_WRITE);
        if(FR_OK != fr && FR_EXIST != fr)
            panic("f_open1(%s) error: %s (%d) \n", fileListNew, FRESULT_str(fr), fr);

        file_puts(templist1, fileNumber/2, &fil);

        for (int i = 0; i < file_count; i++)
        {
            scanFileNum2 = file_temporary_gets(templist1, Temporary_file_name[i]);
            for (int j = i+1; j < file_count; j++)
            {
                scanFileNum1 = file_temporary_gets(templist2, Temporary_file_name[j]);
                if(!(compare_strings(templist1[fileNumber/2-1], templist2[0]) < 0))
                {
                    file_copy(temp, templist1, templist2, scanFileNum1);
                    file_temporary_puts(templist2, scanFileNum1, Temporary_file_name[j]);
                }
            }
            file_puts(templist1, scanFileNum2, &fil);
        }
        fr = f_close(&fil);
        if (FR_OK != fr) {
            printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
        }

        file_rm_ren(Temporary_file_name, file_count);
    }
    run_unmount();
}



