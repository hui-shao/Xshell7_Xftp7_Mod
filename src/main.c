#include <stdio.h>
#include <string.h>
#include <windows.h>

#define PATH_MAX_LEN 195

static const unsigned char originalCode_xshell[] = {0x74, 0x11, 0x6A, 0x00, 0x6A, 0x07, 0x6A, 0x01};
static const unsigned char targetCode_xshell[] = {0xEB, 0x11, 0x6A, 0x00, 0x6A, 0x07, 0x6A, 0x01};
static const unsigned char originalCode_xftp[] = {0x75, 0x10, 0x6A, 0x00, 0x6A, 0x07, 0x50, 0x6A};
static const unsigned char targetCode_xftp[] = {0xEB, 0x10, 0x6A, 0x00, 0x6A, 0x07, 0x50, 0x6A};

static FILE *fp = NULL, *fp_backup = NULL;
static char path[PATH_MAX_LEN + 5] = {0};
static const unsigned char *original_p, *target_p;

void backupFile(FILE *dest, FILE *src)
{
    unsigned char buffer[256];
    unsigned int validCount;
    while ((validCount = fread(buffer, sizeof(unsigned char), 256, src)) != 0)
    {
        fwrite(buffer, sizeof(unsigned char), validCount, dest);
    }
}

long int searchPos()
{
    unsigned char buffer[16] = {0};
    unsigned char *buffer_half = buffer + 8;
    long int offset = -1;
    while (fread(buffer_half, sizeof(unsigned char), 8, fp) != 0)
    {
        int ret;
        for (int i = 0; i < 8; ++i)
        {
            ret = memcmp(buffer + i, original_p, 8 * sizeof(unsigned char));
            if (ret == 0)
            {
#if DEBUG_OUTPUT
                MessageBox(NULL, " Found!", "INFO", MB_OK | MB_ICONINFORMATION);
#endif
                offset = ftell(fp) - 16 + i;
                return offset;
            }
        }
        memcpy_s(buffer, 8 * sizeof(unsigned char), buffer_half, 8 * sizeof(unsigned char));
    }
    return offset;
}

int main(int argc, char *argv[])
{
    SetConsoleTitle("Xshell7/Xftp7 Modifier   v1.0");

    /* Argument Parse */
    if (argc == 2)
    {
        memcpy_s(path, sizeof(path) / sizeof(path[0]), argv[1], strnlen_s(argv[1], PATH_MAX_LEN));
        printf("The argument parsed is %s . Argument length: %d\n", path, strnlen_s(argv[1], PATH_MAX_LEN));
    } else if (argc > 2)
    {
        MessageBox(NULL,
                   " Too many arguments supplied\n Drag and drop target files onto the program to use.\n Press OK to exit.",
                   "WARN", MB_OK | MB_ICONWARNING);
        return 1;
    } else
    {
        MessageBox(NULL,
                   " Excepted one argument.\n Drag and drop target files onto the program to use.\n Press OK to exit.",
                   "WARN", MB_OK | MB_ICONWARNING);
        return 1;
    }

    /* Judge - Xshell or Xftp */
    if (strstr(path, "ftp.exe") != NULL)
    {
        printf("Switch to 'Xftp' modify mode.\n");
        original_p = originalCode_xftp;
        target_p = targetCode_xftp;
    } else
    {
        printf("Switch to 'Xshell' modify mode.\n");
        original_p = originalCode_xshell;
        target_p = targetCode_xshell;
    }

    /* Open File */
    errno_t err[2] = {0};
    err[0] = fopen_s(&fp, path, "rb+");
    if (err[0])
    {
        MessageBox(NULL, " Can not open target file!\n Check the path or Run as Administrators", "ERROR",
                   MB_OK | MB_ICONERROR);
        return 1;
    }

    /* Backup */
    strcat_s(path, strnlen_s(path, PATH_MAX_LEN) + 4 + 1, ".bak");
    err[1] = fopen_s(&fp_backup, path, "wb");
    if (err[1])
    {
        MessageBox(NULL, " Can not open backupFile file!", "ERROR", MB_OK | MB_ICONERROR);
        fclose(fp);
        return 1;
    }
    backupFile(fp_backup, fp);
    fclose(fp_backup);


    /* Search Hex value */
    rewind(fp);
    long int offset = searchPos();
    if (offset == -1)
    {
        MessageBox(NULL, " Patch point not found!\n Have nothing to do.", "ERROR", MB_OK | MB_ICONERROR);
        fclose(fp);
        return 1;
    }

    /* Modify */
    fseek(fp, offset, SEEK_SET);
    if (fwrite(target_p, sizeof(unsigned char), 8, fp) == 8)
    {
        MessageBox(NULL, " Modify OK!", "INFO", MB_OK | MB_ICONINFORMATION);
    } else
    {
        MessageBox(NULL, " Modify Failed!", "ERROR", MB_OK | MB_ICONERROR);
    }

    fclose(fp);
    return 0;
}
