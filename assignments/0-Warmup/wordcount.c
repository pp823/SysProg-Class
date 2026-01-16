#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

/*
 * Print correct usage information for the program.
 * This is displayed when an invalid option is provided.
 */
void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s [-l] [-w] [-c] [file ...]\n", program_name);
    fprintf(stderr, "Count lines, words, and characters in files or stdin\n");
    fprintf(stderr, "  -l    count lines\n");
    fprintf(stderr, "  -w    count words\n");
    fprintf(stderr, "  -c    count characters\n");
    fprintf(stderr, "  If no options specified, counts all three\n");
    fprintf(stderr, "  If no files specified, reads from stdin\n");
}

/*
 * Structure used to store counts of lines, words, and characters.
 */
typedef struct {
    long lines;
    long words;
    long chars;
} Counts;

/*
 * Count lines, words, and characters from a given input stream.
 * This function works for both stdin and file streams.
 */
Counts count_stream(FILE *fp) {
    Counts counts = {0, 0, 0};
    int c;
    bool in_word = false;

    while ((c = fgetc(fp)) != EOF) {
        counts.chars++;

        /* Count lines */
        if (c == '\n') {
            counts.lines++;
        }

        /* Count words using whitespace detection */
        if (isspace(c)) {
            in_word = false;
        } else if (!in_word) {
            in_word = true;
            counts.words++;
        }
    }

    return counts;
}

/*
 * Print selected counts based on command-line options.
 * If filename is not NULL, it is printed after the counts.
 */
void print_counts(Counts counts,
                  bool show_lines,
                  bool show_words,
                  bool show_chars,
                  const char *filename) {

    if (show_lines) {
        printf("%8ld", counts.lines);
    }
    if (show_words) {
        printf("%8ld", counts.words);
    }
    if (show_chars) {
        printf("%8ld", counts.chars);
    }
    if (filename) {
        printf(" %s", filename);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    bool show_lines = false;
    bool show_words = false;
    bool show_chars = false;
    bool any_option = false;

    int file_start = 1;

    for (int i = 1; i < argc && argv[i][0] == '-'; i++) {
        if (strcmp(argv[i], "-l") == 0) {
            show_lines = true;
            any_option = true;
        } else if (strcmp(argv[i], "-w") == 0) {
            show_words = true;
            any_option = true;
        } else if (strcmp(argv[i], "-c") == 0) {
            show_chars = true;
            any_option = true;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
        file_start = i + 1;
    }

    /*
     * If no options are specified, display all counts.
     */
    if (!any_option) {
        show_lines = show_words = show_chars = true;
    }

    /*
     * If no files are specified, read from standard input.
     */
    if (file_start >= argc) {
        Counts counts = count_stream(stdin);
        print_counts(counts, show_lines, show_words, show_chars, NULL);
        return 0;
    }

    /*
     * Process each file provided as an argument.
     */
    Counts total = {0, 0, 0};
    int num_files = 0;

    for (int i = file_start; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (!fp) {
            fprintf(stderr, "Error: cannot open file '%s'\n", argv[i]);
            return 1;
        }

        Counts counts = count_stream(fp);
        fclose(fp);

        print_counts(counts, show_lines, show_words, show_chars, argv[i]);

        total.lines += counts.lines;
        total.words += counts.words;
        total.chars += counts.chars;
        num_files++;
    }

    /*
     * If more than one file was processed, print total counts.
     */
    if (num_files > 1) {
        print_counts(total, show_lines, show_words, show_chars, "total");
    }

    return 0;
}
