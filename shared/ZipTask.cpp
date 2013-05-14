//
//  TestTask.cpp
//  Plugin
//
//  Created by CoronaLabs on 4/29/13.
//
//

#include "ZipTask.h"
#include "main_zip.h"

namespace Corona
{
	ZipTaskExtract::ZipTaskExtract(	std::string pathSource,
									std::string outputDir,
									LMap *fileNames,
									bool flattenOutput,
									CoronaLuaRef ref):
				fFileNames(fileNames),
				fPathSource(pathSource),
				fFlattenOutput(flattenOutput),
				fOutputDir(outputDir)

	{
		fRef = ref;
	}
	ZipTaskExtract::~ZipTaskExtract()
	{
		if (fFileNames != NULL)
		{
			delete fFileNames;
			fFileNames = NULL;
		}
	}
	void ZipTaskExtract::Execute(CommandInterface *data)
	{

			std::string pathSource = fPathSource;
			
			int opt_extract_without_path = (fFlattenOutput ? 1 : 0);
			const char *password=NULL;
			char zipNameComplete[MAXFILENAME+16] = "";
			
			int ret_value=0;
			int opt_do_list=0;
			int opt_do_extract=1;
			int opt_overwrite=1;
			int opt_extractdir=1;//extractToDir;
			const char *dirname=fOutputDir.c_str();//dirToExtractTo;
			unzFile uf=NULL;
			
			
			const char *zipfilename=pathSource.c_str();//dirToExtractTo;
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
				fIsError = true;
				//printf("Cannot open %s or %s.zip\n",zipfilename,zipfilename);
				return;
			}
			//printf("%s opened\n",zipNameComplete);
			
