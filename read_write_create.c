#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#define LEN 100
#define MAX_FILES 100
#define MAX_FILENAME_LEN 256
#define MAX_LINE_LEN 1024


int write_file(char* str, int len_chars, FILE* file){

    return fwrite(str, 1, len_chars, file);
}
 
 // прототипы функции
int write_file(char* str, int len_chars, FILE* file);
int read_file(char* str, FILE* file);
//char** findTextFiles(char *path);
char **findTextFiles(char *path, int *count);
char* concate_str(char *path, char *entry_name);
void archive();
void mergeTextFiles(char **files, int count, const char *outputFile);


char *readFile(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error opening file %s\n", filename);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *content = (char *)malloc(size + 1);
    fread(content, 1, size, fp);
    content[size] = '\0';
    fclose(fp);

    return content;
}
// Функция, которая создает новый файл и записывает в него данные
void create_new_file(char *filename, char *data) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Ошибка при создании файла %s\n", filename);
        exit(1);
    }
    fprintf(fp, "%s", data);
    fclose(fp);
}

void my_split(char* filename){

    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Ошибка при открытии файла %s\n", filename);
        exit(1);
    }

    char line[MAX_LINE_LEN];
    char file_data[MAX_LINE_LEN];
    char new_filename[MAX_FILENAME_LEN];
    int in_file = 0;
    int line_num = 0;

    while (fgets(line, MAX_LINE_LEN, fp) != NULL) {
        line_num++;

        // Если встретили начальный разделитель
        if (strstr(line, "<<<<") != NULL) {
            // Извлекаем имя нового файла из строки
            char *filename_start = strstr(line, "</") + 1;
            char *filename_end = strstr(line, "\n");
            int filename_len = filename_end - filename_start;
            strncpy(new_filename, filename_start, filename_len);
            new_filename[filename_len] = '\0';

            // Очищаем буфер для нового файла
            memset(file_data, 0, MAX_LINE_LEN);

            // Устанавливаем флаг, что мы находимся внутри файла
            in_file = 1;
        }
        // Если встретили конечный разделитель
        else if (strstr(line, ">>>>") != NULL) {
            // Создаем новый файл и записываем в него данные
            //create_new_file(new_filename, file_data);
            FILE *file_ptr;
            printf("%s\n", new_filename);
            file_ptr = fopen(new_filename, "w+");
            
            int wrnum = write_file(file_data, strlen(file_data), file_ptr);
            //int wrnum = write_file(file_data, 150, file_ptr);// 1 арг - откуда мы читаем данные, 2 арг - сколько читаем за один раз, 3 арг - сколько всего надо прочитать, 4 - куда записать
            // fwrite возвращает колво успешно записанных символов
            // Сбрасываем флаг, что мы находимся внутри файла
            in_file = 0;
        }
        // Если мы находимся внутри файла, то записываем данные в буфер
        else if (in_file) {
            strcat(file_data, line);
        }
        // В противном случае игнорируем строку
        else {
            continue;
        }
    }

    fclose(fp);
}

int main(int argc, char **argv){

    printf("1: Архивировать по пути указаному например /home/bokar/Documents/LR1\n");
    printf("2: Указать путь для раз архивации, например /home/bokar/Documents/LR1/final.txt Архив должен содержать разделители\n");
    int menu;
    scanf("%d", &menu);
    switch (menu) {
        case 1:
            //char str[100]; // создаем массив символов для хранения строки
            //printf("Введите строку: ");
            //scanf("%s", str); // считываем строку с помощью функции scanf()
            //printf("Вы ввели: %s", str); // выводим строку на экран
            archive();

            break;
        case 2:
            //char str2[100]; // создаем массив символов для хранения строки
            //printf("Введите строку: ");
            //scanf("%s", str2); // считываем строку с помощью функции scanf()
            //printf("Вы ввели: %s", str2); // выводим строку на экран
            my_split("/home/bokar/Documents/LR1");

            break;
        default:
            printf("ERROR! Wrong menu item selected");
            return 0;
    }
    

   

//read_write_create
//cc -o read_write_create.exe read_write_create.c
//./read_write_create.exe
}
void archive(){
    char *path = "/home/bokar/Documents/LR1";
    int count = 0;
    char **files = findTextFiles(path, &count);

    if (count > 0) {
        mergeTextFiles(files, count, "/home/bokar/Documents/LR1/final.txt");
    } else {
        printf("No text files found in directory %s\n", path);
    }

    for (int i = 0; i < count; i++) {
        printf("%s\n", files[i]);
        free(files[i]);
    }
    free(files);
}

void mergeTextFiles(char **files, int count, const char *outputFile) {
    FILE *fp = fopen(outputFile, "w");
    if (!fp) {
        fprintf(stderr, "Error opening file %s\n", outputFile);
        return;
    }

    for (int i = 0; i < count; i++) {
        char *content = readFile(files[i]);
        if (content) {
            fprintf(fp, "<<<<%s\n%s\n>>>>\n", files[i], content);
            free(content);
        }
    }

    fclose(fp);
}

char* concate_str(char *path, char *entry_name){
   

    // Вычисляем длины строк
    int path_len = strlen(path);
    int entry_len = strlen(entry_name);

    // Вычисляем длину результирующей строки
    int result_len = path_len + 1 + entry_len;

    // Выделяем память под результирующую строку
    char *result = (char*) malloc(result_len + 1);

    // Копируем первую строку в результирующую
    strcpy(result, path);

    // Добавляем разделитель в результирующую строку
    strcat(result, "/");

    // Добавляем вторую строку в результирующую
    strcat(result, entry_name);

    // Выводим результат
    //printf("%s\n", result);
    return result;
    // Освобождаем память
    free(result);
    
}
char **findTextFiles(char *path, int *count) {
    DIR *dir;
    struct dirent *entry;
    char **files = malloc(sizeof(char *) * MAX_FILES);
    int i = 0;

    if (!(dir = opendir(path))) {
        fprintf(stderr, "Could not open directory: %s\n", path);
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char newPath[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
            snprintf(newPath, sizeof(newPath), "%s/%s", path, entry->d_name);
            char **subFiles = findTextFiles(newPath, count);
            if (subFiles != NULL) {
                for (int j = 0; subFiles[j] != NULL; j++) {
                    files[i] = subFiles[j];
                    i++;
                }
                free(subFiles);
            }
        } else {
            char *dot = strrchr(entry->d_name, '.');
            if (dot && strcmp(dot, ".txt") == 0) {
                printf("Found text file: %s/%s\n", path, entry->d_name);

                files[i] = malloc(strlen(concate_str(path, entry->d_name)) + 1);
                strcpy(files[i], concate_str(path, entry->d_name));
                i++;
            }
        }
    }

    closedir(dir);
    *count = i;
    return files;
}