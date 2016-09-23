#ifndef CELL_SCENE_BACKGROUND_H
#define CELL_SCENE_BACKGROUND_H

namespace Cell
{
    class TextureCube;
    class Material;
    class Shader;
    class Cube;

    /* NOTE(Joey):

      

    */
    class Background 
    {
    private:
     /*   TextureCube m_CubeMap;
        Material m_Material;
        Shader m_Shader;
        Cube m_Cube;        */

    public:
        Background();
        ~Background();

        void SetCubemap(TextureCube cubemap);

        void Render();
    };
}

#endif