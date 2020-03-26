#include <igl/opengl/glfw/Viewer.h>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <imgui/imgui.h>
#include "menu_extra.h"
#include "slicing_plugin.h"
#include <igl/readSTL.h>

int main(int argc, char *argv[])
{

  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  Eigen::MatrixXd N;

  // Plot the mesh
  igl::opengl::glfw::Viewer viewer;
  std::string path = "/home/michelle/Documents/LIBIGL/LABELS_BUG/mishfork/libigl-example-project/3rdparty/libigl/tutorial/data/";
  path += "camel_b.obj";
  // path += "bunny.obj";
  viewer.load_mesh_from_file(path);
  viewer.data().set_mesh(V, F);
  // std::cout<<"Vertex count: " << V.rows() << std::endl;
  // viewer.data().set_face_based(true);
  // igl::readSTL("/home/michelle/Documents/dgp_imguizmo_orthographic/libigl-example-project/3rdparty/libigl/tutorial/data/Sapphos_Head.stl", V, F, N);


  // Custom menu
  SlicingPlugin menu;
  viewer.plugins.push_back(&menu);
  // menu.draw_viewer_menu_func = [&]() {
  //   menu.draw_viewer_menu();

  //   static Eigen::Matrix4f matrix = Eigen::Matrix4f::Identity();
  //   Eigen::Matrix4f view = viewer.core.view * viewer.core.model;
  //   Eigen::Matrix4f proj = viewer.core.proj;
  //   // ImGuizmo::DrawCube(view.data(), proj.data(), matrix.data());
  //   ImGuizmo::EditTransform(view.data(), proj.data(), matrix.data());
  // };

  viewer.resize(1024, 1024);
  viewer.launch();
}