#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    long offset;
    long start;
    long end;
    size_t size;
} cave_code_info;

int scan_cave_codes(const char *file_name, size_t min_size){
    FILE *file = fopen(file_name, "rb");
    if(file == NULL){
        perror("\n[!] File not open\n");
        exit(1);
    } else{
        fseek(file, 0, SEEK_END);
        long int file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        uint8_t *buffer = (uint8_t *)malloc(file_size);
        if(buffer == NULL){
            perror("[!] Not allocated");
            fclose(file);
            exit(1);
        } else{
            fread(buffer, 1, file_size, file);
            fclose(file);

            cave_code_info *cave_struct  = (cave_code_info *)malloc(sizeof(cave_code_info) * file_size);
            long cave_count = 0;
            for(long i = 0;i < file_size; i++){
                if(buffer[i] == 0x00 || buffer[i] == 0){
                    long start_null_byte = i;
                    while(i < file_size && buffer[i] == 0x00 || buffer[i] == 0){
                        i++;
                    }
                    long end_null_byte = i;
                    size_t size_null_byte = start_null_byte - end_null_byte;
                    if(size_null_byte >= min_size){
                        cave_struct[cave_count].offset = start_null_byte;
                        cave_struct[cave_count].start = start_null_byte;
                        cave_struct[cave_count].end = end_null_byte;
                        cave_struct[cave_count].size = size_null_byte;
                        cave_count = cave_count + 1;
                    } else{
                        i++;
                    }
                } else{
                    i++;
                }
                
            }
            if(cave_count > 0){
                printf("\n[+] %d possible cave codes were found in %s\n", cave_count, file_name);
                int i = 0;
                while(cave_count > i){
                    printf("\n[ %d possible cave code ]\n", i);
                    printf("Offset..........:0x%lx\n", cave_struct[i].offset);
                    printf("Start of null byte..........: 0x%lx\n", cave_struct[i].start);
                    printf("End of null byte..........: 0x%lx\n", cave_struct[i].end);
                    printf("Start and end distance(null byte size)..........: %zu bytes away\n", cave_struct[i].size);
                    i = i + 1;
                } 
            } else{
                printf("\n[x] no possible cave code identified in %s\n", file_name);
           }

            free(buffer);
            free(cave_struct);
            
        }
    }
}

int main(int n_arg, char *args[]){
    if(n_arg != 3){
        fprintf(stderr, "\n[!] Please, use: %s ELF minimum-size\n", args[0]);
        return 1;
    } else{
        for (int i = 0;i < strlen(args[2]); i++ ){
            if(args[2][i] != '\0'){
                if(isdigit(args[2][i]) == 0){
                    fprintf(stderr, "\n[!] Please, %s ELF < NUMERIC >\n", args[0]);
                    return 1;
                }
            }             
        }
    }
    unsigned long int min_size = strtoul(args[2], 0, 10);
    scan_cave_codes(args[1], min_size);

    return 0;
}
