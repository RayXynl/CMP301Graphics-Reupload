// Height Map Vertex Shader

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
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
};

float getHeight(float2 uv)
{
    // return the height values stored in the red colour channel
    return texture0.SampleLevel(sampler0, uv, 0).rgb;
}

OutputType main(InputType input)
{
    OutputType output;

    // get height from the height map texture using the texture coords
    float height = getHeight(input.tex);

    // Sample heights from neighboring texels for central finite differences
    float heightLeft = getHeight(input.tex - float2(1.0, 0));
    float heightRight = getHeight(input.tex + float2(1.0, 0));
    float heightUp = getHeight(input.tex - float2(0, 1.0));
    float heightDown = getHeight(input.tex + float2(0, 1.0));

    // Calculate the partial derivatives in the x and y directions
    float dx = (heightRight - heightLeft) / 2.0;
    float dy = (heightDown - heightUp) / 2.0;

    // Calculate the normal using the partial derivatives
    float3 normal = normalize(float3(-dx, -dy, 1.0));
    
    // Change the height the texture based off the previously calculated height
    output.position = float4(input.position.x, input.position.y + height * 50.f, input.position.z, 1.0);

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

   // Store the texture coordinates for the pixel shader
    output.tex = input.tex;
    output.normal = normal;

    return output;
}
