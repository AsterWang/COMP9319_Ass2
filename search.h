#ifndef search_h
#define search_h
#define OCCURENCE_BUFFER  3072
#define BWT_BUFFER_SIZE 5000
#define SIG_NUM 127
int n_search(FILE* fp_bwt, FILE* fp_pos_file, FILE* fp_occ,int* c_table,
             int bwt_size ,int first, int last, char* bwt_buf, char* curr_block,char DELIMITER, char* search_type);

void m_search(FILE* fp_bwt, FILE* fp_pos_file, FILE* fp_occ,
              char* search_char, int* c_table,
              int bwt_size, char* bwt_buf, char DELIMITER, char* search_type);
void a_search(char* repeat, int total);


int* create_count_table(int len, FILE* fp_bwt, FILE* fp_occ, char* bwt_buf, char DELIMITER){
    int length = len;
    int* c_table = calloc(SIG_NUM + 1, sizeof(int));
    int buf_size = 0;
    buf_size = len < BWT_BUFFER_SIZE ? len : BWT_BUFFER_SIZE;
    len -= buf_size;
    fread(bwt_buf, sizeof(char), buf_size, fp_bwt);
    int tmp = 0;
    int i;
    for (i = 0; i < length; i ++){
        c_table[bwt_buf[tmp ++]] ++;
        if (tmp == buf_size){
            buf_size = len < BWT_BUFFER_SIZE ? len : BWT_BUFFER_SIZE;
            fread(bwt_buf, sizeof(char), buf_size, fp_bwt);
            len -= buf_size;
            tmp = 0;
        }
        if ((i + 1) % OCCURENCE_BUFFER == 0){
            fwrite(c_table, sizeof(int), SIG_NUM, fp_occ);
        }
    }
    if(length % OCCURENCE_BUFFER != 0 && length > OCCURENCE_BUFFER){
        fwrite(c_table, sizeof(int), SIG_NUM, fp_occ);
    }
    c_table[0] = c_table[DELIMITER]; //set DELIMITER be the smallest character
    c_table[DELIMITER] = 0;
    int n = c_table[0];
    for (i = 1; i < SIG_NUM; i ++){
        if (c_table[i] != 0){
            int tmp = c_table[i];
            c_table[i] = n;
            n += tmp;
        }
    }
    return c_table;
}


/*
 return how many characters that the same with the current character up to the current index.
 */
int get_occurence(int index, char c, FILE* fp_occ, FILE* fp_bwt, char* curr_block, char* bwt_buf){
    int i;
    int c_occ = 0;
    //for small files, we could search the bwt file directly.
    if (index < OCCURENCE_BUFFER){
        fseek(fp_bwt, 0, SEEK_SET);
        for (i = 0; i < index; i ++)
            if (fgetc(fp_bwt) == c)
                c_occ ++;
    } else{ //else, we could search the occurence file to calculate the occurence of this character.
        //previous block number
        int pre_block_number = ((index + 1 - 1 + OCCURENCE_BUFFER) / OCCURENCE_BUFFER) - 2;
        int pos = pre_block_number * SIG_NUM + c;
        fseek(fp_occ, pos * 4, SEEK_SET);
        fread(&c_occ, sizeof(int), 1, fp_occ);
        fseek(fp_occ,(pre_block_number + 1) * 4 * OCCURENCE_BUFFER, SEEK_SET);
        int curr_block_rest_num = index + 1 - (pre_block_number + 1) * OCCURENCE_BUFFER;
        fread(curr_block, sizeof(char), curr_block_rest_num, fp_occ);
        //up to the current index, how many characters that are the same as the current character.
        int bwt_len = 0;
        int rest_chars = index - (pre_block_number + 1) * OCCURENCE_BUFFER;
        bwt_len = rest_chars < BWT_BUFFER_SIZE ? rest_chars : BWT_BUFFER_SIZE;
        rest_chars -= bwt_len;
        fseek(fp_bwt, (pre_block_number + 1) * OCCURENCE_BUFFER, SEEK_SET);
        fread(bwt_buf, sizeof(char), bwt_len, fp_bwt);
        int tmp = 0;
        for (i = (pre_block_number + 1) * OCCURENCE_BUFFER; i < index; i ++){
            if (bwt_buf[tmp] == c)
                c_occ ++;
            tmp ++;
            if (tmp == BWT_BUFFER_SIZE){
                bwt_len = rest_chars < BWT_BUFFER_SIZE ? rest_chars : BWT_BUFFER_SIZE;
                fread(bwt_buf, sizeof(char), bwt_len, fp_bwt);
                rest_chars -= bwt_len;
                tmp = 0;
            }
        }
    }
    return c_occ;
}


