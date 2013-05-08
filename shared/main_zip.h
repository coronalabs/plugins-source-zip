//
//  main_zip.h
//  Plugin
//
//  Created by CoronaLabs on 4/18/13.
//
//

#include "zlib.h"
#include "unzip.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>

#ifdef _WIN32
# include <direct.h>
# include <io.h>
#else
# include <unistd.h>
# include <utime.h>
# include <sys/types.h>
# include <sys/stat.h>
#endif

#ifdef _WIN32
        #define USEWIN32IOAPI
        #include "iowin32.h"
#endif

//extern "C" {
//#include <zlib.h>
//}

#define CASESENSITIVITY (0)
#define WRITEBUFFERSIZE (16384)
#define MAXFILENAME (256)

#include "zip.h"
#if (!defined(_WIN32)) && (!defined(WIN32)) && (!defined(__APPLE__))
        #ifndef __USE_FILE_OFFSET64
                #define __USE_FILE_OFFSET64
        #endif
        #ifndef __USE_LARGEFILE64
                #define __USE_LARGEFILE64
        #endif
        #ifndef _LARGEFILE64_SOURCE
                #define _LARGEFILE64_SOURCE
        #endif
        #ifndef _FILE_OFFSET_BIT
                #define _FILE_OFFSET_BIT 64
        #endif
#endif

#if (defined(__APPLE__)) || (defined(__ANDROID__))
// In darwin and perhaps other BSD variants off_t is a 64 bit value, hence no need for specific 64 bit functions
#define FOPEN_FUNC(filename, mode) fopen(filename, mode)
#define FTELLO_FUNC(stream) ftello(stream)
#define FSEEKO_FUNC(stream, offset, origin) fseeko(stream, offset, origin)
#else
#define FOPEN_FUNC(filename, mode) fopen64(filename, mode)
#define FTELLO_FUNC(stream) ftello64(stream)
#define FSEEKO_FUNC(stream, offset, origin) fseeko64(stream, offset, origin)
#endif


//extern uLong ZEXPORT crc32(uLong crc, const Bytef *buf, uInt len);
//extern uLong filetime(char *f, tm_zip *tmzip,  uLong *dt);
int getFileCrc(const char* filenameinzip,void*buf,unsigned long size_buf,unsigned long* result_crc);

uLong filetime(char *f, tm_zip *tmzip,  uLong *dt);
#ifdef _WIN32
uLong filetime(char *f, tm_zip *tmzip,  uLong *dt)
{
  int ret = 0;
  {
      FILETIME ftLocal;
      HANDLE hFind;
      WIN32_FIND_DATAA ff32;

      hFind = FindFirstFileA(f,&ff32);
      if (hFind != INVALID_HANDLE_VALUE)
{
        FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftLocal);
        FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);
        FindClose(hFind);
        ret = 1;
      }
  }
  return ret;
}
#else

uLong filetime(char *f, tm_zip *tmzip,  uLong *dt)
{
  int ret=0;
  struct stat s;
  struct tm* filedate;
  time_t tm_t=0;

  if (strcmp(f,"-")!=0)
  {
    char name[MAXFILENAME+1];
    int len = strlen(f);
    if (len > MAXFILENAME)
      len = MAXFILENAME;

    strncpy(name, f,MAXFILENAME-1);

    name[ MAXFILENAME ] = '\0';

    if (name[len - 1] == '/')
      name[len - 1] = '\0';

    if (stat(name,&s)==0)
    {
      tm_t = s.st_mtime;
      ret = 1;
    }
  }
  filedate = localtime(&tm_t);

  tmzip->tm_sec  = filedate->tm_sec;
  tmzip->tm_min  = filedate->tm_min;
  tmzip->tm_hour = filedate->tm_hour;
  tmzip->tm_mday = filedate->tm_mday;
  tmzip->tm_mon  = filedate->tm_mon ;
  tmzip->tm_year = filedate->tm_year;

  return ret;
}
#endif

/* calculate the CRC32 of a file,
   because to encrypt a file, we need known the CRC32 of the file before */
