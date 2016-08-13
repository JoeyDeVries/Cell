#ifndef CELL_RENDERER_H
#define CELL_RENDERER_H




namespace Cell
{
    /* NOTE(Joey):

    Forward declare the types, s.t. we don't need to include the
    required header files here. Seeing as most objects throughout Cell will
    link to renderer.h we want to reduce as much unnecesary additional
    header code as much as possible (saves in compilation times). And seeing
    as we only need the typename for the function specifications this works
    just fine.

    */
    class Mesh;
    class Material;

    /* NOTE(Joey):

      The main renderer; responsible for maintaining a render buffer queue
      and providing the front push commands for filling the buffer, then
      sorting the buffer, manage multiple render passes and render the
      buffer accordingly.

    */
    class Renderer
    {
    public:
        Renderer();

        void Init();

        void SetTarget(/*FrameBuffer/RenderTexture target*/);


        void PushRender(Mesh &mesh, Material &material);

        void Render();
    };
}

#endif