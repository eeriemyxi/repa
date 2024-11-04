#include "ansi_colors.h"
#include "hashmap.h"
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wchar.h>

static const int CWD_FILE_PATH_SIZE = 5000;
static const int MAX_WORD_SIZE = 5000;

static struct Options_S {
    int APPEND_CWD;
    int TOP_X;
    char *CWD;
} Options;

struct Result {
    unsigned int line_counter;
    unsigned int space_counter;
    unsigned int raw_sp_counter;
    unsigned int raw_nl_counter;
};

struct File {
    char *content;
    char *path;
    unsigned int size;
};

char *_get_cwd() {
    char *cwd = malloc(sizeof(char) * CWD_FILE_PATH_SIZE);

    if (getcwd(cwd, sizeof(char) * CWD_FILE_PATH_SIZE) == NULL) {
        fprintf(stderr, "ERROR: couldn't get working directory\n");
        free(cwd);
        exit(1);
    }

    return cwd;
}

void initialize_options(struct Options_S *options) {
    options->APPEND_CWD = 0;
    options->TOP_X = 10;
    options->CWD = _get_cwd();
}

void initialize_result(struct Result *result) {
    result->line_counter = 0;
    result->space_counter = 0;
    result->raw_sp_counter = 0;
    result->raw_nl_counter = 0;
}

void print_usage(char *program, FILE *file) {
    fprintf(file, LIGHT_YELLOW "Usage: %s [-h] [-t n] [file...]\n" RESET, program);
    fprintf(file, LIGHT_BLUE "note: [file] can be - to read stdin \n" RESET);
    fprintf(file, "help: -h   -- show this message.\n");
    fprintf(file, "help: -a   -- don't append working directory when finding files.\n");
    fprintf(file, "help: -t N -- show top N results. can be 0.\n");
}

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

int _compare_pair(const void *const p1, const void *const p2) {
    int a = ((Node *)p1)->value;
    int b = ((Node *)p2)->value;
    return b - a;
}

void print_frequent_words(HashMap *map) {
    // FIXME: optimise this mess

    Node *freqs = malloc(map->capacity * sizeof(Node));
    memcpy(freqs, map->nodes, map->capacity);

    qsort(freqs, map->length, sizeof(Node), _compare_pair);
    char *padding = "    ";
    int biggest = 0;

    for (int i = 0; i < Options.TOP_X; i++) {
        if (freqs[i].key == NULL) {
            continue;
        }
        int len = strlen(freqs[i].key);
        if (len > biggest)
            biggest = len;
    }

    printf("🔸" LIGHT_PINK "Sorted Words by Frequency (" BOLD LIGHT_YELLOW
           "Top %d" LIGHT_PINK "):\n" RESET,
           Options.TOP_X);

    for (int i = 0; i < Options.TOP_X; i++) {
        if (freqs[i].key == NULL) {
            continue;
        }
        char *padding2 = malloc(sizeof(char) * (biggest + 1));
        int _s = strlen(freqs[i].key);
        for (int i = 0; i < biggest - _s; i++)
            padding2[i] = ' ';
        padding2[biggest - _s] = '\0';

        printf(LIGHT_GREEN "%s%03d." BOLD LIGHT_YELLOW " %s%s  " RESET LIGHT_BLUE
                           "%d\n" RESET,
               padding, i + 1, freqs[i].key, padding2, freqs[i].value);

        free(padding2);
    }

    free(freqs);
}

void process_file(struct File *file) {
    HashMap *map = HT_make_hash_map();

    struct Result result;
    initialize_result(&result);

    char word[MAX_WORD_SIZE];
    size_t wi = 0;
    bool add_word = false;

    for (unsigned int i = 0; i < file->size; i++) {
        char ch = file->content[i];

        if (ch == '\n') {
            result.line_counter++;
            add_word = true;

            while (file->content[i + 1] == '\n') {
                i += sizeof(char);
                result.raw_nl_counter++;
            }

        } else if (ch == ' ') {
            result.space_counter++;
            add_word = true;

            while (file->content[i + 1] == ' ') {
                i += sizeof(char);
                result.raw_sp_counter++;
            }

        } else {
            word[wi] = ch;
            wi += 1;
            if (wi >= (size_t) MAX_WORD_SIZE) {
                fprintf(stderr, "ERROR: Max word length reached.\n");
                return;
            }
        }

        if (add_word == true) {
            word[wi + 1] = '\0';
            if (wi > 0) {
                int freq = HT_get(map, word);

                if (freq == -1) {
                    HT_insert(map, word, 1);
                } else if (freq > 0) {
                    HT_insert(map, word, freq + 1);
                }
            }

            memset(word, 0, 1000 * sizeof(char));
            wi = 0;
            add_word = false;
        }
    }

    result.raw_sp_counter += result.space_counter;
    result.raw_nl_counter += result.line_counter;

    printf("🔹" LIGHT_GREEN "Total Lines:  " LIGHT_CYAN "%d " RESET "(" LIGHT_BLUE "%d" RESET ")\n",
           result.raw_nl_counter, result.line_counter);
    printf("🔹" LIGHT_GREEN "Total Spaces: " LIGHT_CYAN "%d " RESET "(" LIGHT_BLUE "%d" RESET
           ")\n",
           result.raw_sp_counter, result.space_counter);

    if (Options.TOP_X) print_frequent_words(map);

    printf(LIGHT_YELLOW "File" RESET ": " BOLD LIGHT_PINK "%s\n" RESET, file->path);

    HT_free_hash_map(map);
}

int main(int argc, char *argv[]) {
    int opt;
    initialize_options(&Options);

    setlocale(LC_ALL, "en_US.UTF-8");

    while ((opt = getopt(argc, argv, "hat:")) != -1) {
        switch (opt) {
        case 'h':
            print_usage(argv[0], stdout);
            exit(0);
            break;
        case 'a':
            Options.APPEND_CWD = true;
            break;
        case 't':
            Options.TOP_X = atoi(optarg);
            break;
        default:
            print_usage(argv[0], stderr);
            exit(1);
        }
    }

    if (argc == 1) { // when no args are provided
        print_usage(argv[0], stderr);
        exit(1);
    }

    for (int i = 0;; i++) {
        char file_path[CWD_FILE_PATH_SIZE];
        memset(file_path, 0, CWD_FILE_PATH_SIZE * sizeof(char));
        if (argv[optind + i] == NULL) {
            break;
        }
        if (strcmp(argv[optind + i], "-") == 0) {
            struct File file = open_entire_file(stdin);
            file.path = "(stdin)";
            process_file(&file);
            continue;
        }

        if (Options.APPEND_CWD == true) {
            strcat(file_path, Options.CWD);
            strcat(file_path, "/");
        }
        strcat(file_path, argv[optind + i]);

        FILE *fp = fopen(file_path, "r");
        struct File file = open_entire_file(fp);
        file.path = file_path;
        if (file.content == NULL) {
            fprintf(stderr, "ERROR: can't open file: %s\n", file_path);
            fprintf(stderr, "INFO: trying next file (if any)...\n");
            continue;
        }

        process_file(&file);
        if (optind + i < argc - 1) printf("\n");

        memset(file_path, 0, sizeof(file_path));
        free(file.content);
        fclose(fp);
    }

    free(Options.CWD);
    return 0;
}

// TODO: Fix valgrind errors
