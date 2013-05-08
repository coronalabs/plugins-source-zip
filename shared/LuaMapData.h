//
//  LuaMapData.h
//  Plugin
//
//  Created by CoronaLabs on 4/28/13.
//
//

#ifndef __Plugin__LuaMapData__
#define __Plugin__LuaMapData__

#include <iostream>
#include "CoronaLua.h"


namespace Corona
{
	
	class LData
	{
	public:
		typedef LData Self;
		
	public:
		typedef enum Type
		{
			kDouble = 0,
			kString = 1,
			kMap	= 2,
			kVector	= 3,
			kListener = 4,
			kLUD	= 5,
			
			kNumTypes
		}
		Type;
		
		LData():
		_lDataType(kDouble)
		{
			
		}
		
		virtual ~LData()
		{
		}
		
		virtual void* GetData() = 0;
		virtual LData *GetCopy() = 0;
	protected:
		void SetLDataType( Type t )
		{
			_lDataType = t;
		};
		Type GetLDataType()
		{
			return _lDataType;
		};
		
	protected:
		
		Type _lDataType;
		
	};
	class LDataLUD : public LData
	{
	public:
		LDataLUD(void* val)
		{
			_lData = val;
			SetLDataType(kLUD);
		}
		virtual ~LDataLUD()
		{
			//Doesn't delete, owner must
			_lData = NULL;
		}
		
		virtual void* GetData()
		{
			return static_cast<void*>(_lData);
		}
		virtual LData *GetCopy()
		{
			return new LDataLUD(_lData);
		}
		
	private:
		void* _lData;
		
	};
	
	class LDataDouble : public LData
	{
	public:
		LDataDouble(double val)
		{
			_lData = val;
			SetLDataType(kDouble);
		}
		virtual void* GetData()
		{
			return static_cast<void*>(&_lData);
		}
		virtual LData *GetCopy()
		{
			return new LDataDouble(_lData);
		}
		
	private:
		double _lData;
		
	};
	
	class LDataString : public LData
	{
	public:
		LDataString(std::string val)
		{
			_lData = val;
			SetLDataType(kString);
		}
		virtual ~LDataString()
		{

		}
		virtual void* GetData()
		{
			return static_cast<void*>(&_lData);
		}
		virtual LData *GetCopy()
		{
			return new LDataString(_lData);
		}
		std::string GetStr()
		{
			return _lData;
		}
		
	private:
		std::string _lData;
		
	};
	
	class LDataBool : public LData
	{
	public:
		LDataBool(int val)
		{
			_lData = (val == 0 ? 0 : 1);
			SetLDataType(kString);
		}
		virtual void* GetData()
		{
			return static_cast<void*>(&_lData);
		}
		virtual LData *GetCopy()
		{
			return new LDataBool(_lData);
		}
		bool GetBool()
		{
			return _lData;
		}
	private:
		bool _lData;
		
	};
	
	class LDataListener : public LData
	{
	public:
		LDataListener(CoronaLuaRef lListener)
		{
			fListener = lListener;
			SetLDataType(kListener);
		}
		virtual ~LDataListener()
		{
			//Doesn't delete
			fListener = 0;
		}
		virtual void* GetData()
		{
			return static_cast<void*>(&fListener);
		}
		virtual LData *GetCopy()
		{
			return new LDataListener(fListener);
		}
		
		CoronaLuaRef GetListener()
		{
			return fListener;
		}
		
	private:
		CoronaLuaRef fListener;
		
	};
	
}

#endif /* defined(__Plugin__LuaMapData__) */