int getFileCrc(const char* filenameinzip,void*buf,unsigned long size_buf,unsigned long* result_crc)
{
   unsigned long calculate_crc=0;
   int err=ZIP_OK;
   FILE * fin = FOPEN_FUNC(filenameinzip,"rb");

   unsigned long size_read = 0;
   unsigned long total_read = 0;
   if (fin==NULL)
   {
       err = ZIP_ERRNO;
   }

    if (err == ZIP_OK)
        do
        {
            err = ZIP_OK;
            size_read = (int)fread(buf,1,size_buf,fin);
            if (size_read < size_buf)
                if (feof(fin)==0)
            {
                printf("error in reading %s\n",filenameinzip);
                err = ZIP_ERRNO;
            }

            if (size_read>0)
                calculate_crc = crc32(calculate_crc,(Bytef *)buf,size_read);
            total_read += size_read;

        } while ((err == ZIP_OK) && (size_read>0));

    if (fin)
        fclose(fin);

    *result_crc=calculate_crc;
    printf("file %s crc %lx\n", filenameinzip, calculate_crc);
    return err;
}

int isLargeFile(const char* filename)
{
  int largeFile = 0;
  ZPOS64_T pos = 0;
  FILE* pFile = FOPEN_FUNC(filename, "rb");

  if(pFile != NULL)
  {
    int n = FSEEKO_FUNC(pFile, 0, SEEK_END);
    pos = FTELLO_FUNC(pFile);

                //printf("File : %s is %lld bytes\n", filename, pos);

    if(pos >= 0xffffffff)
     largeFile = 1;

                fclose(pFile);
  }

 return largeFile;
}
int check_exist_file( const char* filename);

int check_exist_file( const char* filename)
{
    FILE* ftestexist;
    int ret = 1;
    ftestexist = FOPEN_FUNC(filename,"rb");
    if (ftestexist==NULL)
        ret = 0;
    else
        fclose(ftestexist);
    return ret;
}
void change_file_date(const char *filename, uLong dosdate, tm_unz tmu_date);
void change_file_date(const char *filename, uLong dosdate, tm_unz tmu_date)
{
#ifdef _WIN32
  HANDLE hFile;
  FILETIME ftm,ftLocal,ftCreate,ftLastAcc,ftLastWrite;

  hFile = CreateFileA(filename,GENERIC_READ | GENERIC_WRITE,
                      0,NULL,OPEN_EXISTING,0,NULL);
  GetFileTime(hFile,&ftCreate,&ftLastAcc,&ftLastWrite);
  DosDateTimeToFileTime((WORD)(dosdate>>16),(WORD)dosdate,&ftLocal);
  LocalFileTimeToFileTime(&ftLocal,&ftm);
  SetFileTime(hFile,&ftm,&ftLastAcc,&ftm);
  CloseHandle(hFile);
#else
#ifdef unix || __APPLE__
  struct utimbuf ut;
  struct tm newdate;
  newdate.tm_sec = tmu_date.tm_sec;
  newdate.tm_min=tmu_date.tm_min;
  newdate.tm_hour=tmu_date.tm_hour;
  newdate.tm_mday=tmu_date.tm_mday;
  newdate.tm_mon=tmu_date.tm_mon;
  if (tmu_date.tm_year > 1900)
      newdate.tm_year=tmu_date.tm_year - 1900;
  else
      newdate.tm_year=tmu_date.tm_year ;
  newdate.tm_isdst=-1;

  ut.actime=ut.modtime=mktime(&newdate);
  utime(filename,&ut);
#endif
#endif
}


/* mymkdir and change_file_date are not 100 % portable
   As I don't know well Unix, I wait feedback for the unix portion */

