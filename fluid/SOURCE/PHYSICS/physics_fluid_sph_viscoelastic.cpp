/*
Alex Nasser
http://www.alexnasser.co.uk
Winter 2011
*/
// This class provide the main functions for a Fluid Simulation
// the main algorithm used here is based on a this paper 
// "CLAVET, S., BEAUDOIN, P., AND POULIN, P. 2005. Particle-based 
// viscoelastic Fuid simulation. In SCA 2005, 219�228" There are
// some minor changes in the algorithm which I assume in the paper
// was written differently due to typos. A link to view the paper
// is avaiable here 
// http://www.iro.umontreal.ca/labs/infographie/papers/Clavet-2005-PVFS/pvfs.pdf
// the class can be used for a 3D simulation as well the only changes
// needed is changing the vector class to a 3D vector,the rest of the
// code don't need any changes.

#include "physics_fluid_sph_viscoelastic.h"

// -- LOCAL

// .. REFERENCES

#include "fundamental_data_types.h"
#include "physics_fluid_particle.h"
#include "physics_fluid_particle_array.h"
#include "physics_fluid_neighbour_particle.h"
#include "math_vector_2d.h"
#include "physics_spring.h"
#include "physics_spring_array.h"
#include "math_square_root.h"

// -- PUBLIC

// .. OPERATIONS

VOID PHSYICS_FLUID_SPH_VISCOELASTIC::InitialiseSpringTable(
	const INT32 number_of_particles
	)
{
	SpringTable.SetMaxNumberOfPoints( number_of_particles );
}

// ~~

VOID PHSYICS_FLUID_SPH_VISCOELASTIC::CalculateDensity(
	PHYSICS_FLUID_PARTICLE_ARRAY & particles_table, 
	const REAL32 smoothing_radius
	)
{
	INDEX 
		particle_index,
		neighbour_index;
	REAL32 
		density,
		near_density,
		squared_distance_between_particle_and_neighbour,
		distance_between_particle_and_neighbour,
		smoothing_kernel,
		powered_two_smoothing_kernel,
		powered_three_smoothing_kernel;
	MATH_VECTOR_2D vector_between_particle_and_neighbour;
	PHYSICS_FLUID_NEIGHBOUR_PARTICLE neighbour_particle;

	particle_index = 0;

	FUNDAMENTAL_DATA_TYPES_loop_through_table( particle_index, particles_table )
	{
		particles_table[particle_index].SetDensity( 0.0f );
		particles_table[particle_index].SetNearDensity( 0.0f );
		particles_table[particle_index].GetNeighbours().clear();
		density = 0;
		near_density = 0;
		neighbour_index = particle_index+1;

		FUNDAMENTAL_DATA_TYPES_loop_through_table( neighbour_index, particles_table )
		{		
			vector_between_particle_and_neighbour.SetDifference( 
				particles_table[particle_index].GetPosition(), 
				particles_table[neighbour_index].GetPosition() 
				);
			squared_distance_between_particle_and_neighbour 
				= vector_between_particle_and_neighbour.GetSquareLength();

			if( squared_distance_between_particle_and_neighbour 
				< smoothing_radius * smoothing_radius )
			{
				distance_between_particle_and_neighbour 
					= MATH_SQUARE_ROOT::GetSquareRoot( squared_distance_between_particle_and_neighbour );
				smoothing_kernel 
					= 1 - distance_between_particle_and_neighbour / smoothing_radius;
				powered_two_smoothing_kernel = smoothing_kernel * smoothing_kernel;
				powered_three_smoothing_kernel = powered_two_smoothing_kernel * smoothing_kernel;
				density  += powered_two_smoothing_kernel;
				near_density += powered_three_smoothing_kernel;
				particles_table[neighbour_index].SetDensity( 
					particles_table[neighbour_index].GetDensity() 
					+ powered_two_smoothing_kernel 
					);
				particles_table[neighbour_index].SetNearDensity( 
					particles_table[neighbour_index].GetNearDensity() 
					+ powered_three_smoothing_kernel 
					);	
				neighbour_particle.SetParticleIndex( neighbour_index );
				neighbour_particle.SetSmoothingKernel( smoothing_kernel ); 
				neighbour_particle.SetPoweredTwoSmoothingKernel( powered_two_smoothing_kernel );
				neighbour_particle.SetDistance( distance_between_particle_and_neighbour );
				particles_table[particle_index].GetNeighbours().push_back(neighbour_particle);				
			}
		}
		particles_table[particle_index].SetDensity( 
			particles_table[particle_index].GetDensity() 
			+ density );
		particles_table[particle_index].SetNearDensity( 
			particles_table[particle_index].GetNearDensity() 
			+ near_density );
	}
}

