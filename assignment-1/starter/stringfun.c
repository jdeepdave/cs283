#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int setup_buff(char *, char *, int);
int count_words(char *, int, int);

// Helper prototypes
void reverse_string(char *, int);
void print_words(char *, int);

int setup_buff(char *buff, char *user_str, int len) {
    if (!buff || !user_str) return -2; // Null pointer error

    int i = 0, j = 0;
    char last_char = 0;
    while (user_str[i] != '\0' && j < len) {
        if (user_str[i] != ' ' && user_str[i] != '\t') {
            buff[j++] = user_str[i];
            last_char = user_str[i];
        } else if (last_char != ' ') {
            buff[j++] = ' ';
            last_char = ' ';
        }
        i++;
    }

    if (user_str[i] != '\0') return -1; // Input string too large

    while (j < len) {
        buff[j++] = '.';
    }

    return i; // Length of user string
}

void reverse_string(char *buff, int user_len) {
    printf("Reversed String: ");
    for (int i = user_len - 1; i >= 0; i--) {
        putchar(buff[i]);
    }
    putchar('\n');
}

void print_words(char *buff, int user_len) {
    printf("Word Print\n----------\n");
    int word_start = 0, word_len = 0, word_count = 1;

    for (int i = 0; i < user_len; i++) {
        if (buff[i] != ' ') {
            if (word_len == 0) word_start = i;
            word_len++;
        } else if (word_len > 0) {
            printf("%d. ", word_count++);
            fwrite(&buff[word_start], sizeof(char), word_len, stdout);
            printf(" (%d)\n", word_len);
            word_len = 0;
        }
    }

    if (word_len > 0) {
        printf("%d. ", word_count);
        fwrite(&buff[word_start], sizeof(char), word_len, stdout);
        printf(" (%d)\n", word_len);
    }
}

int count_words(char *buff, int len, int str_len) {
    if (!buff) return -2; // Null pointer error

    int count = 0, in_word = 0;

    for (int i = 0; i < str_len; i++) {
        if (buff[i] != ' ') {
            if (!in_word) {
                in_word = 1;
                count++;
            }
        } else {
            in_word = 0;
        }
    }

    return count;
}

void usage(char *exename) {
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);
}

int main(int argc, char *argv[]) {
    char *buff;             // Placeholder for the internal buffer
    char *input_string;     // Holds the string provided by the user on cmd line
    char opt;               // Used to capture user option from cmd line
    int rc;                 // Used for return codes
    int user_str_len;       // Length of user-supplied string

    // TODO: #1 This is safe because argc ensures argv[1] exists if argc >= 2.
    if ((argc < 2) || (*argv[1] != '-')) {
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1] + 1); // Get the option flag

    if (opt == 'h') {
        usage(argv[0]);
        exit(0);
    }

    // TODO: #2 Ensures a string argument exists for processing.
    if (argc < 3) {
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; // Capture the user input string

    // TODO: #3 Allocate space for the buffer and handle malloc errors.
    buff = (char *)malloc(BUFFER_SZ);
    if (!buff) {
        fprintf(stderr, "Error: Failed to allocate memory for buffer.\n");
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);
    if (user_str_len < 0) {
        printf("Error setting up buffer, error = %d\n", user_str_len);
        free(buff);
        exit(2);
    }

    switch (opt) {
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);
            if (rc < 0) {
                printf("Error counting words, rc = %d\n", rc);
                free(buff);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        case 'r':
            reverse_string(buff, user_str_len);
            break;

        case 'w':
            print_words(buff, user_str_len);
            break;

        case 'x':
            if (argc != 5) {
                printf("Error: -x flag requires 3 arguments.\n");
                free(buff);
                exit(2);
            }
            printf("Not Implemented!\n");
            break;

        default:
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    // TODO: #6 Free allocated buffer before exiting.
    print_buff(buff, BUFFER_SZ);
    free(buff);
    exit(0);
}
