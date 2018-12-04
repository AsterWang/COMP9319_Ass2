#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "search.h"
int main(int argc, char* argv[]){
    FILE *fp_bwt = fopen(argv[2],"rb");

    char *pos_file = calloc(100, sizeof(char));
    strcpy(pos_file,argv[2]);
    strcat(pos_file, ".aux");
    FILE* fp_pos_file = fopen(pos_file, "rb");


    char occ_table[] = "/occ_table";
    char *occ_file = calloc(100,sizeof(char));
    strcpy(occ_file, argv[3]);
    strcat(occ_file, occ_table);
    
    FILE* fp_occ = fopen(occ_file, "wb");
    char DELIMITER;
    if (strcmp(argv[1], "\\n") == 0){
        DELIMITER = 10;
    } else {
        DELIMITER = argv[1][0];
    }


    fseek(fp_bwt, 0, SEEK_END);
    int file_size = (int) ftell(fp_bwt);
    fseek(fp_bwt, 0, SEEK_SET);
    char* bwt_buf = calloc(BWT_BUFFER_SIZE + 1,sizeof(char));
    // memset(bwt_buf, 0, BWT_BUFFER_SIZE + 1);
    int* c_table = create_count_table(file_size, fp_bwt, fp_occ, bwt_buf, DELIMITER);
    fclose(fp_occ);
    fp_occ = fopen(occ_file, "rb");
    fseek(fp_bwt, 0, SEEK_SET);
    if (strcmp(argv[4], "-i") == 0){
        const char s[2] = " ";
        char *token;
        char *ptr;
        token = strtok(argv[5], s);
        int begin = (int)strtol(token, &ptr, 10);
        token = strtok(NULL, s);
        int end = (int)strtol(token, &ptr, 10);
        i_search( begin, end, c_table,fp_bwt, fp_pos_file, fp_occ, DELIMITER, bwt_buf);
    } else
        m_search(fp_bwt, fp_pos_file, fp_occ, argv[5], c_table, file_size, bwt_buf, DELIMITER, argv[4]);
    
    free(c_table);
    free(bwt_buf);
    fclose(fp_occ);
    fclose(fp_bwt);
    fclose(fp_pos_file);
    return 0;
}
