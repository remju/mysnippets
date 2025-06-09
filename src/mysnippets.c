#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define SNIPETS_STORE_PATH "$(HOME)/.mysnippets/"

// mysnippets commands enum
enum MYS_COMMAND {
    MYS_ERROR,
    MYS_SAVE_COMMAND,
    MYS_GET_COMMAND,
};

// Prints usage.
void mys_usage(const char *file)
{
    fprintf(stdout, "Usage: %s <command>\n", file);
}

// Get the provided command and check for its args.
int get_check_command(int argc, const char **argv)
{
    const char *command = argv[1];
    if (strcmp(command, "save") == 0)
    {
        if (argc < 4)
        {
            fprintf(stderr, "ERROR: Please provide a valid file and snippet\n\t%s save <file> '<snippet>'\n", argv[0]);
            return MYS_ERROR;
        }
        return MYS_SAVE_COMMAND;
    }
    else if (strcmp(command, "get") == 0)
    {
        if (argc < 3)
        {
            fprintf(stderr, "ERROR: Please provide a valid file name\n\t%s get <file> \n", argv[0]);
            return MYS_ERROR;
        }
        return MYS_GET_COMMAND;
    }
    else
    {
        fprintf(stderr, "Unexpected command: %s\n", command);
        return MYS_ERROR;
    }
}

// Get the size of a file.
off_t get_file_size(int fd) {
    struct stat st;
    if (fstat(fd, &st) == 0) {
        return st.st_size;
    } else {
        perror("fstat");
        return -1;
    }
}

// Get the mysnippets global save path.
char *get_mys_save_path()
{
    const char *home = getenv("HOME");
    if (!home) {
        fprintf(stderr, "HOME environment variable not set.\n");
        return NULL;
    }

    const char *mys_dir_name = "/.mysnippets";
    char *mys_dir_path = calloc(strlen(home) + strlen(mys_dir_name) + 1, 1);
    if (!mys_dir_path)
    {
        fprintf(stderr, "ERROR: malloc");
        return NULL;
    }
    sprintf(mys_dir_path, "%s%s", home, mys_dir_name);
    return mys_dir_path;
}

// Get the save path for a file.
char *get_mys_save_file_path(const char *file)
{
    char *mys_path = get_mys_save_path();
    if (!mys_path)
    {
        return NULL;
    }

    char *save_path = calloc(strlen(mys_path) + strlen(file) + 5, 1);
    if (!save_path)
    {
        fprintf(stderr, "ERROR: malloc");
        return NULL;
    }
    sprintf(save_path, "%s/%s", mys_path, file);
    free(mys_path);
    return save_path;
}

// Make directory "~/.mysnippets" if not exists
int mys_mkdir()
{
    char *mys_dir_path = get_mys_save_path();
    if (!mys_dir_path)
    {
        return 1;
    }

    if (access(mys_dir_path, F_OK) != 0) {
        if (mkdir(mys_dir_path, 0700) != 0) {
            perror("mkdir failed");
            return 1;
        }
    }
    free(mys_dir_path);
    return 0;
}

// Open a file for writing.
// Fails if file exists
int open_for_writing(const char *path)
{
    int fd = open(path, O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (fd < 0)
    {
        perror("open failed");
        return -1;
    }
    return fd;
}

// Open a file for reading.
// Fails if file does not exist
int open_for_reading(const char *path)
{
    int fd = open(path, O_RDONLY, 0600);
    if (fd < 0)
    {
        perror("open failed");
        return -1;
    }
    return fd;
}

// Save a snippet to a file.
// TODO: Add support for save directories.
//   Need to create subdirs before writing to file.
int mys_command_save(const char *file, const char *snippet)
{
    char *save_path = get_mys_save_file_path(file);
    if (!save_path) return 1;

    int fd = open_for_writing(save_path);
    free(save_path);
    if (fd < 0) return 1;

    size_t snippet_len = strlen(snippet);
    if (write(fd, snippet, snippet_len) != (ssize_t)snippet_len)
    {
        fprintf(stderr, "ERROR: write");
        return 1;
    }
    close(fd);
    return 0;
}

// Get a snippet.
int mys_command_get(const char *file)
{
    char *save_path = get_mys_save_file_path(file);
    if (!save_path) return 1;

    int fd = open_for_reading(save_path);
    free(save_path);
    if (fd < 0)
    {
        return 1;
    }

    off_t file_size = get_file_size(fd);
    if (file_size < 0)
    {
        fprintf(stderr, "ERROR: file size < 0\n");
        return 1;
    }

    char *file_buffer = calloc((size_t)file_size + 1, 1);
    if (!file_buffer)
    {
        fprintf(stderr, "ERROR: malloc");
        return 1;
    }

    if (read(fd, file_buffer, (size_t)file_size) != file_size)
    {
        fprintf(stderr, "ERROR: read");
        return 1;
    }
    puts(file_buffer);
    free(file_buffer);
    return 0;
}

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        mys_usage(argv[0]);
        return 0;
    }

    if (mys_mkdir()) return 1;

    int command = get_check_command(argc, argv);
    switch(command)
    {
    case MYS_SAVE_COMMAND:
        if (mys_command_save(argv[2], argv[3])) return 1;
        return 0;
    case MYS_GET_COMMAND:
        if (mys_command_get(argv[2])) return 1;
        return 0;
    case MYS_ERROR:
        return 1;
    }
    return 0;
}