// ~~

VOID PHSYICS_FLUID_SPH_VISCOELASTIC::CalculatePressure(
	PHYSICS_FLUID_PARTICLE_ARRAY & particlesTable,
	const REAL32 stifness_parameter,
	const REAL32 near_stifness_parameter,
	const REAL32 rest_density,
	const REAL32 delta_time
	)
{
	INDEX 
		prticle_index,
		neighbour_table_index,
		neighbour_index;
	MATH_VECTOR_2D 
		particle_pressure_force,
		vector_between_particle_and_neighbour,
		neighbour_pressure_force;
	PHYSICS_FLUID_NEIGHBOUR_PARTICLE neigbour_particle;
	REAL32 
		pressure,
		near_pressure,
		smoothing_kernel,
		powered_two_smoothing_kernel,
		pressure_scalar_value;

	prticle_index = 0;

	FUNDAMENTAL_DATA_TYPES_loop_through_table( prticle_index, particlesTable )
	{
		pressure = 
			stifness_parameter 
			* (particlesTable[prticle_index].GetDensity() - rest_density);
		near_pressure =  
			near_stifness_parameter 
			* particlesTable[prticle_index].GetNearDensity();
		particle_pressure_force.Reset();
		neighbour_table_index = 0;

		FUNDAMENTAL_DATA_TYPES_loop_through_table( 
			neighbour_table_index, 
			particlesTable[prticle_index].GetNeighbours() 
			)
		{
			neigbour_particle 
				= particlesTable[prticle_index].GetNeighbours()[neighbour_table_index];
			neighbour_index = neigbour_particle.GetParticleIndex();     
			smoothing_kernel = neigbour_particle.GetSmoothingKernel();
			powered_two_smoothing_kernel = neigbour_particle.GetPoweredTwoSmoothingKernel();  
			vector_between_particle_and_neighbour.SetDifference( 
				particlesTable[prticle_index].GetPosition(), 
				particlesTable[neighbour_index].GetPosition() 
				);
			pressure_scalar_value 
				= (pressure) * smoothing_kernel 
				+ (near_pressure) * powered_two_smoothing_kernel;
			pressure_scalar_value *= delta_time 
				* delta_time 
				* PHSYICS_FLUID_SPH_VISCOELASTIC_delta_time_scaling_factor	      
			    * PHSYICS_FLUID_SPH_VISCOELASTIC_delta_time_scaling_factor;
			if ( neigbour_particle.GetDistance() != 0 )
			{
				neighbour_pressure_force 
					= vector_between_particle_and_neighbour
					/ neigbour_particle.GetDistance() * pressure_scalar_value; 
			}
			else
			{
				neighbour_pressure_force.Reset();
			}
			particlesTable[neighbour_index].AccumalateForce( neighbour_pressure_force );
			particle_pressure_force -= neighbour_pressure_force ;
		}
		particlesTable[prticle_index].AccumalateForce( particle_pressure_force );
	}
}

// ~~

VOID PHSYICS_FLUID_SPH_VISCOELASTIC::CalculateViscosity(
	PHYSICS_FLUID_PARTICLE_ARRAY & particlesTable,
	const REAL32 smoothing_radius,
	const REAL32 delta_time
	)
{
	INDEX 
		prticle_index,
		neighbour_table_index,
		neighbour_index;
	PHYSICS_FLUID_NEIGHBOUR_PARTICLE 
		neighbour_particle;
	REAL32 
		length,
		q,
		inward_radial_velocity;
	MATH_VECTOR_2D 
		vector_between_particle_and_neighbour,
		normalised_vector_between_particle_and_neighbour,
		impulses;

	prticle_index = 0;

	FUNDAMENTAL_DATA_TYPES_loop_through_table( prticle_index, particlesTable )
	{
		neighbour_table_index = 0;

		FUNDAMENTAL_DATA_TYPES_loop_through_table( 
			neighbour_table_index, 
			particlesTable[prticle_index].GetNeighbours() 
			)
		{
			neighbour_particle = particlesTable[prticle_index].GetNeighbours()[neighbour_table_index];
			neighbour_index = neighbour_particle.GetParticleIndex();    
			vector_between_particle_and_neighbour = particlesTable[neighbour_index].GetPosition() - particlesTable[prticle_index].GetPosition();
			length = neighbour_particle.GetDistance();		
			q = length / smoothing_radius;
      

			normalised_vector_between_particle_and_neighbour 
				= (vector_between_particle_and_neighbour / length);
			inward_radial_velocity  = (
				particlesTable[prticle_index].GetVelocity() 
				- particlesTable[neighbour_index].GetVelocity() 
				).GetDotProduct( normalised_vector_between_particle_and_neighbour );

			if( inward_radial_velocity > 0.0f)
			{
				impulses = 
					(normalised_vector_between_particle_and_neighbour 
					* (particlesTable[neighbour_index].GetViscositySigma() 
					* inward_radial_velocity + particlesTable[neighbour_index].GetViscosityBeta() 
					* inward_radial_velocity * inward_radial_velocity)) 
					* (1 - q);
				 impulses 
					 *= delta_time 
					 * PHSYICS_FLUID_SPH_VISCOELASTIC_delta_time_scaling_factor;
				 particlesTable[prticle_index].SetVelocity( 
					 particlesTable[prticle_index].GetVelocity() 
					 - impulses 
					 );
				 particlesTable[neighbour_index].SetVelocity( 
					 particlesTable[neighbour_index].GetVelocity()
					 + impulses 
					 );
			}
		}
	}
}

