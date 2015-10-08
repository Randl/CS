//
// Created by Evgenii on 28.09.2015.
//

#include "external.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <stdint.h>

void print_arr(int A[], int n)
{
	int i;
	for (i = 0; i < n; i++)
		printf("%d\n", A[i]);
}

int compar(const void *p1, const void *p2)
{
	int *i1, *i2;

	i1 = (int *)p1;
	i2 = (int *)p2;

	return *i1 - *i2;
}

char *form_filename(int chunk, char *dir)
{
	char *path;
	int d;
	int dirlen, filelen, path_len;

	dirlen = strlen(dir);

	// File name is a chunk number, so
	// number of digits in chunk is a filename length
	filelen = 1;
	d = chunk;
	while (d)
	{
		d /= 10;
		filelen++;
	}

	// One for '/' and one for '\0'
	path_len = dirlen + filelen + 1 + 1;
	path = malloc(path_len);
	snprintf(path, path_len, "%s/%d", dir, chunk);

	return path;
}

int save_buf(char *buf, int n, char *dir, int chunk)
{
	FILE *f = NULL;
	int nwrite;
	char *path;
	int ret = 0;

	path = form_filename(chunk, dir);
	f = fopen(path, "wb");
	if (f == NULL) {
		perror("fopen");
		ret = -1;
		goto exit;
	}

	nwrite = fwrite(buf, sizeof(int64_t), n, f);
	if (nwrite != n) {
		perror("fwrite");
		ret = -1;
		goto exit;
	}

	exit:
	if (f) {
		fclose(f);
	}

	if (path) {
		free(path);
	}


	return ret;
}

// Return chunks number of -1 on error
int split(FILE *f, off_t filesize, char *dir, char *buf, size_t bufsize)
{
	int chunks;
	int n, nread;
	int i;

	n = bufsize / sizeof(int64_t);

	// XXX: we assume that filesize % bufsize == 0
	chunks = filesize / bufsize;
	for (i = 0; i < chunks; i++)
	{
		memset(buf, 0, bufsize);
		nread = fread(buf, sizeof(int64_t), n, f);

		if (nread < n) {
			//fprintf(stderr, "I/O error. Asked to read %d bytes, got %d\n", n, nread);
			return -1;
		}

		qsort(buf, nread, sizeof(int64_t), compar);
		save_buf(buf, nread, dir, i);
		//fprintf(stderr, "Saved chunk #%d of size %ld\n", i, nread * sizeof(int64_t));
	}

	return chunks;
}

int merge(char *dir, char *buf, size_t bufsize, int chunks, size_t offset)
{
	FILE *f;
	int slice;
	char *path;
	int buf_offset;
	int i, n;

	slice = bufsize / chunks;
	buf_offset = 0;

	// Each chunk has `chunk` number of slices.
	for (i = 0; i < chunks; i++)
	{
		path = form_filename(i, dir);
		//fprintf(stderr, "Chunk %s at offset %zu\n", path, offset);

		f = fopen(path, "rb");
		if (f == NULL) {
			perror("fopen");
			return -1;
		}
		fseek(f, offset, SEEK_SET);


		// Accumulate slices from each chunk in buffer.
		n = fread(buf + buf_offset, 1, slice, f);
		buf_offset += n;
		fclose(f);
	}

	// Thoughout this function we used buf as char array to use byte-addressing.
	// But now we need to act on ints inside that buffer, so we cast it to actual type.
	n = buf_offset / sizeof(int);
	qsort(buf, n, sizeof(int), compar);
	//print_arr((int *)buf, n);

	return 0;
}

int external_merge_sort(FILE *f, off_t filesize, char *dir, size_t bufsize)
{
	int chunks;
	char *buf;
	size_t chunk_offset;

	// XXX: Here is the only buffer available to us.
	buf = malloc(bufsize);
	if (!buf) {
		perror("malloc");
		return -ENOMEM;
	}

	// Phase 1: split file to sorted chunks of size bufsize.
	chunks = split(f, filesize, dir, buf, bufsize);
	if (chunks < 0) {
		free(buf);
		return -1;
	}

	// Phase 2: merge chunks.
	chunk_offset = 0;
	while (chunk_offset < bufsize)
	{
		//fprintf(stderr, "-> Merging chunks at offset %zu/%zu\n", chunk_offset, bufsize);
		merge(dir, buf, bufsize, chunks, chunk_offset);
		chunk_offset += bufsize / chunks;
	}

	free(buf);
	return 0;
}