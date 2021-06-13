# soal-shift-sisop-modul-4-F06-2021

## Soal No. 4
Untuk memudahkan dalam memonitor kegiatan pada filesystem mereka Sin dan Sei membuat sebuah log system dengan spesifikasi sebagai berikut.

- Log system yang akan terbentuk bernama “SinSeiFS.log” pada direktori home pengguna (/home/[user]/SinSeiFS.log). Log system ini akan menyimpan daftar perintah system call yang telah dijalankan pada filesystem.
- Karena Sin dan Sei suka kerapian maka log yang dibuat akan dibagi menjadi dua level, yaitu INFO dan WARNING.
- Untuk log level WARNING, digunakan untuk mencatat syscall rmdir dan unlink.
- Sisanya, akan dicatat pada level INFO.
- Format untuk logging yaitu:

  [Level]::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]

  Level : Level logging, dd : 2 digit tanggal, mm : 2 digit bulan, yyyy : 4 digit tahun, HH : 2 digit jam (format 24 Jam),MM : 2 digit menit, SS : 2 digit detik, CMD : System Call   yang terpanggil, DESC : informasi dan parameter tambahan
  INFO::28052021-10:00:00:CREATE::/test.txt
  INFO::28052021-10:01:00:RENAME::/test.txt::/rename.txt

## Jawaban Soal No.4

Pada soal ini kita diminta untuk membuat sebuah log yang diberi nama `SinSeiFS.log`. File log tersebut digunakan untuk mencatat setiap command yang diberikan pada file system. Terdapat 2 level pada pencatatan log tersebut yaitu Warning dan Info. Untuk format dalam pencatatan log tersebut adalah berikut `[Level]::[dd][mm][yyyy]-[HH]:[MM]:[SS]:[CMD]::[DESC :: DESC]`. 

Untuk membuat log dengan 2 level seperti yang ada pada soal adalah dengan membuat 2 buah fungsi yang merepresentasikan level pada pencatatan log. Untuk fungsi nya adalah sebagai berikut:

```C
int log_sinsei_info(char *command, const char *from, const char *to){
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  char mains[1000];
  if(to == NULL){
     sprintf(mains,"INFO::%02d%02d%02d-%02d:%02d:%02d:%s::%s\n",
	   tm.tm_mday, tm.tm_mon + 1, 1900 + tm.tm_year, tm.tm_hour, tm.tm_min, tm.tm_sec, command, from);
  }
  else{
     sprintf(mains,"INFO::%02d%02d%02d-%02d:%02d:%02d:%s::%s::%s\n",
	   tm.tm_mday, tm.tm_mon + 1, 1900 + tm.tm_year, tm.tm_hour, tm.tm_min, tm.tm_sec, command, from, to);
  }
  printf("%s", mains);
  FILE *foutput = fopen(log_path, "a+");
  fputs(mains, foutput);
  fclose(foutput);
  return 1;
}
```

Untuk struktur fungsi pada pencatatan log tersebut sama, hanya tinggal mengganti tulisan level log nya dengan `INFO` atau `WARNING`. Untuk memanggil fungsi tersebut agar mencatat setiap aksi yang dilakukan pada filesystem adalah dengan memanggilnya dalam fungsi fuse. Contohnya adalah pada fungsi fuse read berikut:
```C
static int xmp_read(const char *path, char *buf, size_t size, off_t offset,struct fuse_file_info *fi){
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

  //Pemanggilan fungsi log
  log_sinsei_info("READ", path, NULL);
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
```
Jadi dengan memanggil fungsi log di dalam fungsi fuse, maka aksi tersebut akan dicatat dalam file `SinSeiFS.log` dengan aksi yang spesifik.