// ~~

VOID PHSYICS_FLUID_SPH_VISCOELASTIC::CalculateViscoElasticity(
	PHYSICS_FLUID_PARTICLE_ARRAY & particlesTable,
	const REAL32 smoothing_radius,
	const REAL32 coefficient_spring,
	const REAL32 yeild_ratio,
	const REAL32 plasticity_constant,
	const REAL32 delta_time
	)
{
	PHYSICS_FLUID_NEIGHBOUR_PARTICLE 
		neighbour_particle;
	PHYSICS_SPRING 
		spring;
	INDEX
		particle_index,
		neighbour_index,
		spring_index,
		spring_point1_index,
		spring_point2_index;
	REAL32 
		restLength,
		tolerable_deformation,
		distance_between_particle_and_neighbour;
	MATH_VECTOR_2D 		
		spring_force,
		spring_displacement; 

	particle_index = 0;

	FUNDAMENTAL_DATA_TYPES_loop_through_table( particle_index, particlesTable )
	{
		neighbour_index = 0;

		FUNDAMENTAL_DATA_TYPES_loop_through_table( neighbour_index, particlesTable[particle_index].GetNeighbours()  )
		{
			neighbour_particle = particlesTable[particle_index].GetNeighbours()[neighbour_index];
      
			if ( !SpringTable.GetExistenceTable()[particle_index][neighbour_particle.GetParticleIndex()] ) 
			{
				SpringTable.SetItemInExistenceTable( 
					particle_index, 
					neighbour_particle.GetParticleIndex(), 
					BOOLEAN_true 
					);
				spring.SetPoint1Index( particle_index );
				spring.SetPoint2Index( neighbour_particle.GetParticleIndex() );
				spring.SetRestLength( smoothing_radius );
				spring.SetCoefficient( coefficient_spring );
				SpringTable.push_back( spring );
			}
		}
	}
	spring_index = 0;

	FUNDAMENTAL_DATA_TYPES_loop_through_table( spring_index, SpringTable )
	{
		spring_point1_index = SpringTable[spring_index].GetPoint1Index();
		spring_point2_index = SpringTable[spring_index].GetPoint2Index();
		restLength = SpringTable[spring_index].GetRestLength();
		tolerable_deformation = yeild_ratio * restLength;
		distance_between_particle_and_neighbour 
			= (particlesTable[spring_point2_index].GetPosition() 
			- particlesTable[spring_point1_index].GetPosition()).GetLength();

		if ( distance_between_particle_and_neighbour > ( restLength + tolerable_deformation) )
		{
			SpringTable[spring_index].SetRestLength(
				SpringTable[spring_index].GetRestLength()
				+ ( delta_time * PHSYICS_FLUID_SPH_VISCOELASTIC_delta_time_scaling_factor )
				* plasticity_constant 
				* ( distance_between_particle_and_neighbour - restLength - tolerable_deformation ) ); 
		} 
		else if ( distance_between_particle_and_neighbour < (  restLength - tolerable_deformation ))
		{
			SpringTable[spring_index].SetRestLength(
				SpringTable[spring_index].GetRestLength()
				- ( delta_time * PHSYICS_FLUID_SPH_VISCOELASTIC_delta_time_scaling_factor )
				* plasticity_constant 
				* (restLength - tolerable_deformation - distance_between_particle_and_neighbour ) ); 
		}

		if ( SpringTable[spring_index].GetRestLength() > smoothing_radius )
		{
			SpringTable.SetItemInExistenceTable( 
				SpringTable[spring_index].GetPoint1Index(), 
				SpringTable[spring_index].GetPoint2Index(), 
				BOOLEAN_false 
				);
			SpringTable.erase( spring_index );
			spring_index--;
		}
	}

	spring_index = 0;

	FUNDAMENTAL_DATA_TYPES_loop_through_table( spring_index, SpringTable )
	{
		spring_point1_index = SpringTable[spring_index].GetPoint1Index();
		spring_point2_index = SpringTable[spring_index].GetPoint2Index();
		restLength = SpringTable[spring_index].GetRestLength();
		PHYSICS_SPRING::CalculateForce(
			spring_force,
			SpringTable[spring_index],
			particlesTable[spring_point1_index],
			particlesTable[spring_point2_index]
			);
		spring_displacement 
			= spring_force 
			* ( 1.0f - restLength / smoothing_radius ) 
			* delta_time * PHSYICS_FLUID_SPH_VISCOELASTIC_delta_time_scaling_factor 
			* delta_time * PHSYICS_FLUID_SPH_VISCOELASTIC_delta_time_scaling_factor;
		particlesTable[spring_point1_index].SetPosition( 
			particlesTable[spring_point1_index].GetPosition() 
			- spring_displacement 
			);
		particlesTable[spring_point2_index].SetPosition( 
			particlesTable[spring_point2_index].GetPosition() 
			+ spring_displacement 
			);
	}
}

