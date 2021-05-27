/*
Alex Nasser
http://www.alexnasser.co.uk
Winter 2011
*/
// This is an integration based on the paper published by
// "CLAVET, S., BEAUDOIN, P., AND POULIN, P. 2005. Particle-based 
// viscoelastic ﬂuid simulation. In SCA 2005, 219–228" A link to 
// view the paper  is avaiable here 
// http://www.iro.umontreal.ca/labs/infographie/papers/Clavet-2005-PVFS/pvfs.pdf

#include "physics_integration_prediction_relaxation.h"

// -- LOCAL

// .. REFERENCES

#include "math_vector_2D.h"
#include "fundamental_data_types.h"
#include "physics_integration.h"
#include "physics_entity.h"

// -- PUBLIC

// .. OPERATIONS

VOID PHSYICS_INTEGRATION_PREDICTION_RELAXATION::UpdateVelocity(
	PHYSICS_ENTITY & game_entity,
	const REAL32 delta_time
	)
{
	MATH_VECTOR_2D new_position;
		
	new_position 
		= game_entity.GetPosition() 
		+ game_entity.GetForce() * delta_time;
	game_entity.SetVelocity( (
		new_position
		- game_entity.GetOldPosition()
		) / delta_time
		);
}

// ~~

VOID PHSYICS_INTEGRATION_PREDICTION_RELAXATION::UpdatePosition(
	PHYSICS_ENTITY & game_entity,
	const REAL32 delta_time
	)
{
	game_entity.SetPosition( game_entity.GetPosition() 
		+ game_entity.GetForce() 
		* delta_time );
	game_entity.SetOldPosition( game_entity.GetPosition() );
	game_entity.SetPosition( 
		game_entity.GetPosition()
		+ game_entity.GetVelocity() * delta_time );
}