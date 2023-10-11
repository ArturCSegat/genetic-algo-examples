#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cap_low(const char * low_dimensional_file_name) {
    char * n_start = strstr(low_dimensional_file_name, "kp_");
    if (n_start == NULL) {
        return -1;
    }
    n_start += 3;   
    
    char * cap_start = strchr(n_start, '_');
    if (cap_start == NULL) {
        return -1;
    }
    cap_start += 1;

    char cap_buf[7];
    memset(cap_buf, 0, sizeof cap_buf);

    int off = 0;
    for(;*cap_start != '\0' && off < 7; cap_start += 1) {
        cap_buf[off] = *cap_start;
        off += 1;
    }

    return atoi(cap_buf);
}

int cap_high(const char * large_scale_file_name) {
    char * minus_name = strstr(large_scale_file_name, "knapPI_");
    if (minus_name == NULL) {
        return -1;
    }

    minus_name += 7;
    // skip version/junk
    minus_name += 2;
    char * cap_start =  strchr(minus_name, '_');
    if (cap_start == NULL) {
        return -1;
    }
    cap_start += 1;

    char cap_buf[7];
    memset(cap_buf, 0, sizeof cap_buf);

    int off = 0;
    for(;*cap_start != '_' && off < 7; cap_start += 1) {
        cap_buf[off] = *cap_start;
        off += 1;
    }

    return atoi(cap_buf);
}

int inst_size_high(const char * file_name) {
    char * minus_name = strstr(file_name, "knapPI_");
    if (minus_name == NULL) {
        return -1;
    }

    minus_name += 7;
    // skip version/junk
    minus_name += 2;

    char size_buf[7];
    memset(size_buf, 0, sizeof size_buf);

    int off = 0;
    for(;*minus_name != '_' && off < 7; minus_name += 1) {
        size_buf[off] = *minus_name;
        off += 1;
    }

    return atoi(size_buf);
}

int inst_size_low(const char * file_name) {
    char * minus_name = strstr(file_name, "kp_");
    if (minus_name == NULL) {
        return -1;
    }
    minus_name += 3;   

    char size_buf[7];
    memset(size_buf, 0, sizeof size_buf);

    int off = 0;
    for(;*minus_name != '_' && off < 7; minus_name += 1) {
        size_buf[off] = *minus_name;
        off += 1;
    }

    return atoi(size_buf);
}


int main(int argc, char ** argv) {
    if (argc != 2) {
        printf("error: should provide 1 (ONE) file to read\nusage: %s filename", argv[0]);
        exit(1);
    }    

    int cap = cap_high(argv[1]);
    if (cap == -1) {
        cap = cap_low(argv[1]);
        if (cap == -1) {
            printf("error: bad file name, couldn't parse capacity\n");
            exit(1);
        }
    }
        
    FILE * f_ptr = fopen(argv[1], "r");

    if (f_ptr == NULL) {
        printf("file error");
        exit(1);
    }

    char cr;

    char val_buf[6];
    int v_idx = 0;
    char wei_buf[6];
    int w_idx = 0;

    char * reading = val_buf;
    int * idx_ptr = &v_idx;
    
    while((cr = fgetc(f_ptr)) != EOF) {
        if (*idx_ptr >= 6 || cr == '\r') {
            continue;
        }

        if (cr == ' ') {
            reading = wei_buf;
            idx_ptr = &w_idx;

            if (*idx_ptr != 0) {
                break;
            }


            continue;
        }

        if (cr == '\n') {
            printf("v: %d, w:%d\n", atoi(val_buf), atoi(wei_buf));
            
            int a = atoi(val_buf);
            int b = atoi(wei_buf);

            printf("sum: %d\n", a+b);

            memset(val_buf, 0, sizeof(val_buf));
            memset(wei_buf, 0, sizeof(wei_buf));
            v_idx = 0;
            w_idx = 0;
            reading = val_buf;
            idx_ptr = &v_idx;
            continue;
        }

        reading[*idx_ptr] = cr;
        *idx_ptr += 1;
    }
    
    printf("the max capacity of the knapsack is %d\n", cap);

    fclose(f_ptr);
    return 0;
}
