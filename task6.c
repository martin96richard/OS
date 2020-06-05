#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

void copy_read_write(int fd_from, int fd_to) {
    char c[1];

    while (read(fd_from, &c, 1) > 0) {
        write(fd_to, c, 1);
    }
}

void copy_mmap(int fd_from, int fd_to) {
    char *src, *dest;
//    struct stat s;
    int size;

    /* SOURCE */
 //   fstat(fd_from, &s); // st_size = blocksize
    size = lseek(fd_from, 0, SEEK_END);

    src = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd_from, 0); //map the file [fd_from] for reading of size [size] and return the memory adr [srd]

    /* DESTINATION */
    ftruncate(fd_to, size); //Fill the file [fd] to the length of [size] octets

    dest = mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd_to, 0); //map the file [fd] for writing of size [size] and return memory adr [dest]

    /* COPY */
    memcpy(dest, src, size);

    munmap(src, size);
    munmap(dest, size);
}

int main(int argc, char **argv) {
    char rtn = 0;
    int fd_from;
    int fd_to;

    if ((rtn = getopt(argc, argv, "mh")) != -1) {
        switch (rtn) {
            case 'm':
                if (argc == 4) { //call mmap fct
                    if ((fd_from = open(argv[2], O_RDONLY)) != -1) {
                        if ((fd_to = open(argv[3], O_CREAT | O_RDWR, 0666)) != -1) {
                            copy_mmap(fd_from, fd_to);
                            close(fd_to);
                        } else {
                            fprintf(stderr, "ERROR: %s: Cant create file.\n", argv[3]);
                            return (EXIT_FAILURE);
                        }
                        close(fd_from);
                    } else {
                        fprintf(stderr, "ERROR: %s: File not found.\n", argv[2]);
                        return (EXIT_FAILURE);
                    }
                } else
                    fprintf(stderr, "ERROR: Too many or too few arguments.\n");
                break;
            case 'h':
                printf("Usage:\n\tcopy [-m] <file_name> <new_file_name>\n\tcopy [-h]\n");
                break;
            default:
                break;
        }
    } else {
        if (argc == 3) { //call read & write fct
            if ((fd_from = open(argv[1], O_RDONLY)) != -1) {
                if ((fd_to = open(argv[2], O_CREAT | O_RDWR, 0666)) != -1) {
                    copy_read_write(fd_from, fd_to);
                    close(fd_to);
                } else {
                    fprintf(stderr, "ERROR: %s: Cant create file.\n", argv[2]);
                    return (EXIT_FAILURE);
                }
                close(fd_from);
            } else {
                fprintf(stderr, "ERROR: %s: File not found.\n", argv[1]);
                return (EXIT_FAILURE);
            }
        } else
            fprintf(stderr, "ERROR: Too many or too few arguments.\n");
    }

    return (EXIT_SUCCESS);
}