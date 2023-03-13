#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#define LEN 200
#define MAX_FILES_PATH_LEN 2000
#define MAX_FILENAME_LEN 512
#define MAX_LINE_LEN 1024

/**
 * Returns file permissions in mode_t format
 * https://man7.org/linux/man-pages/man2/lstat.2.html
*/
int filePermissionsCStr(char const *path) {
    struct stat stats;
    if(stat(path, &stats) < 0) {
        printf("filePermissions error: Cannot read file properties\n");
        exit(1);
    }
    return stats.st_mode;
}

/**
 * Overrides file permissions in mode_t format
 * https://man7.org/linux/man-pages/man2/lstat.2.html
*/
void fileSetPermissionsCStr(char const *path, char permissions) {
    if(chmod(path, permissions) != 0) {
         printf("fileSetPermissionsCStr error: Cannot assign permission to selected file\n");
        exit(1);
        
    }
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
    //  директория, указанная в аргументе path
    DIR *dir;
    // Структура, содержащая информацию о текущей записи в директории
    struct dirent *entry;
    // Выделяем память для массива, содержащего имена файлов
    char **files = malloc(sizeof(char *) * MAX_FILES_PATH_LEN);
    // Инициализируем счетчик файлов
    int i = 0;
    // Создаем буфер для форматирования пути
    char formatted_path[LEN];
    // Копируем путь в буфер
    snprintf(formatted_path, sizeof(formatted_path), "%s", path);
    // Если не удалось открыть директорию, выводим сообщение об ошибке и завершаем программу
    if (!(dir = opendir(formatted_path))) {
        fprintf(stderr, "Could not open directory: %s\n", formatted_path);
        exit(1);
    }
    // Читаем записи из директории, пока они не закончатся
    while ((entry = readdir(dir)) != NULL) {
        // Если текущая запись - директория, то рекурсивно вызываем функцию findTextFiles
        if (entry->d_type == DT_DIR) {
            // Создаем новый путь, добавляя к текущему пути имя директории
            char newPath[1024];
             // Если имя директории "." или "..", то пропускаем ее
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }
             // Форматируем новый путь
            snprintf(newPath, sizeof(newPath), "%s/%s", path, entry->d_name);
            // Рекурсивно вызываем функцию findTextFiles для нового пути
            char **subFiles = findTextFiles(newPath, count);
            // Если возвращенный массив не пустой, то добавляем его элементы в основной массив
            if (subFiles != NULL) {
                for (int j = 0; subFiles[j] != NULL; j++) {
                    files[i] = subFiles[j];
                    i++;
                }
                free(subFiles);
            }
        } else {
            // Функция strrchr находит последнее вхождение в str
            char *dot = strrchr(entry->d_name, '.');
            //strcmp функция сравнивает посимвольно строки, переданные ей в качестве аргументов
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
        int permissionCode = filePermissionsCStr(files[i]);
        if (content) {
            fprintf(fp, "<<<<%s|||%d|||\n%s\n>>>>\n", files[i], permissionCode, content);
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
    // Сравниваем подстроку имени файла, начиная с позиции (длина имени - длина суффикса)
    // и до конца имени, с переданным суффиксом
    // strcmp сравнение строк
    if (str_len < suffix_len || strcmp(filename + str_len - suffix_len, suffix) != 0) {
        return false;
    } 

    return true;

}
// Функция для проверки существования директории по переданному пути
bool directoryExists(const char *path) {
    struct stat info;
    // Проверяем существование файла или каталога по переданному пути
    // и получаем информацию о нем с помощью функции stat()
    if (stat(path, &info) != 0) {
         // Если файл или каталог не существует, возвращаем false
        return false;
    }
    // Проверяем, является ли объект по указанному пути каталогом
    // с помощью макроса S_IFDIR и операции побитового И (&)
    return (info.st_mode & S_IFDIR) != 0;
}

// Функция для создания директорий по переданному пути, если они отсутствуют

bool createDirectories(const char *rootPath, const char *path) {
    // Создаем копию переданного пути
    char *pathCopy = strdup(path);
    // Инициализируем указатель на первый каталог в пути
    // используя функцию strtok() для разделения строки по символу "/"
    char *dirName = strtok(pathCopy, "/");
    char *subDirPath = NULL;
    bool success = true;

    // Добавляем корневой путь к переданному пути
    char *fullPath = (char *) malloc(strlen(rootPath) + strlen(path) + 2);
    if (fullPath == NULL) {
        free(pathCopy);
        return false; // или какое-то другое действие при ошибке
    }
    strcpy(fullPath, rootPath);
    strcat(fullPath, "/");
    strcat(fullPath, path);

    while (dirName != NULL) {
        // Если subDirPath еще не инициализирован, то создаем копию первого каталога
        // и сохраняем ее в subDirPath
        if (subDirPath == NULL) {
            subDirPath = strdup(dirName);
        } else {
            char *newSubDirPath = (char *) realloc(subDirPath, strlen(subDirPath) + strlen(dirName) + 2);
            if (newSubDirPath == NULL) {
                free(pathCopy);
                free(subDirPath);
                free(fullPath);
                return false; // или какое-то другое действие при ошибке
            }
            subDirPath = newSubDirPath;
            strcat(subDirPath, "/");
            strcat(subDirPath, dirName);
        }

        // Добавляем корневой путь к текущему пути
        char *fullSubPath = (char *) malloc(strlen(rootPath) + strlen(subDirPath) + 2);
        if (fullSubPath == NULL) {
            free(pathCopy);
            free(subDirPath);
            free(fullPath);
            return false; // или какое-то другое действие при ошибке
        }
        strcpy(fullSubPath, rootPath);
        strcat(fullSubPath, "/");
        strcat(fullSubPath, subDirPath);

        if (!directoryExists(fullSubPath)) {
            if (mkdir(fullSubPath, 0777) != 0) {
                success = false;
                free(pathCopy);
                free(subDirPath);
                free(fullPath);
                free(fullSubPath);
                return success;
            }
        }

        free(fullSubPath);
        dirName = strtok(NULL, "/");
    }

    free(pathCopy);
    free(subDirPath);
    free(fullPath);
    return success;
}



char* getFilePath(const char* filename) {
    char* path = NULL;
    char* lastSlash = strrchr(filename, '/');
    if (lastSlash != NULL) {
        int pathLen = lastSlash - filename + 1;
        path = (char*) malloc(pathLen + 1);
        strncpy(path, filename, pathLen);
        path[pathLen] = '\0';
    }
    return path;
}
// Функция для разбиения файла на несколько файлов
void my_split(const char* filename){
    // Открываем файл для чтения
    FILE *fp = fopen(filename, "r");
    // Если не удалось открыть файл, выводим сообщение об ошибке и завершаем работу программы
    if (fp == NULL) {
        printf("Ошибка при открытии файла %s\n", filename);
        exit(1);
    }
    printf("file name = %s\n", filename);

   
    // Проверяем, что имя файла заканчивается на .txt
    if(!isCorrectNameFile(filename)) {
        printf("Строка не заканчивается на .txt\n");
        printf("Укажите путь к архиву с расширением .txt\n");
        exit(1);
    }

    // Объявляем переменные
    char line[MAX_LINE_LEN];
    char file_data[MAX_LINE_LEN];
    char new_filename[MAX_FILENAME_LEN];
    int in_file = 0;
    int line_num = 0;
    bool isCorrectStartFormat = false;
    bool isCorrectEndFormat = true;
    char permissionCode[10];

    // Читаем файл построчно
    while (fgets(line, MAX_LINE_LEN, fp) != NULL) {
        line_num++;

        // Если встретился начальный разделитель
        if (strstr(line, "<<<<") != NULL && isCorrectEndFormat) {
            // Извлекаем код разрешений из строки
            char sep[] = "|||";
            char *permission_start = strstr(line, sep) + 3;
            char *permission_end = strstr(permission_start, sep);
            if (permission_end != NULL) {
                int permission_len = permission_end - permission_start;
                char permission_str[permission_len + 1];
                strncpy(permission_str, permission_start, permission_len);
                permission_str[permission_len] = '\0';
                int code = atoi(permission_str);
                sprintf(permissionCode, "%d", code);
            }

            // Извлекаем имя нового файла из строки
            char *filename_start = strstr(line, "</") + 1;
            char *filename_end = permission_start - strlen(sep);
            int filename_len = filename_end - filename_start;
            strncpy(new_filename, filename_start, filename_len);
            new_filename[filename_len] = '\0';

            // Проверяем, что имя файла заканчивается на .txt
            if(!isCorrectNameFile(new_filename)) {
                printf("Строка не заканчивается на .txt\n");
                printf("Укажите путь к архиву с расширением .txt\n");
                exit(1);
            } else {
                printf("code %s\n", permissionCode);
                //fileSetPermissionsCStr(new_filename, *permissionCode);
            }
            // Очищаем буфер для нового файла
            memset(file_data, 0, MAX_LINE_LEN);

            isCorrectStartFormat = true;
            isCorrectEndFormat = false;
            // Устанавливаем флаг, что мы находимся внутри файла
            in_file = 1;
        }
        // Если встретился конечный разделитель
        else if (strstr(line, ">>>>") != NULL && isCorrectStartFormat) {
            // Создаем новый файл и записываем в него данные
            FILE *file_ptr;

            printf("%s\n", new_filename);
            char* path = getFilePath(new_filename);
            if (path != NULL) {
                printf("Путь до файла: %s\n", path);
                createDirectories("", path);
                free(path);
            } else {
                 printf("Файл не содержит пути.\n");
            }

            file_ptr = fopen(new_filename, "w+");
            int wrnum = write_file(file_data, strlen(file_data), file_ptr);
            if (wrnum == strlen(file_data)) {
                //printf("Запись \"%s\" произошла успешно\n", file_data);
            }
            else {
                printf("Данные не были записаны в полном объеме: \"%s\"", file_data);
            }
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

    // Закрываем файл
    fclose(fp);
}

void removeArchiveFile(const char* path) {
    // Проверяем существование файла
    if (remove(path) == 0) {
        printf("Файл %s существующего архива .\n", path);
    } else {
        printf("Could not remove file %s.\n", path);
    }
}

void archive(){
    char path[LEN];
    
    printf("Введите путь к директории архивации: ");
    fgets(path, LEN, stdin);
    path[strcspn(path, "\n")] = 0; // удаляем символ переноса строки
    printf("Ваш путь: %s\n", path);
    ///home/bokar/Documents/LR2 LR2
    //const char *path = "/home/bokar/Documents/LR1"; // путь архивации директориии
    int count = 0;
    char *result = concate_str(path, "output.txt");
    removeArchiveFile(result);
    free_concate_str(result);
    
    char ** files = findTextFiles(path, &count);

    if (count > 0) {

        char *result = concate_str(path, "output.txt");
        mergeTextFiles(files, count, result); // путь названия архива
        free_concate_str(result);
    } else {
        printf("В директории не найдено текстовых файлов %s\n", path);
    }

    for (int i = 0; i < count; i++) {
        printf("Заархивирован %s\n", files[i]);
        free(files[i]);
    }
    free(files);

}
int main() {
// /home/bokar/Documents/LR1/
// /home/bokar/Documents/LR1/output.txt
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
            printf("Введите путь к файлу архива, например /home/bokar/Documents/LR1/output.txt: ");
            char path[LEN];
    
            fgets(path, LEN, stdin);
            path[strcspn(path, "\n")] = 0; // удаляем символ переноса строки
    
            printf("Ваш путь: %s\n", path);
            my_split(path);

            break;
        default:
            printf("Ошибка! Неправильно выбранный пункт меню");
            return 0;
    }

    return 0;
}
