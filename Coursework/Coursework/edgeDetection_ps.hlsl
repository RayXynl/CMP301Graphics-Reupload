// References :
// University of Auckland. (2010). Edge Detection [PowerPoint Presentation]. Available  at: https://www.cs.auckland.ac.nz/compsci373s1c/PatricesLectures/Edge%20detection-Sobel_2up.pdf (Accessed: December 1, 2023).
// PTC. (n.d). Grayscale and Color in Images[Article]. Available at: https://support.ptc.com/help/mathcad/r9.0/en/index.html#page/PTC_Mathcad_Help/example_grayscale_and_color_in_images.html (Accessed: December 2, 2023).

Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
    float screenWidth;
    float screenHeight;
    float2 padding;
};

cbuffer EdgeDetectionBuffer : register(b1)
{
    float scale;
    float3 padding_;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float3 greyScale(float3 colour)
{
    /// Convert RGB values to grey scale using NTSC Formula
    return dot(colour, float3(0.299, 0.587, 0.114)); 
}

float4 main(InputType input) : SV_TARGET
{
    // Store the original colour to re-apply after grey scale conversion
    float3 originalColour = shaderTexture.Sample(SampleType, input.tex).rgb;

    // Calculate texel width and height
    float texelWidth = 1.0f / screenWidth;
    float texelHeight = 1.0f / screenHeight;

    // Sobel Edge Detection kernels for X and Y planes
    float3x3 edgeDetectionX =
    {
        { 1.0f, 0.0f, -1.0f },
        { 2.0f, 0.0f, -2.0f },
        { 1.0f, 0.0f, -1.0f }
    };
    float3x3 edgeDetectionY =
    {
        { -1.0f, -2.0f, -1.0f },
        { 0.0f, 0.0f, 0.0f },
        { 1.0f, 2.0f, 1.0f }
    };

    float3 gradientX = float3(0.0f, 0.0f, 0.0f);
    float3 gradientY = float3(0.0f, 0.0f, 0.0f);

    for (int i = -1; i <= 1; i++)
    {
        for (int j = -1; j <= 1; j++)
        {
            float3 sample = shaderTexture.Sample(SampleType, (input.tex + float2(texelWidth * i, texelHeight * j))).rgb; // Sample the render texture
            gradientX += greyScale(sample) * edgeDetectionX[i + 1][j + 1]; // Convert sample to grey scale and multiply by kernel values for X plane
            gradientY += greyScale(sample) * edgeDetectionY[i + 1][j + 1]; // Convert sample to grey scale and multiply by kernel values for Y plane
        }
    }
    // Calculate magnitude of gradient
    float3 gradient = sqrt((gradientX * gradientX) + (gradientY * gradientY));
    gradient *= scale; // Multiply by scale
    
    float3 finalColour = float3(0.0f, 0.0f, 0.0f);
    
    if (originalColour.r > 0.2f) // if red presence - no edge detection
    {
        finalColour = originalColour.rgb;
    }
    else if (originalColour.g > 0.2f && gradient.x < 0.4f) // if green presence and gradient below 0.4f - add edge detection
    {
        // Interpolate between the original colour and black for the edges using the gradient as a blending factor
        finalColour = lerp(originalColour, float3(0.0f, 0.0f, 0.0f), gradient); 
    }
    else if (originalColour.b > 0.2f) // if blue presence - no edge detection
    {
        finalColour = originalColour.rgb;
    }
    else // any other colour presence - no edge detection
    {
        finalColour = originalColour.rgb;
    }

    // Return final colour
    return float4(finalColour, 1.0f); 
}
