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
#ifndef XNPROPERTYSET_H
#define XNPROPERTYSET_H

//---------------------------------------------------------------------------
// Includes
//---------------------------------------------------------------------------
#include <Driver/OniDriverAPI.h>
#include <XnPlatform.h>
#include <XnDDK.h>

//---------------------------------------------------------------------------
// Types
//---------------------------------------------------------------------------
/** The type of the property. */
typedef enum XnPropertyType
{
	XN_PROPERTY_TYPE_INTEGER,
	XN_PROPERTY_TYPE_REAL,
	XN_PROPERTY_TYPE_STRING,
	XN_PROPERTY_TYPE_GENERAL,
} XnPropertyType;

struct XnPropertySet; // Forward Declaration
typedef struct XnPropertySet XnPropertySet;

struct XnPropertySetModuleEnumerator; // Forward Declaration
typedef struct XnPropertySetModuleEnumerator XnPropertySetModuleEnumerator;

struct XnPropertySetEnumerator; // Forward Declaration
typedef struct XnPropertySetEnumerator XnPropertySetEnumerator;

//---------------------------------------------------------------------------
// Exported functions
//---------------------------------------------------------------------------

/**
* Creates a new property set.
*
* @param	ppSet			[out]		A pointer to the new set.
*/
XnStatus XnPropertySetCreate(XnPropertySet** ppSet);

/**
* Destroys a previously created property set.
*
* @param	ppSet			[in/out]	A pointer to the set.
*/
XnStatus XnPropertySetDestroy(XnPropertySet** ppSet);

/**
* Clears a property set from all the properties.
*
* @param	pSet			[in]		The property set.
*/
XnStatus XnPropertySetClear(XnPropertySet* pSet);

/**
* Adds a module to the property set.
*
* @param	pSet			[in]		The property set.
* @param	strModuleName	[in]		Name of the module to add.
*/
XnStatus XnPropertySetAddModule(XnPropertySet* pSet, const char* strModuleName);

/**
* Removes a module from the property set.
*
* @param	pSet			[in]		The property set.
* @param	strModuleName	[in]		Name of the module to remove.
*/
XnStatus XnPropertySetRemoveModule(XnPropertySet* pSet, const char* strModuleName);

/**
* Adds an integer property to the property set.
*
* @param	pSet			[in]		The property set.
* @param	strModuleName	[in]		Name of the module.
* @param	strModuleName	[in]		Name of the property to add.
* @param	nValue			[in]		Value for that property.
*/
XnStatus XnPropertySetAddIntProperty(XnPropertySet* pSet, const char* strModuleName, uint32_t propertyId, uint64_t nValue);

/**
* Adds an real property to the property set.
*
* @param	pSet			[in]		The property set.
* @param	strModuleName	[in]		Name of the module.
* @param	strModuleName	[in]		Name of the property to add.
* @param	dValue			[in]		Value for that property.
*/
XnStatus XnPropertySetAddRealProperty(XnPropertySet* pSet, const char* strModuleName, uint32_t propertyId, double dValue);

/**
* Adds an string property to the property set.
*
* @param	pSet			[in]		The property set.
* @param	strModuleName	[in]		Name of the module.
* @param	strModuleName	[in]		Name of the property to add.
* @param	strValue			[in]		Value for that property.
*/
XnStatus XnPropertySetAddStringProperty(XnPropertySet* pSet, const char* strModuleName, uint32_t propertyId, const char* strValue);

/**
* Adds an general property to the property set.
*
* @param	pSet			[in]		The property set.
* @param	strModuleName	[in]		Name of the module.
* @param	strModuleName	[in]		Name of the property to add.
* @param	pgbValue			[in]		Value for that property.
*/
XnStatus XnPropertySetAddGeneralProperty(XnPropertySet* pSet, const char* strModuleName, uint32_t propertyId, const OniGeneralBuffer* pgbValue);

/**
* Removes a property from the property set.
*
* @param	pSet			[in]		The property set.
* @param	strModuleName	[in]		Name of the module.
* @param	strModuleName	[in]		Name of the property to remove.
*/
XnStatus XnPropertySetRemoveProperty(XnPropertySet* pSet, const char* strModuleName, uint32_t propertyId);

/**
* Gets a modules enumerator. This enumerator should be freed using XnPropertySetModuleEnumeratorFree.
*
* @param	pSet			[in]		The property set.
* @param	ppEnumerator	[out]		The created enumerator.
*/
XnStatus XnPropertySetGetModuleEnumerator(const XnPropertySet* pSet, XnPropertySetModuleEnumerator** ppEnumerator);

