# soal-shift-sisop-modul-4-F06-2021

## Soal No. 1
Di suatu jurusan, terdapat admin lab baru yang super duper gabut, ia bernama Sin. Sin baru menjadi admin di lab tersebut selama 1 bulan. Selama sebulan tersebut ia bertemu orang-orang hebat di lab tersebut, salah satunya yaitu Sei. Sei dan Sin akhirnya berteman baik. Karena belakangan ini sedang ramai tentang kasus keamanan data, mereka berniat membuat filesystem dengan metode encode yang mutakhir. Berikut adalah filesystem rancangan Sin dan Sei :
	
NOTE : 
Semua file yang berada pada direktori harus ter-encode menggunakan Atbash cipher(mirror).
Misalkan terdapat file bernama kucinglucu123.jpg pada direktori DATA_PENTING
“AtoZ_folder/DATA_PENTING/kucinglucu123.jpg” → “AtoZ_folder/WZGZ_KVMGRMT/pfxrmtofxf123.jpg”
Note : filesystem berfungsi normal layaknya linux pada umumnya, Mount source (root) filesystem adalah directory /home/[USER]/Downloads, dalam penamaan file ‘/’ diabaikan, dan ekstensi tidak perlu di-encode.
Referensi : https://www.dcode.fr/atbash-cipher


- a. Jika sebuah direktori dibuat dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
- b. Jika sebuah direktori di-rename dengan awalan “AtoZ_”, maka direktori tersebut akan menjadi direktori ter-encode.
- c. Apabila direktori yang terenkripsi di-rename menjadi tidak ter-encode, maka isi direktori tersebut akan terdecode.
- d. Setiap pembuatan direktori ter-encode (mkdir atau rename) akan tercatat ke sebuah log. Format : /home/[USER]/Downloads/[Nama Direktori] → /home/[USER]/Downloads/AtoZ_[Nama Direktori]
- e. Metode encode pada suatu direktori juga berlaku terhadap direktori yang ada di dalamnya.(rekursif)

## Jawaban Soal No.1
Jika folder atau file terenkripsi di FUSE, maka di mount folder akan terdekripsi, begitu juga sebaliknya. Maka, terdapat fungsi enkripsi dan dekripsi. Di masing-masing fungsi tersebut kita akan melakukan looping untuk mengubah nama folder atau file. Maka kita perlu tahu batas perulangannya (dimulai dari awal nama folder/file dan berakhir sebelum file extension). Maka kita butuh fungsi `ext_id` dan `slash_id` yang akan mengembalikan index file extension dan index slash sebagai penanda awal dan akhir enkripsi dan dekripsi.
```c
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
```
Variabel mentok akan dikembalikan oleh fungsi ini jika `path` tidak memiliki `/`.
```c
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
```
Pemanggilan fungsi dekripsi dilakukan di utility functions getattr, mkdir, rename, rmdir, create, dan fungsi-fungsi lain yang menurut kami esensial dalam proses sinkronisasi FUSE dan mount folder. Fungsi dekripsi dan enkripsi dilakukan di utility function readdir karena FUSE akan melakukan dekripsi di mount folder lalu enkripsi di FUSE saat readdir. Pemanggilannya dilakukan dengan pengecekan apakah string "AtoZ_" terdapat di string path di masing-masing utility function dengan menggunakan fungsi strstr(). Jika ya, maka fungsi enkripsi dan dekripsi akan dipanggil untuk string tersebut dengan "AtoZ_" sebagai starting point string yang diteruskan.
- Fungsi log
```c
void tulisLog(char *from, char *to)
{
	int i;
	for(i=strlen(to); i >=0 ; i--){
		if(to[i] == '/')break;
	}
	if(strstr(to + i, en1) == NULL) return;
	char logs[1024];
	sprintf(logs, "%s -> %s\n", from, to);
	FILE *log_file = fopen(log_path,"a+");
	fputs(logs,log_file);
	fclose(log_file);
}
```
Fungsi ini untuk menuliskan log sesuai pada permintaan soal no 1d dimasukkan pada utility function mkdir dan rename.

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
