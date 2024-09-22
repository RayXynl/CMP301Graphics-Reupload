// Terrain Pixel shader
// Calculate lighting 
Texture2D depthMapTexture : register(t0);

SamplerState diffuseSampler : register(s0);
SamplerState shadowSampler : register(s1)
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Clamp;
    AddressV = Clamp;
    ComparisonFunc = LESS_EQUAL;
};

cbuffer LightBuffer : register(b0)
{
    float4 ambient;
    float4 diffuse;
    float3 direction;
    float padding;
};

cbuffer hOneLightBuffer : register(b1)
{
    float4 hOneAmbient;
    float4 hOneDiffuse;
    float3 hOneDirection;
    float hOnePadding;
    float3 hOnePosition;
    float hOnePadding_;
};

cbuffer hTwoLightBuffer : register(b2)
{
    float4 hTwoAmbient;
    float4 hTwoDiffuse;
    float3 hTwoDirection;
    float hTwoPadding;
    float3 hTwoPosition;
    float hTwoPadding_;
};

cbuffer hThreeLightBuffer : register(b3)
{
    float4 hThreeAmbient;
    float4 hThreeDiffuse;
    float3 hThreeDirection;
    float hThreePadding;
    float3 hThreePosition;
    float hThreePadding_;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float height : TEXCOORD2;
    float4 lightViewPos : TEXCOORD3;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(diffuse * intensity);
    return colour;
}

float3 calculateTerrain(float height)
{
    // Create 2 rock colours to be interpolated between to create a textured looking rock colour
    float3 rockColour = float3(0.4, 0.4, 0.4); // Rock Colour - ligther grey
    float3 rockColour2 = float3(0.3, 0.3, 0.3); // Rock Colour 2 - darker grey

    // Create 3 different greens to be interpolated between to create a textured looking grass colour
    float3 grassColour = float3(0.2, 0.2, 0.2); // Grass Colour - darkest green colour
    float3 grassColour2 = float3(0.2, 0.3, 0.2); // Grass Colour 2 - middle green colour between the 3
    float3 grassColour3 = float3(0.2, 0.4, 0.2); // Grass Colour 3 - lightest green colour

    float grassLevel = 0.3; // Variable to determine where the grass colour starts on the terrain
    
    // If the height of the vertex is greater than the grass level then apply the interpolated grass colour
    if (height > grassLevel)
    {
        // Interpolate between the three seperate grass colours using the height as the blending factor to add the appearance of depth
        return lerp(grassColour, lerp(grassColour2, grassColour3, lerp(grassLevel, grassLevel + 0.1, height)), lerp(grassLevel, grassLevel + 0.1, height));
    }
    else
    {
        // Interpolate between the two rock colours using the height as the blending factor
        return lerp(rockColour, rockColour2, lerp(grassLevel, grassLevel - 0.25, height));
    }
}

// Is the gemoetry in our shadow map
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    // Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(shadowSampler, uv).r;
	// Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}

float4 main(InputType input) : SV_TARGET
{
    float shadowMapBias = 0.01f;
    float4 colour = float4(0.0f, 0.0f, 0.0f, 1.0f); // 
    float4 hOneColour = float4(0.0f, 0.0f, 0.0f, 1.0f); // Helicopter One light
    float4 hTwoColour = float4(0.0f, 0.0f, 0.0f, 1.0f); // Helicopter two light
    float4 hThreeColour = float4(0.0f, 0.0f, 0.0f, 1.0f); // Helicopter three light
    
    float4 terrainColour = float4(0.0f, 0.0f, 0.0f, 1.0f); // Terrain Colour 

    // Helicopter Spotlights
    // Spotlight One
    
    // Caluclate Light Vector
    float3 hOneLightVector = normalize(input.worldPosition - hOnePosition);
    // Calculate angle between light vector and the light direction
    float hOneCosLightVector = dot(hOneLightVector, normalize(hOneDirection));
    
     // Set the angle of spotlight cone
    float hOneAngle = cos(radians(5)); 
  
    // Check if the pixel is within the angle of the spotlight
    if (hOneCosLightVector >= hOneAngle)
    {
        float hOneDistance = length(input.worldPosition - hOnePosition); // Calculate the distance between the light and the pixel
        float4 hOneAttenuation = 1 / (0.0 + (0.001 * hOneDistance) + (0.0 * pow(hOneDistance, 2))); // Calculate attenuation factor of the light
        hOneColour = hOneDiffuse * calculateLighting(-hOneDirection, input.normal, hOneAttenuation); // Calculate the colour of the light
    }
    
    // Spotlight Two
    
    // Caluclate Light Vector
    float3 hTwoLightVector = normalize(input.worldPosition - hTwoPosition);
    // Calculate angle between light vector and the light direction
    float hTwoCosLightVector = dot(hTwoLightVector, normalize(hTwoDirection));
    
     // Set the angle of spotlight cone
    float hTwoAngle = cos(radians(5));
    
    // Check if the pixel is within the angle of the spotlight
    if (hTwoCosLightVector >= hTwoAngle)
    {
        float hTwoDistance = length(input.worldPosition - hTwoPosition); // Calculate the distance between the light and the pixel
        float4 hTwoAttenuation = 1 / (0.0f + (0.001f * hTwoDistance) + (0.0f * pow(hTwoDistance, 2))); // Calculate attenuation factor of the light
        hTwoColour = hTwoDiffuse * calculateLighting(-hTwoDirection, input.normal, hTwoAttenuation); // Calculate the colour of the light
    }
   
    // Spotlight Three
    
    // Caluclate Light Vector
    float3 hThreeLightVector = normalize(input.worldPosition - hThreePosition);
    // Calculate angle between light vector and the light direction
    float hThreeCosLightVector = dot(hThreeLightVector, normalize(hThreeDirection));
    
     // Set the angle of spotlight cone
    float hThreeAngle = cos(radians(5));
 
    // Check if the pixel is within the angle of the spotlight
    if (hThreeCosLightVector >= hThreeAngle)
    {
        float hThreeDistance = length(input.worldPosition - hThreePosition); // Calculate the distance between the light and the pixel
        float4 hThreeAttenuation = 1 / (0.0f + (0.001f * hThreeDistance) + (0.0f * pow(hThreeDistance, 2))); // Calculate attenuation factor of the light
        hThreeColour = hThreeDiffuse * calculateLighting(-hThreeDirection, input.normal, hThreeAttenuation); // Calculate the colour of the light
    }
    
    // Shadow Calculations
    // Calculate the projected texture co-ordinates
    float2 pTexCoord = getProjectiveCoords(input.lightViewPos);

    // Check if the pixel is in shadow or not
    if (hasDepthData(pTexCoord))
    {
        // Has depth map data
        if (!isInShadow(depthMapTexture, pTexCoord, input.lightViewPos, shadowMapBias))
        {
            // is NOT in shadow, therefore light
            colour = calculateLighting(-direction, input.normal, diffuse); 
            terrainColour = float4(calculateTerrain(input.height), 1.0f); // Calculate the colour of the terrain based off of the height
        }
    }
    
    // Return colour
    colour = saturate(colour * hOneColour + hTwoColour + hThreeColour + terrainColour);
    return colour;
}