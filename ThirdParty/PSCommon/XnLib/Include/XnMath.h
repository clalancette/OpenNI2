/*****************************************************************************
*                                                                            *
*  PrimeSense PSCommon Library                                               *
*  Copyright (C) 2012 PrimeSense Ltd.                                        *
*                                                                            *
*  This file is part of PSCommon.                                            *
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
#ifndef _XN_MATH_H_
#define _XN_MATH_H_

#include <math.h>
#include <XnPlatform.h>

namespace xnl
{

	namespace Math
	{
		inline int32_t Abs(int32_t i)
		{
			return abs(i);
		}
		inline float Abs(float f)
		{
			return fabs(f);
		}
		template <class T>
		XnBool IsZero(T value, T tolerance)
		{
			return Abs(value) < tolerance;
		}
		inline float Sqr(float f)
		{
			return f*f;
		}
		inline float Sqrt(float f)
		{
			return sqrt(f);
		}
		inline XnDouble Sqrt(XnDouble f)
		{
			return sqrt(f);
		}
		template <class T>
		T Min(T value1, T value2)
		{
			return value1 < value2 ? value1 : value2;
		}
		template <class T>
		T Max(T value1, T value2)
		{
			return value1 > value2 ? value1 : value2;
		}
		template<class T>
		T Max(const T a, const T b, const T c)
		{
			return Max(a, Max(b,c));
		}
		template<class T>
		T Max(const T a, const T b, const T c, const T d)
		{
			return Max(a, Max(b,c,d));
		}

		template <class T>
		T Crop(T value, T upper, T lower)
		{
			return Min(upper, Max(lower, value));
		}
		template <class T>
		XnBool IsBetween(T value, T upper, T lower)
		{
			return value < upper && value > lower;
		}
		template<class T>
		int32_t ArgMax(const T a, const T b) {
			return (a>b) ? 0 : 1;
		}
		template<class T>
		int32_t ArgMax(const T a, const T b, const T c) {
			return (a>b) ? ((a>c) ? 0 : 2) : ((b>c) ? 1 : 2);
		}
		template<class T>
		int32_t ArgMax(const T a, const T b, const T c, const T d) {
			return (a>d) ? ArgMax(a,b,c) : ArgMax(b,c,d)+1;
		}
		template<class T>
		int32_t ArgMin(const T a, const T b) {
			return (a<b) ? 0 : 1;
		}
		template<class T>
		int32_t ArgMin(const T a, const T b, const T c) {
			return (a<b) ? ((a<c) ? 0 : 2) : ((b<c) ? 1 : 2);
		}

		template<class T> T MaxAbs(const T a, const T b) { return Max(Abs(a),Abs(b)); }
		template<class T> T MaxAbs(const T a, const T b, const T c) { return Max(Abs(a),Abs(b),Abs(c)); }
		template<class T> T MaxAbs(const T a, const T b, const T c, const T d) { return Max(Abs(a),Abs(b),Abs(c),Abs(d)); }

		template<class T>
		void Exchange(T &a, T &b) { T c=a; a=b; b=c; }

		template<class T>
		void Swap(T &a, T &b) { T c=a; a=b; b=c; }

		template<class T>
		void ExchangeSort(T &a, T &b)
		{
			if(a > b) Exchange(a,b);
		}

		template<class T>
		void ExchangeSort(T &a, T &b, T &c)
		{
			if(a > b) Exchange(a,b);
			if(b > c) Exchange(b,c);
			if(a > b) Exchange(a,b);
		}

		template<class T>
		struct OneOverSqrtHelper
		{
			static T OneOverSqrt(T MagSq) {return T(1.0)/Sqrt(MagSq); }
		};
		template<class T>
		inline XnBool IsNaN(const T& scalar)
		{
#if defined(_WIN32)
			return _isnan(scalar)!=0;
#else
			return isnan(scalar);
#endif
		}

#define round(x) ((int32_t)floor((x)+0.5f))

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
		static const float ONE_THIRD = (float)(1.0f/3.0f);
		static const float ONE_SIXTH = (float)(1.0f/6.0f);
		static const float ROOT_TWO = (float)Sqrt(2.0f);
		static const float ROOT_THREE = (float)Sqrt(3.0f);
		static const float PI = (float)M_PI;
		static const float HALF_PI = (float)(0.5f*M_PI);
		static const float TWO_PI = (float)(2.0f*M_PI);
		static const float ROOT_TWO_PI = (float)Sqrt(2.0f*M_PI);
		static const float DTR = (float)(M_PI / 180.0f);
		static const float RTD = (float)(180.0f / M_PI);
		static const float PHI = (float)((-1.0f + Sqrt(5.0f)) / 2.0f);

	} // Math
} // xnl

#endif // _XN_MATH_H_
