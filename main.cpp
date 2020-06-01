#include <igl/opengl/glfw/Viewer.h>
#include <igl/readOFF.h>
#include <igl/opengl/create_shader_program.h>
#include <igl/opengl/destroy_shader_program.h>
#include <iostream>

Eigen::MatrixXd V;
Eigen::MatrixXi F;

int main(int argc, char *argv[])
{
  // Load a mesh in OFF format
  igl::readOFF("/home/michelle/Documents/LIBIGL/opengl_text_rendering/libigl-example-project/libigl/tutorial/data/bunny.off", V, F);

  // Plot the mesh
  igl::opengl::glfw::Viewer viewer;
  viewer.data().set_mesh(V, F);
  viewer.launch_init(true,false);

  viewer.data().meshgl.init();
  igl::opengl::destroy_shader_program(viewer.data().meshgl.shader_mesh);

  {


    std::string mesh_vertex_shader_string =
      R"(
        
        #version 150
        uniform mat4 view;
        uniform mat4 proj;
        uniform mat4 normal_matrix;

        in vec3 position;
        in vec3 normal;
        in vec2 texcoord;
        in vec4 Ka;
        in vec4 Kd;
        in vec4 Ks;

        out VS_OUT {
          vec3 position_eye;
          vec3 normal_eye;
          vec4 Ksi;
          vec4 Kdi;
          vec4 Kai;
          vec2 texcoordi;
        } vs_out;

        void main()
        {
          vs_out.position_eye = vec3 (view * vec4 (position, 1.0));
          vs_out.normal_eye = vec3 (normal_matrix * vec4 (normal, 0.0));
          vs_out.normal_eye = normalize(vs_out.normal_eye);
          gl_Position = proj * vec4 (vs_out.position_eye, 1.0);

          vs_out.Ksi = Ka;
          vs_out.Kdi = Kd;
          vs_out.Kai = Ks;
          vs_out.texcoordi = texcoord;
        }
      )";

    std::string mesh_geometry_shader_string =
      R"(
        
        #version 150 core

        layout(triangles) in;
        layout(triangle_strip, max_vertices = 5) out;

        in VS_OUT {
          vec3 position_eye;
          vec3 normal_eye;
          vec4 Ksi;
          vec4 Kdi;
          vec4 Kai;
          vec2 texcoordi;
        } gs_in[];

        out VS_OUT {
          vec3 position_eye;
          vec3 normal_eye;
          vec4 Ksi;
          vec4 Kdi;
          vec4 Kai;
          vec2 texcoordi;
        } gs_out;

        void main()
        {
          gs_out.position_eye = gs_in[0].position_eye;
          gs_out.normal_eye   = gs_in[0].normal_eye;
          gs_out.texcoordi    = gs_in[0].texcoordi;
          gs_out.Kai          = gs_in[0].Kai;
          gs_out.Kdi          = gs_in[0].Kdi;
          gs_out.Ksi          = gs_in[0].Ksi;
        
          gl_Position = gl_in[1].gl_Position;
          EmitVertex();
          gl_Position = gl_in[2].gl_Position;
          EmitVertex();
          gl_Position = gl_in[0].gl_Position;
          EmitVertex();
          EndPrimitive();
        }
      
    )";

    std::string mesh_fragment_shader_string =
      R"(
        
        #version 150 

        uniform vec4 fixed_color;
        uniform mat4 view;
        uniform mat4 proj;

        in VS_OUT {
          vec3 position_eye;
          vec3 normal_eye;
          vec4 Ksi;
          vec4 Kdi;
          vec4 Kai;
          vec2 texcoordi;
        } fs_out;

        uniform vec3 light_position_eye;
        vec3 Ls = vec3 (1, 1, 1);
        vec3 Ld = vec3 (1, 1, 1);
        vec3 La = vec3 (1, 1, 1);
        uniform sampler2D tex;
        uniform float specular_exponent;
        uniform float lighting_factor;
        uniform float texture_factor;

        out vec4 outColor;

        void main()
        {
          vec3 Ia = 0.2 *La * vec3(fs_out.Kai);    // ambient intensity

          vec3 vector_to_light_eye = light_position_eye - fs_out.position_eye;
          vec3 direction_to_light_eye = normalize (vector_to_light_eye);
          float dot_prod = dot (direction_to_light_eye, normalize(fs_out.normal_eye));
          float clamped_dot_prod = max (dot_prod, 0.0);
          vec3 Id = Ld * vec3(fs_out.Kdi) * clamped_dot_prod;    // Diffuse intensity

          vec3 reflection_eye = reflect (-direction_to_light_eye, normalize(fs_out.normal_eye));
          vec3 surface_to_viewer_eye = normalize (-fs_out.position_eye);
          float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);
          dot_prod_specular = float(abs(dot_prod)==dot_prod) * max (dot_prod_specular, 0.0);
          float specular_factor = pow (dot_prod_specular, specular_exponent);
          vec3 Is = Ls * vec3(fs_out.Ksi) * specular_factor;    // specular intensity

          vec4 color = vec4(lighting_factor * (Is + Id) + Ia + (1.0-lighting_factor) * 
                       vec3(fs_out.Kdi),(fs_out.Kai.a+fs_out.Ksi.a+fs_out.Kdi.a)/3);
          outColor = mix(vec4(1,1,1,1), texture(tex, fs_out.texcoordi), texture_factor) * color;
          if (fixed_color != vec4(0.0)) outColor = fixed_color;
        }
    )";

    // std::string mesh_vertex_shader_string =
    //   R"(
    //   #version 150
    //   uniform mat4 view;
    //   uniform mat4 proj;
    //   uniform mat4 normal_matrix;

    //   in vec3 position;
    //   in vec3 normal;
    //   in vec2 texcoord;
    //   in vec4 Ka;
    //   in vec4 Kd;
    //   in vec4 Ks;

    //   out vec3 p;
    //   out vec3 n;
    //   out vec2 t;
    //   out vec4 Ka2;
    //   out vec4 Kd2;
    //   out vec4 Ks2;

    //   void main()
    //   {
    //     p = vec3 (view * vec4 (position, 1.0));
    //     n = vec3 (normal_matrix * vec4 (normal, 0.0));
    //     n = normalize(n);
    //     gl_Position = proj * vec4 (p, 1.0); //proj * view * vec4(position, 1.0);

    //     Ka2 = Ka;
    //     Kd2 = Kd;
    //     Ks2 = Ks;
    //     t = texcoord;
    //   }
    // )";

    // std::string mesh_geometry_shader_string =
    // R"(
    //   #version 150

    //   layout(points) in;
    //   layout(points, max_vertices = 4) out;

    //   in vec3 p[1];
    //   in vec3 n[1];
    //   in vec2 t[1];
    //   in vec4 Ka2[1];
    //   in vec4 Kd2[1];
    //   in vec4 Ks2[1];

    //   out vec3 position_eye;
    //   out vec3 normal_eye;
    //   out vec2 texcoordi;
    //   out vec4 Kai;
    //   out vec4 Kdi;
    //   out vec4 Ksi;

    //   void main()
    //   {
    //     position_eye = p[0];
    //     normal_eye = n[0];
    //     texcoordi = t[0];
    //     Kai = Ka2[0]; 
    //     Kdi = Kd2[0];
    //     Ksi = Ks2[0];

    //     gl_Position = gl_in[0].gl_Position;
    //     EmitVertex();

    //     EndPrimitive();

    //   }

    // )";

    // std::string mesh_fragment_shader_string =
    // R"(#version 150
    //   uniform mat4 view;
    //   uniform mat4 proj;
    //   uniform vec4 fixed_color;

    //   in vec3 position_eye;
    //   in vec3 normal_eye;
    //   in vec4 Ksi;
    //   in vec4 Kdi;
    //   in vec4 Kai;
    //   in vec2 texcoordi;

    //   uniform vec3 light_position_eye;
    //   vec3 Ls = vec3 (1, 1, 1);
    //   vec3 Ld = vec3 (1, 1, 1);
    //   vec3 La = vec3 (1, 1, 1);
    //   uniform sampler2D tex;
    //   uniform float specular_exponent;
    //   uniform float lighting_factor;
    //   uniform float texture_factor;

    //   out vec4 outColor;

    //   void main()
    //   {
    //     vec3 Ia = La * vec3(Kai);    // ambient intensity

    //     vec3 vector_to_light_eye = light_position_eye - position_eye;
    //     vec3 direction_to_light_eye = normalize (vector_to_light_eye);
    //     float dot_prod = dot (direction_to_light_eye, normalize(normal_eye));
    //     float clamped_dot_prod = max (dot_prod, 0.0);
    //     vec3 Id = Ld * vec3(Kdi) * clamped_dot_prod;    // Diffuse intensity

    //     vec3 reflection_eye = reflect (-direction_to_light_eye, normalize(normal_eye));
    //     vec3 surface_to_viewer_eye = normalize (-position_eye);
    //     float dot_prod_specular = dot (reflection_eye, surface_to_viewer_eye);
    //     dot_prod_specular = float(abs(dot_prod)==dot_prod) * max (dot_prod_specular, 0.0);
    //     float specular_factor = pow (dot_prod_specular, specular_exponent);
    //     vec3 Is = Ls * vec3(Ksi) * specular_factor;    // specular intensity
    //     vec4 color = vec4(lighting_factor * (Is + Id) + Ia + (1.0-lighting_factor) * vec3(Kdi),(Kai.a+Ksi.a+Kdi.a)/3);
    //     outColor = mix(vec4(1,1,1,1), texture(tex, texcoordi), texture_factor) * color;
    //     if (fixed_color != vec4(0.0)) outColor = fixed_color;
    //   }
    // )";

    igl::opengl::create_shader_program(
      mesh_geometry_shader_string,
      mesh_vertex_shader_string,
      mesh_fragment_shader_string,
      {},
      viewer.data().meshgl.shader_mesh);
  }

  viewer.launch_rendering(true);
  viewer.launch_shut();


}
