#include "ParticleBase.hlsli"

VS_OUT main( uint vertexID : SV_VERTEXID )
{
	VS_OUT vout;
	vout.vertexID = vertexID;
	return vout;
}