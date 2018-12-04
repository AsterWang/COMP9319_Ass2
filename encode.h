#ifndef encode_h
#define encode_h
static char *buf;
#include <stdlib.h>
#include <string.h>
void int_swap(int *a, int *b)
{
    int tmp;
    tmp = *a;
    *a = *b;
    *b = tmp;
}

void q3sort(int *rank, int lo, int hi, int d, char DELIMITER)
{
    if (hi <= lo) return;
    
    int lt = lo, gt = hi;//initialize the gt index to be the length of this string
    int v = buf[rank[lo]+d];
    int i = lo + 1;
    while (i <= gt)
    {
        int t = buf[rank[i]+d];
        if (v == DELIMITER){
            if (t != DELIMITER){
                int_swap(&rank[i], &rank[gt]);
                gt --;
            } else {
                if (rank[i] > rank[lt]){
                    int_swap(&rank[i], &rank[gt]);
                    gt --;
                } else {
                    int_swap(&rank[lt], &rank[i]);
                    lt++; i++;
                }
            }
        } else if (t == DELIMITER){ 
            int_swap(&rank[lt], &rank[i]);
            lt++; i++;
        } else {
            if (t < v) {
                int_swap(&rank[lt], &rank[i]);
                lt++; i++;
            }
            else if (t > v) {
                int_swap(&rank[i], &rank[gt]);
                gt--;
            }
            else
                i++;
        }
    }
    
    q3sort(rank, lo, lt-1, d, DELIMITER);
    //Regarding equal part, we increase d, the compare the d-th character to determine the sequence
    if (v >= 0)
        q3sort(rank, lt, gt, d + 1, DELIMITER);
    q3sort(rank, gt + 1, hi, d, DELIMITER);
}

void generate_bwt(FILE* fp_ori_file, char* bwt_file,
                  char* pos_file, char* tmp_file, char DELIMITER){
    fseek(fp_ori_file, 0, SEEK_END);
    int file_size = (int) ftell(fp_ori_file);
    rewind(fp_ori_file);
    int len = file_size;
    buf = malloc(sizeof(char) * len + 1);
    memset(buf, 0, len + 1);
    int i;
    FILE *fout = fopen(bwt_file, "wb");
    FILE *f_pos = fopen(pos_file, "wb");
    FILE *f_tmp = fopen(tmp_file, "wb");
    fread(buf, sizeof(char), file_size, fp_ori_file);
    int delimiter_size = 0;
    for(i = 0; i < len; i ++)
        if (buf[i] == DELIMITER){
            fwrite(&buf[i - 1], sizeof(char), 1, fout);
            delimiter_size ++;
        }
    int rest_file_size = file_size - delimiter_size;
    int *rank = calloc(rest_file_size, sizeof(int));
    delimiter_size = 0;
    int tmp = 0;
    int pre_is_delimiter = 0;
    for (i = 0; i < file_size; i ++)
        if (buf[i] == DELIMITER){
            //if pre_char is delimiter, we need to write into f_pos file in advance in order to avoid missing looping this position
            if(pre_is_delimiter == 1){
                int d = delimiter_size - 1;
                fwrite(&d, sizeof(int), 1, f_pos);
            }
            fwrite(&delimiter_size, sizeof(int), 1, f_tmp);
            delimiter_size ++;
            pre_is_delimiter = 1;
        } else {
            int t = 0;
            fwrite(&t, sizeof(int), 1, f_tmp);
            rank[tmp++] = i;
            pre_is_delimiter = 0;
        }
    fclose(f_tmp);
    f_tmp = fopen(tmp_file, "rb");
    q3sort(rank,0,tmp - 1, 0, DELIMITER);
    int k;
    for (k = 0; k < tmp; ++k){
        int x;
        if (rank[k] == 0){
            x = len - 1;
            fwrite(&(buf[len - 1]), sizeof(char), 1,fout);
        } else {
            x = rank[k] - 1;
            fwrite(&(buf[rank[k] - 1]), sizeof(char), 1, fout);
        }
        if (buf[x] == DELIMITER){
            int y = x * 4;
            fseek(f_tmp,y, SEEK_SET);
            int times = 0;
            fread(&times, sizeof(int), 1, f_tmp);
            fwrite(&times, sizeof(int), 1, f_pos);
        }
    }
    free(rank);
    free(buf);
    fclose(fout);
    fclose(fp_ori_file);
    fclose(f_tmp);
    remove(tmp_file);
    fclose(f_pos);
}
#endif
