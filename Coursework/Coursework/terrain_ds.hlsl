// Terrain Domain Shader

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
};

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct InputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float height : TEXCOORD2;
    float4 lightViewPos : TEXCOORD3;
    float4 colour : COLOR;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float height : TEXCOORD2;
    float4 lightViewPos : TEXCOORD3;
    float4 colour : COLOR;
};

[domain("tri")]
OutputType main(ConstantOutputType input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 3> patch)
{
    float3 vertexPosition;
    OutputType output;
 
    // Determine the position of the new vertex.
	// Invert the y and Z components of uvwCoord as these coords are generated in UV space and therefore y is positive downward.
	// Alternatively you can set the output topology of the hull shader to cw instead of ccw (or vice versa).
    vertexPosition = uvwCoord.x * patch[0].position + -uvwCoord.y * patch[1].position + -uvwCoord.z * patch[2].position;
		    
    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.lightViewPos = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);
    
    output.worldPosition = mul(float4(vertexPosition, 1.0f), worldMatrix).xyz;
    
    // Send the input color into the pixel shader.
    output.colour = patch[0].colour;

    // Transfer other data
    output.tex = patch[0].tex;

    output.worldPosition = patch[0].worldPosition;
    output.height = patch[0].height;


    
    // Store the texture coordinates for the pixel shader
  //  output.tex = input.tex;
    
    // Calculate the normal vector against the world matrix only and normalise
  //  output.normal = mul(input.normal, (float3x3) worldMatrix);
  //  output.normal = normalize(output.normal);
    
    // Calculate the world position vector against the world matrix
  //  output.worldPosition = mul(input.position, worldMatrix).xyz;


    return output;
}