// ~~

VOID PHSYICS_FLUID_SPH_VISCOELASTIC::ResetSprings(
	VOID
	)
{
	SpringTable.clear();
}


// ~~

VOID PHSYICS_FLUID_SPH_VISCOELASTIC::InitialisePlasticity(
	PHYSICS_FLUID_PARTICLE_ARRAY & particlesTable,
	const REAL32 smoothing_radius,
	const REAL32 coefficient_spring
	)
{
	PHYSICS_FLUID_NEIGHBOUR_PARTICLE 
		neighbour_particle;
	PHYSICS_SPRING 
		spring;
	INDEX
		particle_index_1,
		particle_index_2;
	REAL32 
		distance_between_particle_and_neighbour;
	MATH_VECTOR_2D 		
		vector_between_particle_and_neighbour;

	ResetSprings();

	particle_index_1 = 0;

	FUNDAMENTAL_DATA_TYPES_loop_through_table( particle_index_1, particlesTable )
	{
		particle_index_2 = 0;

		FUNDAMENTAL_DATA_TYPES_loop_through_table( particle_index_2, particlesTable  )
		{
  			vector_between_particle_and_neighbour.SetDifference( 
				particlesTable[ particle_index_1 ].GetPosition(),
				particlesTable[ particle_index_2 ].GetPosition()
				);
			distance_between_particle_and_neighbour 
				= vector_between_particle_and_neighbour.GetLength();

			if ( distance_between_particle_and_neighbour < smoothing_radius )
			{
				spring.SetPoint1Index( particle_index_1 );
				spring.SetPoint2Index( particle_index_2 );
				spring.SetRestLength( distance_between_particle_and_neighbour );
				spring.SetCoefficient( coefficient_spring );
				SpringTable.push_back( spring );
			}
		}
	}
}

// ~~

VOID PHSYICS_FLUID_SPH_VISCOELASTIC::CalculatePlasticity(
	PHYSICS_FLUID_PARTICLE_ARRAY & particlesTable,
	const REAL32 delta_time
	)
{
	PHYSICS_SPRING 
		spring;
	INDEX
		spring_index,
		spring_point1_index,
		spring_point2_index;
	REAL32 
		restLength;
	MATH_VECTOR_2D 		
		spring_force,
		spring_displacement; 

	spring_index = 0;

	FUNDAMENTAL_DATA_TYPES_loop_through_table( spring_index, SpringTable )
	{
		spring_point1_index = SpringTable[spring_index].GetPoint1Index();
		spring_point2_index = SpringTable[spring_index].GetPoint2Index();
		restLength = SpringTable[spring_index].GetRestLength();
		PHYSICS_SPRING::CalculateForce(
			spring_force,
			SpringTable[spring_index],
			particlesTable[spring_point1_index],
			particlesTable[spring_point2_index]
			);
		spring_displacement 
			= spring_force 
			* delta_time * PHSYICS_FLUID_SPH_VISCOELASTIC_delta_time_scaling_factor;
		particlesTable[spring_point1_index].SetPosition( 
			particlesTable[spring_point1_index].GetPosition() 
			- spring_displacement 
			);
		particlesTable[spring_point2_index].SetPosition( 
			particlesTable[spring_point2_index].GetPosition() 
			+ spring_displacement 
			);
	}
}