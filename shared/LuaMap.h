//
//  LuaMap.h
//  Plugin
//
//  Created by CoronaLabs on 4/28/13.
//
//

#ifndef __Plugin__LuaMap__
#define __Plugin__LuaMap__

#include "LuaMapData.h"
#include "CoronaLua.h"
#include <map>
#include <vector>
#include <sstream>


namespace Corona
{
	class LuaVectorData : public LData
	{
	public:
		LuaVectorData()
		{
			SetLDataType(kVector);
		}
		virtual ~LuaVectorData()
		{
			fVector.erase (fVector.begin(),fVector.end());
		}
		
		virtual void* GetData()
		{
			return static_cast<void*>(&fVector);
		}
		
		void AddData(LData *data)
		{
			fVector.push_back(data);
		}
		LData *GetData(int index)
		{
			return fVector[index];
		}
		
		virtual LData *GetCopy()
		{
			LuaVectorData *result = new LuaVectorData();
			
			return result;
		}
		
	private:
		std::vector<LData*> fVector;
		
	};
	
	class LuaMapData : public LData
	{
	public:
		LuaMapData()
		{
			SetLDataType(kMap);
		}
		virtual ~LuaMapData()
		{
		
			for(std::map<std::string,LData*>::iterator it = fDataMap.begin(); it != fDataMap.end(); ++it)
			{
				//LData *data = it->second;
				delete it->second;
			}
			fDataMap.clear();
			
			
		}
		virtual void* GetData()
		{
			return static_cast<void*>(&fDataMap);
		}
		
		void SetData(std::string key, LData *data)
		{
			fDataMap[key] = data;
		}
		bool HasKey(std::string key)
		{
			return false;
		}
		LData *GetData(std::string key)
		{
			return fDataMap[key];
		}
		
		virtual LData *GetCopy()
		{
			LuaMapData *result = new LuaMapData();
			
			std::vector<std::string> keyList = GetKeys();

			for(std::map<std::string,LData*>::iterator it = fDataMap.begin(); it != fDataMap.end(); ++it)
			{
				LData *dataCopy = (it->second)->GetCopy();
				
				result->SetData(it->first, dataCopy);
			}
			
			return result;
		}
		
		std::vector<std::string> GetKeys()
		{
			std::vector<std::string> result;
			
			
			for(std::map<std::string,LData*>::iterator it = fDataMap.begin(); it != fDataMap.end(); ++it)
			{
				result.push_back(it->first);
			}
			
			return result;
		}
		
	private:
		std::map<std::string, LData*> fDataMap;
		
	};
	
	class LVector : public LData
	{
		
	public:
		
		LVector()
		{
		}
		
		LVector(lua_State *L, int t)
		{
			SetLDataType(kVector);
			
			int framesIndex = t;//lua_gettop( L );
			
			lua_getfield( L, framesIndex, "filenames" );
			
			for ( int i = 0, iMax = lua_objlen( L, framesIndex ); i < iMax; i++ )
			{
				int index = (i+1); // Lua is 1-based so (i+1)th frame.
				lua_rawgeti( L, framesIndex, index );
				{
					int element = lua_gettop( L );
					
					if (!lua_isnil(L,-1))
					{
						const char *val = lua_tostring( L, -1 );
						std::string strVal = std::string(val);
						fVec.push_back(strVal);
					}
				}
			}
		}
		virtual LData *GetCopy()
		{
			return NULL;
		}
		virtual void* GetData()
		{
			return NULL;//return static_cast<void*>(&fDataMap);
		}
		int GetCount()
		{
			return fVec.size();
		}
		std::string GetVal(int index)
		{
			return fVec[index];
		}
		void SetVal(int index, std::string val)
		{
			fVec[index] = val;
		}
		void Push(std::string val)
		{
			fVec.push_back(val);
		}
		
	private:
		std::vector<std::string> fVec;
	};
	
	class LMap : public LuaMapData
	{
	public:
		LMap(lua_State *L, int t)
		{
			
			if (t < 0)
			{
				t = lua_gettop( L ) + (t + 1);
			}
			
			lua_pushnil( L );
			while ( lua_next( L, t ) != 0 )
			{
				
				int isNum		= lua_isnumber(L,-2);
				const char *k = NULL;
				
				std::string key;// = std::string(k);
				if (isNum == 1)
				{
					std::stringstream ss;
					ss << lua_tonumber( L, -2 );
					
					k = std::string(ss.str()).c_str();
				}
				else
				{
					k = lua_tostring( L, -2 );
				}
				
				key = std::string(k);
				
				
				
				int vType = lua_type( L, -1 );
				if ( LUA_TSTRING == vType )
				{
					const char *v = lua_tostring( L, -1 );
					if ( k && v )
					{
						std::string val = std::string(v);
						LData *dataVal = new LDataString(val);
						SetData(key,dataVal);
					}
				}
				else if ( LUA_TBOOLEAN == vType )
				{
					int b = lua_toboolean( L, -1 );
					LData *dataVal = new LDataBool(b);
					SetData(key,dataVal);
					
				}
				else if ( LUA_TNUMBER == vType )
				{
					
					double d = lua_tonumber( L, -1 );
					LData *dataVal = new LDataDouble(d);
					SetData(key,dataVal);
					
				}
				else if ( LUA_TTABLE == vType )
				{
					
					LData *dataVal = new LMap(L,-1);//lua_gettop( L ));
					
					if ( dataVal )
					{
						SetData(key,dataVal);
					}
				}
				else if (vType == LUA_TFUNCTION)
				{
					if ( CoronaLuaIsListener( L, -1, "zipevent" ) )
					{
						CoronaLuaRef fListener	= CoronaLuaNewRef( L, -1 );
						LData *listener = new LDataListener(fListener);//lua_gettop( L ));
						SetData(key,listener);
					}
				}
				else if (vType == LUA_TLIGHTUSERDATA)
				{
					void* lud = lua_touserdata( L, -1 );
					
					LDataLUD *dataVal = new LDataLUD(lud);
					SetData(key,dataVal);
				}
				else
				{
					
					//Rtt_ASSERT_NOT_IMPLEMENTED();
				}
				
				lua_pop( L, 1 ); // pop value
			}
			
		};
	};
	
}

#endif /* defined(__Plugin__LuaMap__) */
