#include "ansi_colors.h"
#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct File {
    char *content;
    char *path;
    unsigned int size;
};

struct File open_entire_file(FILE *fp) {
    const int INITIAL_SIZE = 2048;
    struct File file;

    if (fp == NULL) {
        file.content = NULL;
        file.size = 0;
        return file;
    }

    unsigned int bytes_read = 0;
    unsigned int capacity = INITIAL_SIZE;
    file.content = malloc(capacity);
    file.size = 0;

    do {
        bytes_read = fread(file.content + file.size, 1, capacity - file.size, fp);
        file.size += bytes_read;

        if (file.size == capacity) {
            capacity += INITIAL_SIZE;
            file.content = realloc(file.content, capacity);
            if (file.content == NULL) {
                free(file.content);
                file.content = NULL;
                file.size = 0;
                return file;
            }
            for (unsigned int i = capacity - INITIAL_SIZE; i < capacity; i++) {
                file.content[i] = 0;
            }
        }
    } while (bytes_read > 0);

    if (ferror(fp)) {
        file.content = NULL;
        file.size = 0;
        return file;
    }

    return file;
}

/* Credits: https://stackoverflow.com/a/122974 */
char *trim_whitespace(char *str) {
    size_t len = 0;
    char *frontp = str;
    char *endp = NULL;

    if (str == NULL) {
        return NULL;
    }
    if (str[0] == '\0') {
        return str;
    }

    len = strlen(str);
    endp = str + len;

    while (isspace((unsigned char)*frontp)) {
        ++frontp;
    }
    if (endp != frontp) {
        while (isspace((unsigned char)*(--endp)) && endp != frontp) {
        }
    }

    if (frontp != str && endp == frontp) {
        *(isspace((unsigned char)*endp) ? str : (endp + 1)) = '\0';
    } else if (str + len - 1 != endp)
        *(endp + 1) = '\0';

    endp = str;
    if (frontp != str) {
        while (*frontp) {
            *endp++ = *frontp++;
        }
        *endp = '\0';
    }

    return str;
}

char peek_file(struct File *file, int cur, int k) { return file->content[cur + k]; }

char *ask(int size) {
    char *value = malloc(size * sizeof(char));

    fgets(value, size, stdin);
    value = trim_whitespace(value);
    return value;
}

int get_current_year() {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    return t->tm_year + 1900;
}

void process_file(struct File *file, char *file_path) {
    printf("INFO: processsing file '%s'\n", file_path);
    char *processed_file = malloc(file->size * sizeof(char) * 2);
    int pi = 0;
    int line_count = 1;

    for (unsigned int i = 0; i < file->size; i++) {
        char c = peek_file(file, i, 0);
        processed_file[pi] = c;
        pi++;

        if (c == '\n') {
            line_count++;
        }
        if (c != '<') continue;
        if (peek_file(file, i, 1) != '@') continue;
        i += 2;
        pi--;

        char key[1000] = {0};

        for (int j = 0;; j++) {
            c = peek_file(file, i, 0);

            if (c == '\0') {
                fprintf(stderr, "ERROR: reached EOF while parsing (from line %d).\n",
                        line_count);
                exit(EXIT_FAILURE);
            }
            if (c == '<') {
                fprintf(stderr,
                        "ERROR: Unterminated format specifier in the file (line %d).\n",
                        line_count);
                exit(EXIT_FAILURE);
            }
            if (c == '>') break;

            key[j] = c;

            i++;
        }
        char extra[300] = {0};
        char year[300] = {0};

        if (strcmp(key, "year") == 0) {
            int iyear = get_current_year();
            sprintf(year, "%d", iyear);
            char str[1000] = {0};
            sprintf(str,
                    RESET " (" RESET LIGHT_CYAN "leave it empty to put %s" RESET ")",
                    year);
            strcat(extra, str);
        }

        printf(LIGHT_RED "❆ " LIGHT_BLUE BOLD "%s%s: " LIGHT_YELLOW, key, extra);
        char *value = ask(1000);
        printf(RESET);

        if (strlen(value) == 0) {
            memset(value, 0, 1000);
            memmove(value, year, sizeof(char) * 300);
        }

        memmove(processed_file + pi, value, strlen(value) * sizeof(char));
        pi += strlen(value);

        free(value);
    }

    printf(LIGHT_RED "❆ " LIGHT_BLUE BOLD "filename " RESET "(" LIGHT_CYAN
                     "defaults to 'LICENSE'" RESET "): " LIGHT_YELLOW);
    char *path = ask(1000);
    if (strlen(path) == 0) {
        memset(path, 0, 1000);
        memmove(path, "LICENSE", sizeof(char) * 8);
    }

    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: can't write to file '%s'", path);
        exit(EXIT_FAILURE);
    }

    fprintf(fp, "%s", processed_file);

    fclose(fp);
    printf("INFO: Saved the file as '%s'\n", path);

    free(processed_file);
    free(path);
}
int main(int argc, char *argv[]) {
    char *license_dir = getenv("SPER_LICENSE_DIR");

    if (license_dir == NULL) {
        fprintf(stderr, "Environment variable SPER_LICENSE_DIR not found.\n");
        exit(EXIT_FAILURE);
    }

    if (argc == 1 || argc > 2) {
        fprintf(stderr, "Usage: sper [file]\n");
        fprintf(stderr, "note : [file] will be searched in '%s'\n", license_dir);
        exit(EXIT_FAILURE);
    }

    char file_path[2000] = {0};
    strcat(file_path, license_dir);
    strcat(file_path, "/");
    strcat(file_path, argv[1]);

    FILE *fp = fopen(file_path, "r");

    struct File file = open_entire_file(fp);

    if (file.content == NULL) {
        fprintf(stderr, "ERROR: file '%s' doesn't exist\n", file_path);
        exit(EXIT_FAILURE);
    }

    process_file(&file, file_path);
    free(file.content);
    fclose(fp);

    return 0;
}
