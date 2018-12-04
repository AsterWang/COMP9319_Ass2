#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "encode.h"
int main(int argc, char* argv[]){
    char DELIMITER;
    if (strcmp(argv[1], "\\n") == 0){
        DELIMITER = 10;
    } else {
        DELIMITER = argv[1][0];
    }
    char tm[] = "/pos";
    char *tmp_file = calloc(100,sizeof(char));
    strcpy(tmp_file, argv[2]);
    strcat(tmp_file, tm);
    FILE *fp = fopen(argv[3], "rb");

    char *pos_file = calloc(100, sizeof(char));
    strcpy(pos_file,argv[4]);
    strcat(pos_file, ".aux");
    generate_bwt(fp, argv[4], pos_file, tmp_file, DELIMITER);
    return 0;
}
