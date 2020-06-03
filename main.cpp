#include <igl/opengl/glfw/Viewer.h>
#include <igl/readOFF.h>
#include <igl/opengl/create_shader_program.h>
#include <igl/opengl/destroy_shader_program.h>
#include <iostream>

Eigen::MatrixXd V;
Eigen::MatrixXi F;
Eigen::VectorXd T;

int main(int argc, char *argv[])
{
  // Load a mesh in OFF format
  igl::readOFF("/home/michelle/Documents/LIBIGL/opengl_text_rendering/libigl-example-project/libigl/tutorial/data/bunny.off", V, F);

  char a = 'a';
  int ia = (int)a; 
  int da = (double)a; 
  std::cout << "a: " << a << std::endl;
  std::cout << "ia: " << ia << std::endl;
  T.resize(V.rows());
  for(int v=0; v<V.rows(); v++)
    T(v) = da;

  // Plot the mesh
  igl::opengl::glfw::Viewer viewer;

  viewer.data().set_mesh(V, F);
  viewer.data().set_points(viewer.data().V_normals.normalized()*0.1 + V, Eigen::RowVector3d(1, 0, 0), T);
  viewer.data().show_overlay = true;
  viewer.launch();

  // ----- FOR CUSTOM SHADER
  // viewer.launch_init(true,false);
  // viewer.data().meshgl.init();
  // igl::opengl::destroy_shader_program(viewer.data().meshgl.shader_mesh);
  // {
  //   igl::opengl::create_shader_program(
  //     mesh_geometry_shader_string,
  //     mesh_vertex_shader_string,
  //     mesh_fragment_shader_string,
  //     {},
  //     viewer.data().meshgl.shader_mesh);
  // }
  // viewer.launch_rendering(true);
  // viewer.launch_shut();

}