			char* fileList = NULL;
			int size;
			
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
						//printf("Error changing into %s, aborting\n", dirname);
						return;
						fIsError = true;
					}
				
				uLong i;
				unz_global_info64 gi;
				int err;
				
				err = unzGetGlobalInfo64(uf,&gi);
				if (err!=UNZ_OK)
				{
					fIsError = true;
				}
				//	printf("error %d with zipfile in unzGetGlobalInfo \n",err);
				
				if (fFileNames == NULL)
				{
					for (i=0;i<gi.number_entry;i++)
					{
						char filename_inzip[256];
						unz_file_info64 file_info;
						err = unzGetCurrentFileInfo64(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
						
						//Cache result
						if (do_extract_currentfile(uf,&opt_extract_without_path,
												   &opt_overwrite,
												   password) == UNZ_OK)
						{
							LDataString *str = new LDataString(filename_inzip);
							fOutputList.SetData(filename_inzip, str);
						}
						else
						{
							fIsError = true;
							break;
						}
						if ((i+1)<gi.number_entry)
						{
							err = unzGoToNextFile(uf);
							if (err!=UNZ_OK)
							{
								//printf("error %d with zipfile in unzGoToNextFile\n",err);
								fIsError = true;
								break;
							}
						}
					}
				}
				else
				{
					//fileNames
					
					std::vector<std::string> keys = fFileNames->GetKeys();
					for (int i = 0; i < keys.size(); i++)
					{
						LDataString *str = static_cast<LDataString*>(fFileNames->GetData(keys[i])->GetCopy());
						
						if (str != NULL)
						{
							std::string fileName = str->GetStr();
							
							if ( UNZ_OK == do_extract_onefile(uf, fileName.c_str(), opt_extract_without_path, opt_overwrite, password))
							{
							
								size_t pos = fileName.find_last_of("\\");
								if (pos == std::string::npos)
								{
									pos = fileName.find_last_of("/");
								}
								if(pos != std::string::npos)
									fileName.assign(fileName.begin() + pos + 1, fileName.end());
							
								fOutputList.SetData(fileName.c_str(), str);
							}
							else
							{
								fIsError = true;
							}
						}
					}
					
				}
				
			}
			unzClose(uf);

	}
		
	void ZipTaskExtract::DoDispatch(lua_State *L)
	{
	
		Corona::ZipEvent e = Corona::ZipEvent(Corona::ZipEvent::kUnZipType,NULL,fIsError);
		e.Push(L);
		
		
		int responseTable = lua_gettop( L );
		lua_createtable( L, 0, 1);
		{
			
			//int luaTableStackIndex = lua_gettop( L );
			
			std::vector<std::string> keys = fOutputList.GetKeys();
			int keySize = keys.size();
			//lua_createtable( L, 0, keySize);
			//{
			
				for (int i = 0; i < keySize; i++)
				{
					int luaHeaderTableStackIndex = lua_gettop( L );
					
					std::string fileName = keys[i];
					lua_pushstring( L, fileName.c_str() );
					
					lua_rawseti(L, luaHeaderTableStackIndex, i+1);
				}
			//}
			//lua_setfield( L, luaTableStackIndex, "fileInfo" );
			
		}
		lua_setfield( L, responseTable, "response" );
		
		
		e.Dispatch( L, fRef);
		
	}

	//List All Files Task
	ZipTaskListAllFilesInZip::ZipTaskListAllFilesInZip( std::string pathSource,
														CoronaLuaRef ref):
	fPathSource(pathSource)
	{
		fRef = ref;
	}
	ZipTaskListAllFilesInZip::~ZipTaskListAllFilesInZip()
	{
	
	}
	void ZipTaskListAllFilesInZip::Execute(CommandInterface *data)
	{

		std::string pathSource = fPathSource;
		const char *zipfilename=pathSource.c_str();
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
			//printf("Cannot open %s or %s.zip\n",zipfilename,zipfilename);
			fIsError = true;
			return;
		}
		//printf("%s opened\n",zipNameComplete);
		
		//char* fileList = NULL;
		//int size;
		
		uLong i;
		unz_global_info64 gi;
		int err;
		
		
		err = unzGetGlobalInfo64(uf,&gi);
		//if (err!=UNZ_OK)
		//	printf("error %d with zipfile in unzGetGlobalInfo \n",err);
		//printf("  Length  Method     Size Ratio   Date    Time   CRC-32     Name\n");
		//printf("  ------  ------     ---- -----   ----    ----   ------     ----\n");
		
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
				//printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
				fIsError = true;
				break;
			}
			
			if (file_info.uncompressed_size>0)
				ratio = (uLong)((file_info.compressed_size*100)/file_info.uncompressed_size);
			
			/* display a '*' if the file is crypted */
			if ((file_info.flag & 1) != 0)
				charCrypt='*';
			
			if (file_info.compression_method==0)
			{
				string_method="Stored";
			}
			else
			{
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
			}
			
			//Display64BitsSize(file_info.uncompressed_size,7);
			//printf("  %6s%c",string_method,charCrypt);
			//Display64BitsSize(file_info.compressed_size,7);
			//printf(" %3lu%%  %2.2lu-%2.2lu-%2.2lu  %2.2lu:%2.2lu  %8.8lx   %s\n",
				   //ratio,
				   //(uLong)file_info.tmu_date.tm_mon + 1,
				   //(uLong)file_info.tmu_date.tm_mday,
				   //(uLong)file_info.tmu_date.tm_year % 100,
				   //(uLong)file_info.tmu_date.tm_hour,(uLong)file_info.tmu_date.tm_min,
				   //(uLong)file_info.crc,filename_inzip);
			
			output_info info;
			info.fileName	= std::string(filename_inzip);
			info.size		= file_info.uncompressed_size;
			info.crc		= file_info.crc;
			info.ratio		= ratio;
			
			fOutputInfo.push_back(info);
			
			if ((i+1)<gi.number_entry)
			{
				err = unzGoToNextFile(uf);
				if (err!=UNZ_OK)
				{
					//printf("error %d with zipfile in unzGoToNextFile\n",err);
					fIsError = true;
					break;
				}
			}
		}
	}

	void ZipTaskListAllFilesInZip::DoDispatch(lua_State *L)
	{
	
		Corona::ZipEvent e = Corona::ZipEvent(Corona::ZipEvent::kFileType,NULL,fIsError);
		e.Push(L);
		
		int responseTable = lua_gettop( L );

		int infoSize = fOutputInfo.size();
		lua_createtable( L, 0, 1);
		{
			//int luaTableStackIndex = lua_gettop( L );
			//lua_createtable( L, 0, infoSize);
			//{
				int luaHeaderTableStackIndex = lua_gettop( L );
				for (int i = 0; i < infoSize; i++)
				{
					
					lua_newtable(L);
					{
						int index = lua_gettop( L );
						std::string fileName = fOutputInfo[i].fileName;
						lua_pushstring( L, fileName.c_str() );
						lua_setfield( L, index, "file" );
						
						lua_pushnumber(L,fOutputInfo[i].size);
						lua_setfield( L, index, "size" );
						
						lua_pushnumber(L,fOutputInfo[i].ratio);
						lua_setfield( L, index, "ratio" );
					}
					lua_rawseti(L, luaHeaderTableStackIndex, i+1); //1 Based
				}
			//}
			//lua_setfield( L, luaTableStackIndex, "fileInfo" );
		}
		lua_setfield( L, responseTable, "response" );
		
		e.Dispatch( L, fRef);
	}



