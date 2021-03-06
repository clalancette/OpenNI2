/*****************************************************************************
*                                                                            *
*  OpenNI 2.x Alpha                                                          *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of OpenNI.                                              *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/
#ifndef XNDEVICEBASE_H
#define XNDEVICEBASE_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <list>
#include <set>
#include <string>

#include <XnStringsHash.h>
#include <XnDevice.h>
#include <DDK/XnDeviceModule.h>
#include "XnDeviceModuleHolder.h"
#include <XnEvent.h>
#include <DDK/XnDeviceStream.h>
#include <DDK/XnActualStringProperty.h>
#include <DDK/XnActualIntProperty.h>
#include <DDK/XnActualGeneralProperty.h>

//---------------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------------
#define XN_MASK_DEVICE						"Device"
#define XN_DEVICE_BASE_MAX_STREAMS_COUNT	100

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------

class XnDeviceBase
{
public:
	XnDeviceBase();
	virtual ~XnDeviceBase();

	//---------------------------------------------------------------------------
	// Properties Getters
	//---------------------------------------------------------------------------
	inline XnActualIntProperty& DeviceMirrorProperty() { return m_DeviceMirror; }

	//---------------------------------------------------------------------------
	// Getters
	//---------------------------------------------------------------------------
	inline bool GetDeviceMirror() const { return (bool)m_DeviceMirror.GetValue(); }

	inline XnDeviceModule* DeviceModule() { return m_pDevicePropertiesHolder->GetModule(); }
	inline XnDeviceModuleHolder* DeviceModuleHolder() { return m_pDevicePropertiesHolder; }

	//---------------------------------------------------------------------------
	// Setters
	//---------------------------------------------------------------------------
	virtual XnStatus SetMirror(bool bMirror);

	//---------------------------------------------------------------------------
	// IXnDevice Methods
	//---------------------------------------------------------------------------
	virtual XnStatus Init(const XnDeviceConfig* pDeviceConfig);
	virtual XnStatus Destroy();
	virtual XnStatus GetSupportedStreams(const char** aStreamNames, uint32_t* pnStreamNamesCount);
	virtual XnStatus CreateStream(const char* StreamType, const char* StreamName = NULL, const XnPropertySet* pInitialValues = NULL);
	virtual XnStatus DestroyStream(const char* StreamName);
	virtual XnStatus OpenStream(const char* StreamName);
	virtual XnStatus CloseStream(const char* StreamName);
	virtual XnStatus GetStreamNames(const char** pstrNames, uint32_t* pnNamesCount);
	virtual XnStatus DoesModuleExist(const char* ModuleName, bool* pbDoesExist);
	virtual XnStatus OpenAllStreams();
	virtual XnStatus CloseAllStreams();
	virtual XnStatus RegisterToNewStreamData(XnDeviceOnNewStreamDataEventHandler Handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual XnStatus UnregisterFromNewStreamData(XnCallbackHandle hCallback);
	virtual XnStatus DoesPropertyExist(const char* ModuleName, uint32_t propertyId, bool* pbDoesExist);
	virtual XnStatus GetPropertyType(const char* ModuleName, uint32_t propertyId, XnPropertyType* pnType);
	virtual XnStatus SetProperty(const char* ModuleName, uint32_t propertyId, uint64_t nValue);
	virtual XnStatus SetProperty(const char* ModuleName, uint32_t propertyId, double dValue);
	virtual XnStatus SetProperty(const char* ModuleName, uint32_t propertyId, const char* csValue);
	virtual XnStatus SetProperty(const char* ModuleName, uint32_t propertyId, const OniGeneralBuffer& Value);
	virtual XnStatus GetProperty(const char* ModuleName, uint32_t propertyId, uint64_t* pnValue);
	virtual XnStatus GetProperty(const char* ModuleName, uint32_t propertyId, double* pdValue);
	virtual XnStatus GetProperty(const char* ModuleName, uint32_t propertyId, char* csValue);
	virtual XnStatus GetProperty(const char* ModuleName, uint32_t propertyId, const OniGeneralBuffer& pValue);
	virtual XnStatus LoadConfigFromFile(const char* csINIFilePath, const char* csSectionName);
	virtual XnStatus BatchConfig(const XnPropertySet* pChangeSet);
	virtual XnStatus GetAllProperties(XnPropertySet* pSet, bool bNoStreams = false, const char* strModule = NULL);
	virtual XnStatus RegisterToPropertyChange(const char* Module, uint32_t propertyId, XnDeviceOnPropertyChangedEventHandler Handler, void* pCookie, XnCallbackHandle& hCallback);
	virtual XnStatus UnregisterFromPropertyChange(const char* Module, uint32_t propertyId, XnCallbackHandle hCallback);

	typedef xnl::Event<XnNewStreamDataEventArgs> NewStreamDataEvent;
	NewStreamDataEvent::Interface& OnNewStreamDataEvent() { return m_OnNewStreamDataEvent; }

	/**
	* Finds a stream (a module which has the IS_STREAM property set to true).
	*/
	XnStatus FindStream(const char* StreamName, XnDeviceStream** ppStream);