void m_search(FILE* fp_bwt, FILE* fp_pos_file, FILE* fp_occ,
              char* search_char, int* c_table,
              int bwt_size, char* bwt_buf,char DELIMITER, char* search_type){
    char *curr_block = calloc(OCCURENCE_BUFFER, sizeof(char));
    int i = (int) strlen(search_char) - 1;
    char c = search_char[i];
    int first = c_table[c];
    if (first == 0){
        if (strcmp(search_type, "-m") == 0)
            printf("0\n");
        if (strcmp(search_type ,"-n") == 0)
            printf("0\n");
    } else {
        int j = 0;
        int last = 0;
        for (j = c + 1; j < SIG_NUM; j ++){
            if (c_table[j] != 0){
                last = c_table[j];
                break;
            }
        }
        if (last == 0)
            last = bwt_size;
        while (first < last && i >= 1){
            c = search_char[i - 1];
            int x = get_occurence(first, c, fp_occ, fp_bwt, curr_block, bwt_buf);
            first = c_table[c] + x;
            int y = get_occurence(last, c, fp_occ, fp_bwt, curr_block, bwt_buf);
            last = c_table[c] + y;
            i --;
        }
        if (last < first){
            if (strcmp(search_type, "-m") == 0)
                printf("0\n");
            if (strcmp(search_type ,"-n") == 0)
                printf("0\n");
        } else {
            if (strcmp(search_type, "-m") == 0)
                printf("%d\n", last - first);
            if (strcmp(search_type ,"-n") == 0)
                n_search(fp_bwt, fp_pos_file, fp_occ, c_table, bwt_size, first, last, bwt_buf, curr_block, DELIMITER, search_type);
            if(strcmp(search_type, "-a") == 0)
                n_search(fp_bwt, fp_pos_file, fp_occ, c_table, bwt_size, first, last, bwt_buf, curr_block, DELIMITER, search_type);
        }
    }
}

void a_search(char* repeat, int total){
    int x = 0;
    int m = 0;
    int j;
    int flag = 0;
    while(total > 0){
        char tmp = repeat[x];
        if (tmp != '\0'){
            m = 0;
            for (j = 0; j < 8; j ++){
                char r =((0x80 >> m) & tmp) == 0?48:49;
                if (r == 49){
                    total --;
                    int record_number = x * 8 + j;
                    printf("%d\n", record_number);
                    if (total == 0){
                        flag = 1;
                        break;
                    }
                }
                m ++;
            }
        }
        if (flag == 1)
            break;
        x ++;
    }
}

/*
 remove duplicate records of m search
 */
int n_search(FILE* fp_bwt, FILE* fp_pos_file, FILE* fp_occ,
             int* c_table, int bwt_size ,
             int first, int last, char* bwt_buf, char* curr_block, char DELIMITER,char* search_type){
    int len = last - first;
    int *records = calloc(last - first, sizeof(int));
    int index = 0;
    int j;
    for (j = first; j < last; j ++)
        records[index ++] = j;
    char* repeat = calloc((bwt_size / 8) + 10, sizeof(char));
    int amount = len;
    int flag = 0;
    int x;
    int a = 0;
    int total = 0;
    int the_delimiter_number = 0;
    char next_char = '\0';
    int record_number = 0, row = 0, column = 0, tmp = 0;
    if (amount > 0){
        while (flag == 0){
            x = 0;
            for (j = 0; j < len; j ++){
                fseek(fp_bwt, records[j], SEEK_SET);
                next_char = fgetc(fp_bwt);
                if (next_char == DELIMITER){
                    a ++;
                    int b = get_occurence(records[j], DELIMITER, fp_occ, fp_bwt, bwt_buf, curr_block);
                    fseek(fp_pos_file, 4 * b, SEEK_SET);
                    fread(&the_delimiter_number, sizeof(int), 1, fp_pos_file);
                    record_number = (the_delimiter_number + 1) % c_table[0] + 1;
		            row = record_number / 8;
                    column = record_number % 8;
                    tmp = ((0x80 >> column) & repeat[row]) == 0 ? 0 : 1;
                    if (tmp == 0){
                        repeat[row] = ((0x80 >> column) | repeat[row]);
                        total ++;
                    }
                    if (a == amount){
                        flag = 1;
                        break;
                    }
                } else {
                    int curr_occ = get_occurence(records[j], next_char, fp_occ, fp_bwt, bwt_buf, curr_block);
                    records[x ++] = c_table[next_char] + curr_occ;
                }
            }
            len = x;
        }
    }
    if (strcmp(search_type, "-n") == 0)
        printf("%d\n", total);
    
    if (strcmp(search_type, "-a") == 0)
        a_search(repeat, total);
    
    free(repeat);
    free(records);
    free(curr_block);
    return total;
}

void i_search(int begin, int end, int* c_table,FILE* fp_bwt, FILE* f_pos_file, FILE* f_occ, char DELIMITER, char* bwt_buf){
    char *curr_block = calloc(OCCURENCE_BUFFER, sizeof(char));
    char m = '\0';
    int pos = begin - 1;
    char *x = calloc(5007, sizeof(char));
    int tmp = 5004;
    x[tmp --] = 10;
    fseek(fp_bwt, pos, SEEK_SET);
    fread(&m, sizeof(char), 1, fp_bwt);
    x[tmp --] = m;
    while(begin <= end){
        if (m == DELIMITER){
            printf("%s", &x[tmp + 2]);
            tmp = 5004;
             begin ++;
            if (begin <= end){
                x[tmp --] = 10;
                fseek(fp_bwt, begin - 1,SEEK_SET);
                fread(&m, sizeof(char), 1, fp_bwt);
                x[tmp --] = m;
                pos = begin - 1;
            }
        } else {
            int begin_pos = c_table[m];
            int c_occ = get_occurence(pos, m, f_occ, fp_bwt, curr_block, bwt_buf);
            pos = begin_pos + c_occ;
            fseek(fp_bwt, pos, SEEK_SET);
            fread(&m, sizeof(char), 1, fp_bwt);
            x[tmp --] = m;
        }
    }
    free(x);
    free(curr_block);
}
#endif
