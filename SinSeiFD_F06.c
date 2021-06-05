#define FUSE_USE_VERSION 28

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

static const char *dirPath = "/home/zoniacer/Downloads";

char *en1 = "AtoZ_";

int x = 0;


int ext_id(char *path)
{
	int i;
	
	
	for (i = strlen(path) - 1; i >= 0; i--){
		if (path[i] == '.')
			return i;
	}

	return strlen(path);
}

int slash_id(char *path, int mentok)
{
	int i;

	for (i = 0; i < strlen(path); i++)
	{
		if (path[i] == '/')
			return i + 1;
	}

	return mentok;
}

void enc(char *path)
{
	if (!strcmp(path, ".") || !strcmp(path, ".."))
		return;

	printf("encrypt path: %s\n", path);

	int i;

	int endid = ext_id(path);
	int startid = slash_id(path, 0);
	
	for(i = startid; i < endid; i++) {
		if (path[i] != '/'){
			if (path[i] >= 'A' && path[i] <= 'Z') {
			    path[i] = 'Z' - (path[i] - 'A') ;
			}
			else if (path[i] >= 'a' && path[i] <= 'z') {
			    path[i] = 'z' - (path[i] - 'a') ;
			}
		}
		
	}
	
}

void dec(char *path) 
{
	if (!strcmp(path, ".") || !strcmp(path, ".."))
		return;

	printf("encrypt path: %s\n", path);

	int i;

	int endid = ext_id(path);
	int startid = slash_id(path, endid);
	
	for(i = startid; i < endid; i++) {
		if (path[i] != '/'){
			if (path[i] >= 'A' && path[i] <= 'Z') {
			    path[i] = 'Z' - (path[i] - 'A') ;
			}
			else if (path[i] >= 'a' && path[i] <= 'z') {
			    path[i] = 'z' - (path[i] - 'a') ;
			}
		}
		
	}
	
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char fpath[1000];

	char *a = strstr(path, en1);
	if (a != NULL)
		dec(a);

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = lstat(fpath, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
					   off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];

	char *a = strstr(path, en1);
	if (a != NULL)
		dec(a);

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	
	int res = 0;
	DIR *dp;
	struct dirent *de;

	(void)offset;
	(void)fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) //read whats inside dirpath (level 1 only)
	{						// de->d_name is file name + ext
		if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
			continue;

		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;


		// printf("before: %s\n", de->d_name);
		printf("a: %s\n", a);
		if (a != NULL)
			enc(de->d_name);
		// printf("after: %s\n", de->d_name);

		res = (filler(buf, de->d_name, &st, 0));

		if (res != 0)
			break;
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
					struct fuse_file_info *fi)
{
	char fpath[1000];
	bzero(fpath, 1000);
	char *a = strstr(path, en1);
	if (a != NULL)
		dec(a);

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	int res = 0;
	int fd = 0;

	(void)fi;

	fd = open(fpath, O_RDONLY);


	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;
	char fpath[1000];

	char *a = strstr(path, en1);

	if (a != NULL){
		dec(a);
	}

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = mkdir(fpath, mode);


	if (res == -1)
		return -errno;

	return 0;
}



static int xmp_rename(const char *from, const char *to)
{
	int res;
	char frompath[1000], topath[1000];

	char *a = strstr(to, en1);

	if (a != NULL)
		dec(a);

	sprintf(frompath, "%s%s", dirPath, from);
	sprintf(topath, "%s%s", dirPath, to);

	res = rename(frompath, topath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	(void)fi;

	int res;
	char fpath[1000];

	char *a = strstr(path, en1);
	if (a != NULL)
		dec(a);

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = creat(fpath, mode);


	if (res == -1)
		return -errno;

	close(res);

	return 0;
}

static int xmp_rmdir(const char *path)
{
	int res;
	char fpath[1000];
	char *a = strstr(path, en1);
	if (a != NULL)
		dec(a);

	if (strcmp(path, "/") == 0){
		path = dirPath;
		sprintf(fpath, "%s", path);
	}else{
		sprintf(fpath, "%s%s", dirPath, path);
	}

	res = rmdir(fpath);


	if (res == -1)
		return -errno;

	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr = xmp_getattr,
	.readdir = xmp_readdir,
	.read = xmp_read,
	.mkdir = xmp_mkdir,
	.rename = xmp_rename,
	.create = xmp_create,
	.rmdir = xmp_rmdir,
};


int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