int mymkdir(const char* dirname);
int mymkdir(const char* dirname)
{
    int ret=0;
#ifdef _WIN32
    ret = _mkdir(dirname);
#elif unix
    ret = mkdir (dirname,0775);
#elif __APPLE__
    ret = mkdir (dirname,0775);
#endif
    return ret;
}
int makedir (const char *newdir);
int makedir (const char *newdir)
{
  char *buffer ;
  char *p;
  int  len = (int)strlen(newdir);

  if (len <= 0)
    return 0;

  buffer = (char*)malloc(len+1);
        if (buffer==NULL)
        {
                printf("Error allocating memory\n");
                return UNZ_INTERNALERROR;
        }
  strcpy(buffer,newdir);

  if (buffer[len-1] == '/') {
    buffer[len-1] = '\0';
  }
  if (mymkdir(buffer) == 0)
    {
      free(buffer);
      return 1;
    }

  p = buffer+1;
  while (1)
    {
      char hold;

      while(*p && *p != '\\' && *p != '/')
        p++;
      hold = *p;
      *p = 0;
      if ((mymkdir(buffer) == -1) && (errno == ENOENT))
        {
          printf("couldn't create directory %s\n",buffer);
          free(buffer);
          return 0;
        }
      if (hold == 0)
        break;
      *p++ = hold;
    }
  free(buffer);
  return 1;
}

