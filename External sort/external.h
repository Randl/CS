//
// Created by Evgenii on 28.09.2015.
//

#ifndef EXTERNAL_SORT_EXTERNAL_H
#define EXTERNAL_SORT_EXTERNAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

void print_arr(int A[], int n);

int compar(const void *p1, const void *p2);

char *form_filename(int chunk, char *dir);

int save_buf(char *buf, int n, char *dir, int chunk);
int split(FILE *f, off_t filesize, char *dir, char *buf, size_t bufsize);

int merge(char *dir, char *buf, size_t bufsize, int chunks, size_t offset);

int external_merge_sort(FILE *f, off_t filesize, char *dir, size_t bufsize);


#endif //EXTERNAL_SORT_EXTERNAL_H
