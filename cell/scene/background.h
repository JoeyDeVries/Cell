#ifndef CELL_SCENE_BACKGROUND_H
#define CELL_SCENE_BACKGROUND_H

#include "scene_node.h"


namespace Cell 
{
    class TextureCube;
    class Material;
    class Shader;
    class Renderer;
    class Cube;

    /* NOTE(Joey):

      

    */
    class Background : public SceneNode
    {
    private:
        TextureCube *m_CubeMap;
        Shader      *m_Shader;

    public:
        Background();
        ~Background();

        void SetCubemap(TextureCube *cubemap);
    };
}

#endif