
#define JLOGV(x, ...) printf(x, ##__VA_ARGS__)


//
// load profile from a file
//
void loadProfile(char* profile)
{
    char strLine[64];
    char *pstrLine;
    int pos, len;
    std::string strPara;
    int strength = 0;

    FILE* fd = fopen(profile, "rt");
    if (fd == NULL) {
        JLOGV("Open File(%s) with error(%s)\n", profile, strerror(errno));
        return;
    }

    while (!feof(fd)) {
        pstrLine = fgets(strLine, 64, fd);
        if (pstrLine) {
            strLine[63] = '\0';
            strPara = strLine;
            pos = strPara.find_first_of("=");
            if (pos < 0)
                continue;
            len = strlen(strLine);
            if ((len <= 4) || (strLine[0] == '\#')) {
                continue;
            }
            if (strLine[len-1] == '\n')
                strLine[len-1] = '\0';
            //JLOGV("%s:%d\n", strLine, len);
            strLine[pos] = '\0';

            //_redlipStrength=0
            if (!strcmp(strLine, "_redlipStrength")) {
            strength = atoi(strLine+pos+1);
            JLOGV("%s=%d\n", strLine, strength);
            _redlipStrength = strength;
            }
           
        }
    }

    fclose(fd);
}

