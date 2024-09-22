cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
};

cbuffer SineWaveBuffer : register(b1)
{
    float time;
    float amplitude;
    float frequency;
    float speed;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float4 lightViewPos : TEXCOORD2;
};

OutputType main(InputType input)
{
    OutputType output;

    // Create 3 different waves to be combined to simulate water
    float wave1 = sin((input.position.x + time * speed) * frequency) * amplitude * 0.5f;
    float wave2 = cos((input.position.z + time * speed) * frequency) * amplitude * 0.5f;
    float wave3 = -sin((input.position.y + time * speed) * frequency) * amplitude * 0.5f;

    // Combine the 3 waves to simulate the water effect
    float waves = (wave1 + wave2 + wave3) / 3;
    
    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    output.lightViewPos = mul(input.position, worldMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);

    // Store the texture coordinates for the pixel shader
    output.tex = input.tex;
     // Apply wave effect to the y component of each vertex
    output.position.y += waves;
    // Calculate the normal vector against the world matrix only and normalize
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    
    output.worldPosition = mul(input.position, worldMatrix).xyz;

    return output;
}
