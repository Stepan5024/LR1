#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#define LEN 200
#define MAX_FILES_PATH_LEN 1000
#define MAX_FILENAME_LEN 512
#define MAX_LINE_LEN 1024

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

char* concate_str(const char *path, const char *entry_name){
   

    // Вычисляем длины строк
    size_t path_len = strlen(path);
    size_t entry_len = strlen(entry_name);

    // Вычисляем длину результирующей строки
    size_t result_len = path_len + strlen("/") + entry_len;

    // Выделяем память под результирующую строку
    char *result = (char*) malloc(result_len + 1);

    // Проверяем, удалось ли выделить память
    if (result == NULL) {
        return NULL;
    }

    // Копируем первую строку в результирующую
    memcpy(result, path, path_len);

    // Добавляем разделитель в результирующую строку
    result[path_len] = '/';

    // Добавляем вторую строку в результирующую
    memcpy(result + path_len + 1, entry_name, entry_len);

    // Добавляем завершающий символ
    result[result_len] = '\0';

    // Выводим результат
    return result;
}

// Освобождаем память, которую выделили с помощью malloc
void free_concate_str(char *str) {
    free(str);
} 


// /home/bokar/Documents/LR2 LR2
// /home/bokar/Documents/LR1
char **findTextFiles(const char *path, int *count) {
    DIR *dir;
    struct dirent *entry;
    char **files = malloc(sizeof(char *) * MAX_FILES_PATH_LEN);
    int i = 0;
    char formatted_path[LEN];
    snprintf(formatted_path, sizeof(formatted_path), "%s", path);

    if (!(dir = opendir(formatted_path))) {
        fprintf(stderr, "Could not open directory: %s\n", formatted_path);
        exit(1);
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

                char *filename = concate_str(path, entry->d_name);
        
               

                files[i] = malloc(strlen(filename) + 1);
                strcpy(files[i], filename);
                free_concate_str(filename);
                i++;
            }
        }
    }

    closedir(dir);
    *count = i;
    files[i] = NULL;
    return files;
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
            fprintf(fp, "<<<<%s\n%s>>>>\n", files[i], content);
            free(content);
        }
    }

    fclose(fp);
}

int write_file(char* str, int len_chars, FILE* file){

    return fwrite(str, 1, len_chars, file);
}

bool isCorrectNameFile(const char* filename){
    
    char *suffix = ".txt";

    int str_len = strlen(filename);
    int suffix_len = strlen(suffix);

    if (str_len < suffix_len || strcmp(filename + str_len - suffix_len, suffix) != 0) {
        printf("Строка не заканчивается на .txt\n");
        printf("Укажите путь к архиву с расширением .txt\n");
        return false;
        //
    } 

    return true;

}
void my_split(const char* filename){
    ///home/bokar/Documents/GitHub/StudyMai/Учеба 6 сем/Операционные системы/ЛР
    //const char *filename = "/home/bokar/Documents/LR1/output.txt";
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Ошибка при открытии файла %s\n", filename);
        exit(1);
    }

    if(!isCorrectNameFile(filename)) {
        printf("Строка не заканчивается на .txt\n");
        printf("Укажите путь к архиву с расширением .txt\n");
        exit(1);
    }

    char line[MAX_LINE_LEN];
    char file_data[MAX_LINE_LEN];
    char new_filename[MAX_FILENAME_LEN];
    int in_file = 0;
    int line_num = 0;
    bool isCorrectStartFormat = false;
    bool isCorrectEndFormat = true;

    while (fgets(line, MAX_LINE_LEN, fp) != NULL) {
        line_num++;

        // Если встретили начальный разделитель
        if (strstr(line, "<<<<") != NULL && isCorrectEndFormat) {
            // Извлекаем имя нового файла из строки
            char *filename_start = strstr(line, "</") + 1;
            char *filename_end = strstr(line, "\n");
            int filename_len = filename_end - filename_start;
            strncpy(new_filename, filename_start, filename_len);
            new_filename[filename_len] = '\0';

            if(!isCorrectNameFile(new_filename)) {
                printf("Строка не заканчивается на .txt\n");
                printf("Укажите путь к архиву с расширением .txt\n");
                exit(1);
            }

            // Очищаем буфер для нового файла
            memset(file_data, 0, MAX_LINE_LEN);

            isCorrectStartFormat = true;
            isCorrectEndFormat = false;
            // Устанавливаем флаг, что мы находимся внутри файла
            in_file = 1;
        }
        // Если встретили конечный разделитель
        else if (strstr(line, ">>>>") != NULL && isCorrectStartFormat) {
            // Создаем новый файл и записываем в него данные

            FILE *file_ptr;
            printf("%s\n", new_filename);
            file_ptr = fopen(new_filename, "w+");
            
            int wrnum = write_file(file_data, strlen(file_data), file_ptr);
            if (wrnum == strlen(file_data)) {
                //printf("Запись \"%s\" произошла успешно\n", file_data);
            }
            else {
                printf("Данные не были записаны в полном объеме: \"%s\"", file_data);
            }
            // fwrite возвращает колво успешно записанных символов
            // Сбрасываем флаг, что мы находимся внутри файла
            in_file = 0;
            isCorrectStartFormat = false;
            isCorrectEndFormat = true;
        }
        // Если мы находимся внутри файла, то записываем данные в буфер
        else if (in_file) {
            strncat(file_data, line, MAX_LINE_LEN - strlen(file_data) - 1);
        }
        // В противном случае игнорируем строку
        else {
            printf("Игнорируем строку из-за битых тегов\n");
            continue;
        }
    }

    fclose(fp);
}

void archive(){
    char path[LEN];
    
    printf("Введите путь к директории архивации: ");
    fgets(path, LEN, stdin);
    path[strcspn(path, "\n")] = 0; // удаляем символ переноса строки
    //getchar(); // ждем нажатия Enter
    printf("Ваш путь: %s\n", path);
    ///home/bokar/Documents/LR2 LR2
    //const char *path = "/home/bokar/Documents/LR1"; // путь архивации директориии
    int count = 0;
    char ** files = findTextFiles(path, &count);

    if (count > 0) {

        char *result = concate_str(path, "output.txt");
        mergeTextFiles(files, count, result); // путь названия архива
        free_concate_str(result);
    } else {
        printf("В директории не найдено текстовых файлов %s\n", path);
    }

    for (int i = 0; i < count; i++) {
        printf("%s\n", files[i]);
        free(files[i]);
    }
    free(files);

}
int main() {

    int menu;
    printf("1: Заархивируйте директорию, указав путь в программе\n");
    printf("2: Разархивировать, введя путь к файлу архива с помощью клавиатуры\n");
    scanf("%d", &menu);
    // очищаем буфер ввода
    while (getchar() != '\n');
    switch (menu) {
        case 1:
            printf("Твой выбор архивировать директорию\n");
            archive();
            break;
        case 2:
            printf("Твой выбор разархивировать директорию\n");
            //const char *path = "/home/bokar/Documents/LR1/output.txt"; // путь разархивации
    
            printf("Введите путь к файлу архива, например /home/bokar/Documents/LR1/output.txt: ");
            char path[LEN];
    
            fgets(path, LEN, stdin);
            path[strcspn(path, "\n")] = 0; // удаляем символ переноса строки
    
            printf("Ваш путь: %s\n", path);

            my_split(path);

            break;
        default:
            printf("ERROR! Wrong menu item selected");
            return 0;
    }

    

    


    return 0;
}