//Add File
ZipTaskAddFileToZip::ZipTaskAddFileToZip(	std::string pathSource,
											LVector fileList,
											CoronaLuaRef ref):
	fPathSource(pathSource),
	fFileList(fileList)
	{
		fRef = ref;
	}
	ZipTaskAddFileToZip::~ZipTaskAddFileToZip()
	{
	
	}
	void ZipTaskAddFileToZip::Execute(CommandInterface *data)
	{
		
		std::string pathSource = fPathSource;// + "/" + _fileName;
		int vals = fFileList.GetCount();
		
		for (int i = 0; i < vals && fIsError == false; i++)
		{
			
			std::string fileToAddName = fFileList.GetVal(i);
			if (ZIP_OK != AddToZip(pathSource.c_str(), fileToAddName.c_str(), 0))
			{
				fIsError = true;
			}
			else
			{
				std::string fileName = fileToAddName;
				size_t pos = fileToAddName.find_last_of("\\");
				if (pos == std::string::npos)
				{
					pos = fileToAddName.find_last_of("/");
				}
				if(pos != std::string::npos)
					fileName.assign(fileToAddName.begin() + pos + 1, fileToAddName.end());

	
				output_info info;
				info.fileName = std::string(fileName);
				info.crc	= 0;
				info.size	= 0;
				fOutputInfo.push_back(info);

			}
		}
	}

	void ZipTaskAddFileToZip::DoDispatch(lua_State *L)
	{
	
		Corona::ZipEvent e = Corona::ZipEvent(Corona::ZipEvent::kZipType,NULL,fIsError);
		e.Push(L);
		int responseTable = lua_gettop( L );
		lua_createtable( L, 0, 1);
		{
			//int luaTableStackIndex = lua_gettop( L );
			int infoSize = fOutputInfo.size();
			//lua_createtable( L, 0, infoSize);
			//{
				for (int i = 0; i < infoSize; i++)
				{
					int luaHeaderTableStackIndex = lua_gettop( L );
					std::string fileName = fOutputInfo[i].fileName;
					lua_pushstring( L, fileName.c_str() );
					lua_rawseti(L, luaHeaderTableStackIndex, i);
				}
			//}
			//lua_setfield( L, luaTableStackIndex, "fileInfo" );
		}
		lua_setfield( L, responseTable, "response" );
		e.Dispatch( L, fRef);

	}
}
