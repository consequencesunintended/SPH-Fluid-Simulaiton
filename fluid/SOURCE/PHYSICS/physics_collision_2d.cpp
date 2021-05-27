/*
Alex Nasser
http://www.alexnasser.co.uk
Winter 2011
*/
#include "physics_collision_2d.h"

// -- LOCAL

// .. REFERENCES

#include "fundamental_data_types.h"
#include "physics_entity.h"

// -- PUBLIC

// .. OPERATIONS

VOID PHYSICS_COLLISION_2D::CheckForWall(
			PHYSICS_ENTITY &entity,
			const REAL32 wall_spring_coefficient,
			const MATH_POINT_2D & position,
			const REAL32 min_x_value,
			const REAL32 max_x_value,
			const REAL32 min_y_value,
			const REAL32 max_y_value
			)
{
	if( entity.GetPosition().X < min_x_value + position.X ) 
	{
		entity.SetForce( 
			MATH_VECTOR_2D( 
				entity.GetForce().X - ( entity.GetPosition().X - ( min_x_value + position.X ) ) * wall_spring_coefficient,
				entity.GetForce().Y 
				)
			);				
	}

	if( entity.GetPosition().X >  max_x_value + position.X ) 
	{
		entity.SetForce( 
			MATH_VECTOR_2D(
				entity.GetForce().X - (entity.GetPosition().X - ( max_x_value + position.X ) ) * wall_spring_coefficient,
				entity.GetForce().Y 
				)
			);		
	}

	if( entity.GetPosition().Y < min_y_value + position.Y ) 
	{
		entity.SetForce( 
			MATH_VECTOR_2D(
				entity.GetForce().X,
				entity.GetForce().Y - (entity.GetPosition().Y - ( min_y_value + position.Y ) ) * wall_spring_coefficient
				)
			);
	}

	if( entity.GetPosition().Y > max_y_value + position.Y )
	{
		entity.SetForce( 
			MATH_VECTOR_2D(
				entity.GetForce().X,
				entity.GetForce().Y - (entity.GetPosition().Y - ( max_y_value + position.Y ) ) * wall_spring_coefficient
				)
			);
	}
}