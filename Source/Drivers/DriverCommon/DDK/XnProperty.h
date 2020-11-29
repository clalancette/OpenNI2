/*****************************************************************************
*									     *
*  OpenNI 2.x Alpha							     *
*  Copyright (C) 2012 PrimeSense Ltd.					     *
*									     *
*  This file is part of OpenNI. 					     *
*									     *
*  Licensed under the Apache License, Version 2.0 (the "License");	     *
*  you may not use this file except in compliance with the License.	     *
*  You may obtain a copy of the License at				     *
*									     *
*      http://www.apache.org/licenses/LICENSE-2.0			     *
*									     *
*  Unless required by applicable law or agreed to in writing, software	     *
*  distributed under the License is distributed on an "AS IS" BASIS,	     *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and	     *
*  limitations under the License.					     *
*									     *
*****************************************************************************/
#ifndef XNPROPERTY_H
#define XNPROPERTY_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <XnDevice.h>
#include <XnStringsHash.h>
#include <XnLog.h>
#include <XnEvent.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

/**
* A holder for a property (a name and value pair). Note that this class should be inherited, and
* can not be used directly.
*/
class XnProperty
{
public:
	typedef XnStatus (XN_CALLBACK_TYPE* OnValueChangedHandler)(const XnProperty* pSender, void* pCookie);
	typedef xnl::EventInterface<OnValueChangedHandler> ChangeEventInterface;

	/**
	* Creates a new property.
	*
	* @param	Type			[in]	Type of the property.
	* @param	pValueHolder	[in]	A pointer to the holder of the value.
	* @param	strName 		[in]	Name of the property.
	* @param	strModule		[in]	Name of the module holding this property.
	*/
	XnProperty(XnPropertyType Type, void* pValueHolder, uint32_t propertyId, const char* strName, const char* strModule);
	virtual ~XnProperty();

	inline uint32_t GetId() const { return m_propertyId; }
	inline const char* GetName() const { return m_strName; }
	inline const char* GetModule() const { return m_strModule; }
	inline bool IsActual() const { return (m_pValueHolder != NULL); }
	virtual bool IsReadOnly() const = 0;
	inline XnPropertyType GetType() const { return m_Type; }

	inline ChangeEventInterface& OnChangeEvent() { return m_OnChangeEvent; }

	/** Updates property name. */
	void UpdateName(const char* strModule, const char* strName);

	/** Updates the value of the property according to an INI file. */
	virtual XnStatus ReadValueFromFile(const char* csINIFile, const char* csSection) = 0;

	/** Adds this property to the property set. */
	virtual XnStatus AddToPropertySet(XnPropertySet* pSet) = 0;

	/** Sets the log severity under which changes to the property are printed. */
	inline void SetLogSeverity(int32_t nSeverity) { m_LogSeverity = nSeverity; }

	/** When true, the property will always call the set callback, even if value hasn't changed. */
	inline void SetAlwaysSet(bool bAlwaysSet) { m_bAlwaysSet = bAlwaysSet; }

protected:
	/** Sets the property value. */
	XnStatus SetValue(const void* pValue);

	/** Gets the property value. */
	XnStatus GetValue(void* pValue) const;

	/** Updates the value of the property without any checks. */
	XnStatus UnsafeUpdateValue(const void* pValue = NULL);

	virtual XnStatus CopyValueImpl(void* pDest, const void* pSource) const = 0;
	virtual bool IsEqual(const void* pValue1, const void* pValue2) const = 0;
	virtual XnStatus CallSetCallback(const void* pValue) = 0;
	virtual XnStatus CallGetCallback(void* pValue) const = 0;
	virtual bool ConvertValueToString(char* csValue, const void* pValue) const;

	inline void* Value() const { return m_pValueHolder; }

private:
	class ChangeEvent : public xnl::EventInterface<OnValueChangedHandler>
	{
	public:
		XnStatus Raise(const XnProperty* pSender);
	};

	char m_strModule[XN_DEVICE_MAX_STRING_LENGTH]; // module name
	char m_strName[XN_DEVICE_MAX_STRING_LENGTH]; // property name
	uint32_t m_propertyId;
	XnPropertyType m_Type; // property type

	void* m_pValueHolder; // a pointer to the storage of the property

	ChangeEvent m_OnChangeEvent;

	int32_t m_LogSeverity;
	bool m_bAlwaysSet;
};

/** A hash table, mapping property name to the property */
typedef xnl::Hash<uint32_t, XnProperty*> XnPropertiesHash;

#endif // XNPROPERTY_H