protected:
	virtual XnStatus InitImpl(const XnDeviceConfig* pDeviceConfig);
	virtual XnStatus CreateStreamImpl(const char* strType, const char* strName, const XnActualPropertiesHash* pInitialSet);

	virtual XnStatus CreateModule(const char* strName, XnDeviceModuleHolder** ppModuleHolder);
	virtual XnStatus CreateDeviceModule(XnDeviceModuleHolder** ppModuleHolder);
	virtual void DestroyModule(XnDeviceModuleHolder* pModuleHolder);

	XnStatus CreateStreams(const XnPropertySet* pSet);

	/**
	* Adds a module to the device modules.
	*/
	XnStatus AddModule(XnDeviceModuleHolder* pModuleHolder);

	/**
	* Removes a module from the device modules.
	*/
	XnStatus RemoveModule(const char* ModuleName);

	/**
	* Finds a module.
	*/
	XnStatus FindModule(const char* ModuleName, XnDeviceModule** ppModule);

	/**
	* Finds a module.
	*/
	XnStatus FindModule(const char* ModuleName, XnDeviceModuleHolder** ppModuleHolder);

	/**
	* Checks if a module is a stream.
	*/
	static bool IsStream(XnDeviceModule* pModule);

	/**
	* Finds a stream holder (a module which has the IS_STREAM property set to true).
	*/
	XnStatus FindStream(const char* StreamName, XnDeviceModuleHolder** ppStreamHolder);

	/**
	* Adds a stream to the list of supported streams.
	*/
	XnStatus AddSupportedStream(const char* StreamType);

	/**
	* Creates a stream.
	*
	* @param	StreamType	[in]	Type of the stream to create.
	* @param	StreamName	[in]	The name of the new stream.
	*/
	virtual XnStatus CreateStreamModule(const char* StreamType, const char* StreamName, XnDeviceModuleHolder** ppStreamHolder) = 0;

	virtual void DestroyStreamModule(XnDeviceModuleHolder* pStreamHolder) = 0;

	/**
	* Gets the required output size of a stream.
	*/
	XnStatus GetStreamRequiredDataSize(const char* StreamName, uint32_t* pnRequiredSize);

	/**
	* Gets the list of modules the device supports.
	*
	* @param	aModules	[out]	an array of modules.
	* @param	pnModules	[out]	The number of modules.
	*/
	XnStatus GetModulesList(XnDeviceModuleHolder** apModules, uint32_t* pnCount);
	XnStatus GetModulesList(std::list<XnDeviceModuleHolder*>& list);

	XnStatus GetStreamsList(std::list<XnDeviceModuleHolder*>& list);

	/**
	* Raises the NewStreamData event.
	*
	* @param	StreamName	[in]	The name of the stream with new data.
	*/
	XnStatus RaiseNewStreamDataEvent(const char* StreamName, OniFrame* pFrame);

	/** Gets called when a stream has new data. */
	virtual void OnNewStreamData(XnDeviceStream* pStream, OniFrame* pFrame);

	XnStatus ValidateOnlyModule(const XnPropertySet* pSet, const char* StreamName);

private:
	void FreeModuleRegisteredProperties(const char* strModule);

	static XnStatus XN_CALLBACK_TYPE PropertyValueChangedCallback(const XnProperty* pSender, void* pCookie);
	static XnStatus XN_CALLBACK_TYPE SetMirrorCallback(XnActualIntProperty* pSender, uint64_t nValue, void* pCookie);

	static void NewStreamDataCallback(XnDeviceStream* pSender, OniFrame* pFrame, void* pCookie);

	XnDeviceModuleHolder* m_pDevicePropertiesHolder;
	XnActualIntProperty m_DeviceMirror;

	static XnStatus XN_CALLBACK_TYPE StreamNewDataCallback(XnDeviceStream* pStream, void* pCookie);

	typedef xnl::XnStringsHashT<XnDeviceModuleHolder*> ModuleHoldersHash;
	ModuleHoldersHash m_Modules;

	std::set<std::string> m_SupportedStreams;

	struct XnPropertyCallback
	{
		XnPropertyCallback(const char* strModule, uint32_t propertyId, XnDeviceOnPropertyChangedEventHandler pHandler, void* pCookie);

		char strModule[XN_DEVICE_MAX_STRING_LENGTH];
		uint32_t propertyId;
		void* pCookie;
		XnDeviceOnPropertyChangedEventHandler pHandler;
		XnCallbackHandle hCallback;
	};

	std::list<XnPropertyCallback*> m_PropertyCallbacks;

	NewStreamDataEvent m_OnNewStreamDataEvent;

	XnDumpFile* m_StreamsDataDump;

	XN_CRITICAL_SECTION_HANDLE m_hLock;
};

#endif // XNDEVICEBASE_H
