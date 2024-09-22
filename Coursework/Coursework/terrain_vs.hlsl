// Terrain Vertex Shader
// Create terrain using perlin noise algorithm
// References : 
// de Carpienter, G. (2011). Scape: 2. Procedural basics [Article]. Available at: https://www.decarpentier.nl/scape-procedural-basics (Accessed: November 14, 2023).
// Atrefeu. (2021). How to handle octave & frequency in the Perlin noise algorithm? [Online Forum]. Available at : https://gamedev.stackexchange.com/questions/197861/how-to-handle-octave-frequency-in-the-perlin-noise-algorithm (Accessed: November 14, 2023).
// Mount, D. (2018). CMSC 425: Lecture 14 Procedural Generation: Perlin Noise [PDF]. Available at: https://www.cs.umd.edu/class/fall2018/cmsc425/Lects/lect14-perlin.pdf (Accessed: November 14, 2023).


cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewMatrix;
    matrix lightProjectionMatrix;
};

cbuffer TerrainBuffer : register(b1)
{
    float frequency;
    float amplitude;
    float persistence;
    float octaves;
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
    float height : TEXCOORD2;
    float4 lightViewPos : TEXCOORD3;
    
};

float fade(float t)
{
    // Cubic functions allows the gradient effect to 'fade' as we get further away from the vertex
    return t * t * t * (t * (t * 6 - 15) + 10);
}

// Generate gradient noise 
float gradient(int hash, float x, float y)
{
    int h = hash & 7;
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : x;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}


float noise(float x, float y, float frequency, int seed)
{
    int p[256]; // Create permutation array
    
    // Fill permutation array
    for (int i = 0; i < 256; i++)
    {
        p[i] = (i * seed) % 256;
    }
    
    int tOctaves = octaves;
    float lacunarity = 1.4;
    float value = 0;
    float tAmplitude = amplitude;
    
    
    for (int j = 0; j < tOctaves; j++)
    {
        // Calculate integer coords
        int X = (int) floor(x * frequency) & 255;
        int Y = (int) floor(y * frequency) & 255;
    
        // Find relative X & Y of point the square
        x += floor(x);
        y += floor(y);
    
        // Compute fade curves for x and y
        float u = fade(x);
        float v = fade(y);
    
        // Hash Coords
        int A = p[X] + Y;
        int AA = p[A];
        int AB = p[A + 1];
    
        int B = p[X + 1] + Y;
        int BA = p[B];
        int BB = p[B + 1];
      
        // Multiply the perlin values by the amplitude and frequency and add it to the final y value
        value += frequency * tAmplitude * lerp(v, lerp(u, gradient(p[AA], x, y), gradient(p[BA], x - 1, y)),
                            lerp(u, gradient(p[AB], x, y - 1), gradient(p[BB], x - 1, y - 1))),
                            lerp(v, lerp(u, gradient(p[AA + 1], x, y), gradient(p[BA + 1], x - 1, y)),
                            lerp(u, gradient(p[AB + 1], x, y - 1), gradient(p[BB + 1], x - 1, y - 1)));
        tAmplitude *= persistence; // Multiply ampltiude by persistence
        frequency *= lacunarity; // Multiply frequency by lacunarity
    }
    
  
    // return a value between 2 and -2
    return 4.0f * value - 2.0f;
}



OutputType main(InputType input)
{
    OutputType output;
    
    // Calculate height 
    output.height = noise(input.tex.x, input.tex.y, frequency, 77); //77
    
    // if the height is below a certain threshold multiply it for space for water
    if (output.height < 0.3)
    {
        output.height *= 5;
    }
    
    // Apply the height to each vertex
    output.position = float4(input.position.x, input.position.y + output.height * 2.5, input.position.z, 1.0);
    
    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Calculate the position of the vertice as viewed by the light source.
    output.lightViewPos = mul(input.position, worldMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);
    
    // Store the texture coordinates for the pixel shader
    output.tex = input.tex;
    
    // Calculate the normal vector against the world matrix only and normalise
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);
    
    // Calculate the world position vector against the world matrix
    output.worldPosition = mul(input.position, worldMatrix).xyz;
    
    return output;
}
