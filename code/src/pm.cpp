#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "light.hpp"

#include <string>

using namespace std;

int main(int argc, char *argv[])
{
    for (int argNum = 1; argNum < argc; ++argNum)
    {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc != 3)
    {
        cout << "Usage: ./bin/PA1 <input scene file> <output bmp file>" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2]; // only bmp is allowed.

    // Main RayCasting Logic
    // First, parse the scene using SceneParser.
    SceneParser parser(inputFile.c_str());
    // Then loop over each pixel in the image, shooting a ray
    // through that pixel and finding its intersection with
    // the scene.  Write the color at the intersection to that
    // pixel in your output image.
    int w = parser.getCamera()->getWidth(),
        h = parser.getCamera()->getHeight();
    Image img(w, h);

    //TODO: for (x,y) within (0,0) to (w,h), get all Pixel done
    // and img.SetPixel(x, y, finalColor);


    /* Original Ray-Casting Code
    for (int x = 0; x < w; ++x)
    {
        for (int y = 0; y < h; ++y)
        {
            Ray camRay = parser.getCamera()->generateRay(Vector2f(x, y));
            Group *baseGroup = parser.getGroup();
            Hit hit;

            bool isIntersect = baseGroup->intersect(camRay, hit, 0);
            if (isIntersect)
            {
                Vector3f finalColor = Vector3f::ZERO;
                for (int li = 0; li < parser.getNumLights(); ++li)
                {
                    Light *light = parser.getLight(li);
                    Vector3f L, LightColor;

                    light->getIllumination(camRay.pointAtParameter(hit.getT()), L, LightColor);

                    finalColor += hit.getMaterial()->Shade(camRay, hit, L, LightColor);
                }
                img.SetPixel(x, y, finalColor);
            }
            else
            {
                img.SetPixel(x, y, parser.getBackgroundColor());
            }
        }
    }
    */

    img.SaveImage(outputFile.c_str());
    cout << "Hello! Computer Graphics!" << endl;
    return 0;
}