void Display64BitsSize(ZPOS64_T n, int size_char);
void Display64BitsSize(ZPOS64_T n, int size_char)
{
  /* to avoid compatibility problem , we do here the conversion */
  char number[21];
  int offset=19;
  int pos_string = 19;
  number[20]=0;
  for (;;) {
      number[offset]=(char)((n%10)+'0');
      if (number[offset] != '0')
          pos_string=offset;
      n/=10;
      if (offset==0)
          break;
      offset--;
  }
  {
      int size_display_string = 19-pos_string;
      while (size_char > size_display_string)
      {
          size_char--;
          printf(" ");
      }
  }

  printf("%s",&number[pos_string]);
}
int do_list(unzFile uf, char **retList, int &size);
int do_list(unzFile uf, char **retList, int &size)
{
    uLong i;
    unz_global_info64 gi;
    int err;
	

    err = unzGetGlobalInfo64(uf,&gi);
    if (err!=UNZ_OK)
        printf("error %d with zipfile in unzGetGlobalInfo \n",err);
    printf("  Length  Method     Size Ratio   Date    Time   CRC-32     Name\n");
    printf("  ------  ------     ---- -----   ----    ----   ------     ----\n");

	
	char *fileList = (char*)malloc(gi.number_entry * 256 * sizeof(char));
    //retList = fileList;
	
	size = gi.number_entry;

    for (i=0;i<gi.number_entry;i++)
    {
        char filename_inzip[256];
        unz_file_info64 file_info;
        uLong ratio=0;
        const char *string_method;
        char charCrypt=' ';
        err = unzGetCurrentFileInfo64(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
        if (err!=UNZ_OK)
        {
            printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
            break;
        }
		
		char *curFile = &fileList[i];
		
		//curFile = (char*)malloc(256);
		strcpy(curFile,filename_inzip );
		
		
        if (file_info.uncompressed_size>0)
            ratio = (uLong)((file_info.compressed_size*100)/file_info.uncompressed_size);

        /* display a '*' if the file is crypted */
        if ((file_info.flag & 1) != 0)
            charCrypt='*';

        if (file_info.compression_method==0)
            string_method="Stored";
        else
        if (file_info.compression_method==Z_DEFLATED)
        {
            uInt iLevel=(uInt)((file_info.flag & 0x6)/2);
            if (iLevel==0)
              string_method="Defl:N";
            else if (iLevel==1)
              string_method="Defl:X";
            else if ((iLevel==2) || (iLevel==3))
              string_method="Defl:F"; /* 2:fast , 3 : extra fast*/
        }
        else
        if (file_info.compression_method==Z_BZIP2ED)
        {
              string_method="BZip2 ";
        }
        else
            string_method="Unkn. ";

        Display64BitsSize(file_info.uncompressed_size,7);
        printf("  %6s%c",string_method,charCrypt);
        Display64BitsSize(file_info.compressed_size,7);
        printf(" %3lu%%  %2.2lu-%2.2lu-%2.2lu  %2.2lu:%2.2lu  %8.8lx   %s\n",
                ratio,
                (uLong)file_info.tmu_date.tm_mon + 1,
                (uLong)file_info.tmu_date.tm_mday,
                (uLong)file_info.tmu_date.tm_year % 100,
                (uLong)file_info.tmu_date.tm_hour,(uLong)file_info.tmu_date.tm_min,
                (uLong)file_info.crc,filename_inzip);
				

        if ((i+1)<gi.number_entry)
        {
            err = unzGoToNextFile(uf);
            if (err!=UNZ_OK)
            {
                printf("error %d with zipfile in unzGoToNextFile\n",err);
                break;
            }
        }
    }
	
	*retList = fileList;

    return 0;
}

int do_extract_currentfile(    unzFile uf, const int* popt_extract_without_path, int* popt_overwrite, const char* password);
int do_extract_currentfile(    unzFile uf, const int* popt_extract_without_path, int* popt_overwrite, const char* password)
{
    char filename_inzip[256];
    char* filename_withoutpath;
    char* p;
    int err=UNZ_OK;
    FILE *fout=NULL;
    void* buf;
    uInt size_buf;

    unz_file_info64 file_info;
    //uLong ratio=0;
    err = unzGetCurrentFileInfo64(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);

    if (err!=UNZ_OK)
    {
        //printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
        return err;
    }

    size_buf = WRITEBUFFERSIZE;
    buf = (void*)malloc(size_buf);
    if (buf==NULL)
    {
        //printf("Error allocating memory\n");
        return UNZ_INTERNALERROR;
    }

    p = filename_withoutpath = filename_inzip;
    while ((*p) != '\0')
    {
        if (((*p)=='/') || ((*p)=='\\'))
            filename_withoutpath = p+1;
        p++;
    }

    if ((*filename_withoutpath)=='\0')
    {
        if ((*popt_extract_without_path)==0)
        {
            //printf("creating directory: %s\n",filename_inzip);
            mymkdir(filename_inzip);
        }
    }
    else
    {
        const char* write_filename;
        int skip=0;

        if ((*popt_extract_without_path)==0)
            write_filename = filename_inzip;
        else
            write_filename = filename_withoutpath;

        err = unzOpenCurrentFilePassword(uf,password);
        if (err!=UNZ_OK)
        {
            //printf("error %d with zipfile in unzOpenCurrentFilePassword\n",err);
        }

        if (((*popt_overwrite)==0) && (err==UNZ_OK))
        {
            char rep=0;
            FILE* ftestexist;
            ftestexist = FOPEN_FUNC(write_filename,"rb");
            if (ftestexist!=NULL)
            {
                fclose(ftestexist);
                do
                {
                    char answer[128];
                    int ret;

                    //printf("The file %s exists. Overwrite ? [y]es, [n]o, [A]ll: ",write_filename);
                    ret = scanf("%1s",answer);
                    if (ret != 1)
                    {
                       exit(EXIT_FAILURE);
                    }
                    rep = answer[0] ;
                    if ((rep>='a') && (rep<='z'))
                        rep -= 0x20;
                }
                while ((rep!='Y') && (rep!='N') && (rep!='A'));
            }

            if (rep == 'N')
                skip = 1;

            if (rep == 'A')
                *popt_overwrite=1;
        }

        if ((skip==0) && (err==UNZ_OK))
        {
            fout=FOPEN_FUNC(write_filename,"wb");
            /* some zipfile don't contain directory alone before file */
            if ((fout==NULL) && ((*popt_extract_without_path)==0) &&
                                (filename_withoutpath!=(char*)filename_inzip))
            {
                char c=*(filename_withoutpath-1);
                *(filename_withoutpath-1)='\0';
                
				
				
				makedir(write_filename);
                *(filename_withoutpath-1)=c;
                fout=FOPEN_FUNC(write_filename,"wb");
            }

            if (fout==NULL)
            {
                //printf("error opening %s\n",write_filename);
            }
        }

        if (fout!=NULL)
        {
            //printf(" extracting: %s\n",write_filename);

            do
            {
                err = unzReadCurrentFile(uf,buf,size_buf);
                if (err<0)
                {
                    //printf("error %d with zipfile in unzReadCurrentFile\n",err);
                    break;
                }
                if (err>0)
                    if (fwrite(buf,err,1,fout)!=1)
                    {
                        //printf("error in writing extracted file\n");
                        err=UNZ_ERRNO;
                        break;
                    }
            }
            while (err>0);
            if (fout)
                    fclose(fout);

            if (err==0)
                change_file_date(write_filename,file_info.dosDate,
                                 file_info.tmu_date);
        }

        if (err==UNZ_OK)
        {
            err = unzCloseCurrentFile (uf);
            if (err!=UNZ_OK)
            {
                //printf("error %d with zipfile in unzCloseCurrentFile\n",err);
            }
        }
        else
            unzCloseCurrentFile(uf); /* don't lose the error */
    }

    free(buf);
    return err;
}

int do_extract(unzFile uf,int opt_extract_without_path,int opt_overwrite,const char* password);
int do_extract(unzFile uf,int opt_extract_without_path,int opt_overwrite,const char* password)

{
    uLong i;
    unz_global_info64 gi;
    int err;
    //FILE* fout=NULL;

    err = unzGetGlobalInfo64(uf,&gi);
    //if (err!=UNZ_OK)
        //printf("error %d with zipfile in unzGetGlobalInfo \n",err);

    for (i=0;i<gi.number_entry;i++)
    {
        if (do_extract_currentfile(uf,&opt_extract_without_path,
                                      &opt_overwrite,
                                      password) != UNZ_OK)
            break;

        if ((i+1)<gi.number_entry)
        {
            err = unzGoToNextFile(uf);
            if (err!=UNZ_OK)
            {
                //printf("error %d with zipfile in unzGoToNextFile\n",err);
                break;
            }
        }
    }

    return 0;
}
int do_extract_onefile(unzFile uf, const char* filename, int opt_extract_without_path, int opt_overwrite, const char* password);
int do_extract_onefile(unzFile uf, const char* filename, int opt_extract_without_path, int opt_overwrite, const char* password)
{
    //int err = UNZ_OK;
    if (unzLocateFile(uf,filename,CASESENSITIVITY)!=UNZ_OK)
    {
       // printf("file %s not found in the zipfile\n",filename);
        return 2;
    }

    if (do_extract_currentfile(uf,&opt_extract_without_path,
                                      &opt_overwrite,
                                      password) == UNZ_OK)
        return 0;
    else
        return 1;
}

int GetFileList(const char *zipFileName, char **retList, int &size);
int GetFileList(const char *zipFileName, char **retList, int &size)
{
	const char *zipfilename=zipFileName;
	char zipNameComplete[MAXFILENAME+16] = "";
	unzFile uf=NULL;

    if (zipfilename!=NULL)
    {

#        ifdef USEWIN32IOAPI
        zlib_filefunc64_def ffunc;
#        endif

        strncpy(zipNameComplete, zipfilename,MAXFILENAME-1);
        /* strncpy doesnt append the trailing NULL, of the string is too long. */
        zipNameComplete[ MAXFILENAME ] = '\0';

#        ifdef USEWIN32IOAPI
        fill_win32_filefunc64A(&ffunc);
        uf = unzOpen2_64(zipfilename,&ffunc);
#        else
        uf = unzOpen64(zipfilename);
#        endif
        if (uf==NULL)
        {
            strcat(zipNameComplete,".zip");
#            ifdef USEWIN32IOAPI
            uf = unzOpen2_64(zipNameComplete,&ffunc);
#            else
            uf = unzOpen64(zipNameComplete);
#            endif
        }
    }

    if (uf==NULL)
    {
        printf("Cannot open %s or %s.zip\n",zipfilename,zipfilename);
        return 1;
    }
    printf("%s opened\n",zipNameComplete);

	//char* fileList = NULL;
	//int size;
	
	do_list(uf,retList,size);

	return 1;
}


int ExtractFileFromZip(const char *zipFileName, const char *fileToExtract, int extractToDir, const char *dirToExtractTo);
int ExtractFileFromZip(const char *zipFileName, const char *fileToExtract, int extractToDir, const char *dirToExtractTo)
{
	const char *zipfilename=zipFileName;
    const char *filename_to_extract=fileToExtract;
    const char *password=NULL;
    char zipNameComplete[MAXFILENAME+16] = "";
    int ret_value=0;
    int opt_do_list=0;
    int opt_do_extract=1;
    int opt_do_extract_withoutpath=1;
    int opt_overwrite=1;
    int opt_extractdir=extractToDir;
    const char *dirname=dirToExtractTo;
    unzFile uf=NULL;

    if (zipfilename!=NULL)
    {

#        ifdef USEWIN32IOAPI
        zlib_filefunc64_def ffunc;
#        endif

        strncpy(zipNameComplete, zipfilename,MAXFILENAME-1);
        /* strncpy doesnt append the trailing NULL, of the string is too long. */
        zipNameComplete[ MAXFILENAME ] = '\0';

#        ifdef USEWIN32IOAPI
        fill_win32_filefunc64A(&ffunc);
        uf = unzOpen2_64(zipfilename,&ffunc);
#        else
        uf = unzOpen64(zipfilename);
#        endif
        if (uf==NULL)
        {
            strcat(zipNameComplete,".zip");
#            ifdef USEWIN32IOAPI
            uf = unzOpen2_64(zipNameComplete,&ffunc);
#            else
            uf = unzOpen64(zipNameComplete);
#            endif
        }
    }

    if (uf==NULL)
    {
        printf("Cannot open %s or %s.zip\n",zipfilename,zipfilename);
        return 1;
    }
    printf("%s opened\n",zipNameComplete);

	char* fileList = NULL;
	int size;
	
	do_list(uf,&fileList,size);
    if (opt_do_list==1)
        ret_value = do_list(uf,&fileList,size);
    else if (opt_do_extract==1)
    {
#ifdef _WIN32
        if (opt_extractdir && _chdir(dirname))
#else
        if (opt_extractdir && chdir(dirname))
#endif
        {
          printf("Error changing into %s, aborting\n", dirname);
          exit(-1);
        }

        if (filename_to_extract == NULL)
            ret_value = do_extract(uf, opt_do_extract_withoutpath, opt_overwrite, password);
        else
            ret_value = do_extract_onefile(uf, filename_to_extract, opt_do_extract_withoutpath, opt_overwrite, password);
    }

    unzClose(uf);

    return ret_value;
}
int ExtractAllFromZip(const char *zipFileName, int extractToDir, const char *dirToExtractTo);
int ExtractAllFromZip(const char *zipFileName, int extractToDir, const char *dirToExtractTo)
{
	return ExtractFileFromZip(zipFileName,NULL,extractToDir,dirToExtractTo);
}

int AddToZip(const char *zipFileName, const char *fileToAdd, int includeFilePath);
int AddToZip(const char *zipFileName, const char *fileToAdd, int includeFilePath)
{
	
    int overwriteZipFile=0;
    int opt_compress_level=Z_DEFAULT_COMPRESSION;
    char zipNameComplete[MAXFILENAME+16];
    int retCode=0;
    int size_buf=0;
    void* buf=NULL;
    const char* password=NULL;
	
	//Memory alloc issue
    size_buf = WRITEBUFFERSIZE;
    buf = (void*)malloc(size_buf);
    if (buf==NULL)
    {
        return ZIP_INTERNALERROR;
    }
	
	int len;
	int dot_found=0;
	
	strncpy(zipNameComplete, zipFileName, MAXFILENAME-1);
	/* strncpy doesnt append the trailing NULL, of the string is too long. */
	zipNameComplete[ MAXFILENAME ] = '\0';
	len=(int)strlen(zipNameComplete);
	
	int i = 0;
	for (i=0;i<len;i++)
	{
		if (zipNameComplete[i]=='.')
			dot_found=1;
	}
	
	if (dot_found==0)
	{
		strcat(zipNameComplete,".zip");
	}
	
	if (overwriteZipFile==0)
	{
		if (check_exist_file(zipNameComplete)!=0)
		{
			overwriteZipFile = 2; //overwrite file
		}
	}
	
	zipFile zf;
	int errclose;
#        ifdef USEWIN32IOAPI
	zlib_filefunc64_def ffunc;
	fill_win32_filefunc64A(&ffunc);
	zf = zipOpen2_64(zipNameComplete,(overwriteZipFile==2) ? 2 : 0,NULL,&ffunc);
#        else
	zf = zipOpen64(zipNameComplete,(overwriteZipFile==2) ? 2 : 0);
#        endif
	
	if (zf == NULL)
	{
		retCode = ZIP_ERRNO;
	}
	else
	{
		//printf("creating %s\n",zipNameComplete);
	}
	
	FILE * fin;
	int size_read;
	//const char* filenameinzip = "testfile.txt";//argv[i];
	
	char filenameinzip[MAXFILENAME];// = fileToAdd;//{"testfile.txt"};
	
	strncpy(filenameinzip, fileToAdd, MAXFILENAME-1);
	
	const char *savefilenameinzip;
	zip_fileinfo zi;
	unsigned long crcFile=0;
	int zip64 = 0;
	
	zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
	zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
	zi.dosDate = 0;
	zi.internal_fa = 0;
	zi.external_fa = 0;
	
	filetime(&(*filenameinzip),&zi.tmz_date,&zi.dosDate);
	
	if ((password != NULL) && (retCode==ZIP_OK))
	{
		retCode = getFileCrc(filenameinzip,buf,size_buf,&crcFile);
	}
	
	zip64 = isLargeFile(filenameinzip);
	
	/* The path name saved, should not include a leading slash. */
	/*if it did, windows/xp and dynazip couldn't read the zip file. */
	savefilenameinzip = filenameinzip;
	while( savefilenameinzip[0] == '\\' || savefilenameinzip[0] == '/' )
	{
		savefilenameinzip++;
	}
	
	/*should the zip file contain any path at all?*/
	
	if( includeFilePath == 0 )
	{
		const char *tmpptr;
		const char *lastslash = 0;
		for( tmpptr = savefilenameinzip; *tmpptr; tmpptr++)
		{
			if( *tmpptr == '\\' || *tmpptr == '/')
			{
				lastslash = tmpptr;
			}
		}
		if( lastslash != NULL )
		{
			savefilenameinzip = lastslash+1; // base filename follows last slash.
		}
	}
	
	
	retCode = zipOpenNewFileInZip3_64(zf,savefilenameinzip,&zi,
								  NULL,0,NULL,0,NULL /* comment*/,
								  (opt_compress_level != 0) ? Z_DEFLATED : 0,
								  opt_compress_level,0,
								  /* -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, */
								  -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
								  password,crcFile, zip64);
	
	if (retCode != ZIP_OK)
	{
		//printf("error in opening %s in zipfile\n",savefilenameinzip);
	}
	else
	{
		fin = FOPEN_FUNC(fileToAdd,"rb");
		if (fin==NULL)
		{
			retCode=ZIP_ERRNO;
			//printf("error in opening %s for reading\n",filenameinzip);
		}
	}
	
	if (retCode == ZIP_OK)
	{
		do
		{
			retCode = ZIP_OK;
			size_read = (int)fread(buf,1,size_buf,fin);
			if (size_read < size_buf)
			{
				if (feof(fin)==0)
				{
					printf("error in reading %s\n",filenameinzip);
					retCode = ZIP_ERRNO;
				}
			}
			
			if (size_read>0)
			{
				retCode = zipWriteInFileInZip (zf,buf,size_read);
				if (retCode<0)
				{
					//printf("error in writing %s in the zipfile\n",filenameinzip);
				}
				
			}
		} while ((retCode == ZIP_OK) && (size_read>0));
	}
	
	if (fin)
	{
		fclose(fin);
	}
	
	if (retCode<0)
	{
		retCode=ZIP_ERRNO;
	}
	else
	{
		retCode = zipCloseFileInZip(zf);
		if (retCode!=ZIP_OK)
		{
			//printf("error in closing %s in the zipfile\n",filenameinzip);
		}
	}
	
	errclose = zipClose(zf,NULL);
	if (errclose != ZIP_OK)
	{
		//printf("error in closing %s\n",zipNameComplete);
	}

	
    free(buf);
	
    return retCode;
}