/**
* Frees a previously created module enumerator.
*
* @param	ppEnumerator	[in/out]	The enumerator.
*/
XnStatus XnPropertySetModuleEnumeratorFree(XnPropertySetModuleEnumerator** ppEnumer);

/**
* Moves the enumerator to the next module. This function must be called *before* getting current.
*
* @param	pEnumerator		[in]		The enumerator.
* @param	pbEnd			[out]		true if the enumerator has reached the end of the collection.
*/
XnStatus XnPropertySetModuleEnumeratorMoveNext(XnPropertySetModuleEnumerator* pEnumerator, bool* pbEnd);

/**
* Gets the current module name from the enumerator.
*
* @param	pEnumerator		[in]		The enumerator.
* @param	pstrModuleName	[out]		The name of the current module.
*/
XnStatus XnPropertySetModuleEnumeratorGetCurrent(const XnPropertySetModuleEnumerator* pEnumer, const char** pstrModuleName);

/**
* Gets a property enumerator. This enumerator must be freed using XnPropertySetEnumeratorFree.
*
* @param	pSet			[in]			The property set.
* @param	ppEnumerator	[in/out]		The enumerator.
* @param	strModule		[in]			Optional. When provided, only properties of this module will be enumerated.
*/
XnStatus XnPropertySetGetEnumerator(const XnPropertySet* pSet, XnPropertySetEnumerator** ppEnumerator, const char* strModule = NULL);

/**
* Finds a property according to its name and module, and returns an enumerator to it.
* This enumerator must be freed using XnPropertySetEnumeratorFree.
*
* @param	pSet			[in]			The property set.
* @param	strModule		[in]			The module name.
* @param	strProp 		[in]			The property name.
* @param	ppEnumerator	[in/out]		The enumerator.
*/
XnStatus XnPropertySetFindProperty(const XnPropertySet* pSet, const char* strModule, uint32_t propertyId, XnPropertySetEnumerator** ppEnumerator);

/**
* Frees a previously created properties enumerator.
*
* @param	ppEnumerator	[in/out]	The enumerator.
*/
XnStatus XnPropertySetEnumeratorFree(XnPropertySetEnumerator** ppEnumerator);

/**
* Moves the enumerator to the next property. This function must be called *before* getting current.
*
* @param	pEnumerator		[in]		The enumerator.
* @param	pbEnd			[out]		true if the enumerator has reached the end of the collection.
*/
XnStatus XnPropertySetEnumeratorMoveNext(XnPropertySetEnumerator* pEnumerator, bool* pbEnd);

/**
* Gets information regarding the current property.
*
* @param	pEnumerator		[in]		The enumerator.
* @param	pnType			[out]		The type of the current property.
* @param	pstrModule		[out]		The module of the current property.
* @param	pstrProp		[out]		The name of the current property.
*/
XnStatus XnPropertySetEnumeratorGetCurrentPropertyInfo(const XnPropertySetEnumerator* pEnumerator, XnPropertyType* pnType, const char** pstrModule, uint32_t* pPropertyId);

/**
* Gets the current integer value.
*
* @param	pEnumerator			[in]		The enumerator.
* @param	pnValue 			[out]		The value of the property.
*/
XnStatus XnPropertySetEnumeratorGetIntValue(const XnPropertySetEnumerator* pEnumerator, uint64_t* pnValue);

/**
* Gets the current real property.
*
* @param	pEnumerator			[in]		The enumerator.
* @param	pdValue 			[out]		The value of the property.
*/
XnStatus XnPropertySetEnumeratorGetRealValue(const XnPropertySetEnumerator* pEnumerator, double* pdValue);

/**
* Gets the current string property.
*
* @param	pEnumerator			[in]		The enumerator.
* @param	pstrValue			[out]		The value of the property.
*/
XnStatus XnPropertySetEnumeratorGetStringValue(const XnPropertySetEnumerator* pEnumerator, const char** pstrValue);

/**
* Gets the current general property.
*
* @param	pEnumerator			[in]		The enumerator.
* @param	pgbValue			[out]		The value of the property.
*/
XnStatus XnPropertySetEnumeratorGetGeneralValue(const XnPropertySetEnumerator* pEnumerator, OniGeneralBuffer* pgbValue);

#endif // XNPROPERTYSET_H
