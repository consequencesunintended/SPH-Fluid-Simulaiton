#ifndef __MATH_SQUARE_ROOT__
	#define __MATH_SQUARE_ROOT__

	// -- LOCAL

	// .. REFERENCES

	#include "fundamental_data_types.h"

	// -- GLOBAL

	// .. CONSTANTS

	// .. TYPES

	class MATH_SQUARE_ROOT
	{
		// -- PUBLIC

	public:

		// .. CONSTRUCTORS

		MATH_SQUARE_ROOT(
			VOID
			)
		{
		}

		// ~~

		MATH_SQUARE_ROOT(
			MATH_SQUARE_ROOT & other
			)
		{
		}

		// ~~

		virtual ~MATH_SQUARE_ROOT(
			VOID
			)
		{
		}

		// .. OPERATORS

		MATH_SQUARE_ROOT & operator=(
			const MATH_SQUARE_ROOT & other
			)
		{
			return *this;
		}

		// .. FUNCTIONS

		// The Code and Algorithm used here is
		// based on :
		// http://ilab.usc.edu/wiki/index.php/Fast_Square_Root
		// this method is faster than the normal square root
		// used in the standard library.

		static REAL32 GetSquareRoot(
			const REAL32 value
			)
		{
			union
			{
				INT32 i;
				REAL32 value;
			} u;
			u.value = value;
			u.i = ( 1 << 29 ) + ( u.i >> 1 ) - ( 1 << 22 ); 
			return u.value;
		}
	};
#endif