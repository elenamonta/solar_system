#pragma once
#include "ImGui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imGui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "material.h"
#include "mesh.h"

using namespace ImGui;
extern bool trackballMode;
extern bool Clockwise;
extern vector<Mesh> scene; 

class Gui {
	public:

		int selectedMaterialType = 0;
		int selectedShader = 0; 

		Gui(GLFWwindow* window){
			Window = window;
		}

		void Initilize_IMGUI() {
			IMGUI_CHECKVERSION();
			CreateContext();
			ImGuiIO& io = GetIO();
			io.FontGlobalScale = 1.0f;
			StyleColorsDark();

			ImGui_ImplGlfw_InitForOpenGL(Window, true);
			ImGui_ImplOpenGL3_Init("#version 330 core");
		}

		void set_selected_id(int selected) {
			this->selected_id = selected;
		}

		void my_interface() {
			ImGui_ImplGlfw_NewFrame();
			ImGui_ImplOpenGL3_NewFrame(); // Inizia un nuovo frame per ImGui
			ImGui::NewFrame();


			ImGui::Begin("Impostazioni", NULL,
				ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoTitleBar |
				ImGuiWindowFlags_NoMove
			);

			if (selected_id > -1) {
				string toPrint = "Oggetto selezionato ";
				toPrint.append(scene[selected_id].name);
				Text(toPrint.c_str());
			}
			
			const char* materialNames[] = {
				"RedPlastic", "Brass", "Emerald", "SnowWhite", "Yellow", "Pink", "Brown"
			};

			bool material = Combo("Tipo di Materiale", &selectedMaterialType, materialNames, IM_ARRAYSIZE(materialNames));

			const char* shaderOption[] = {
				"Phong", "Blinn Phong"
			};

			bool shader = Combo("Scelta shader", &selectedShader, shaderOption, IM_ARRAYSIZE(shaderOption));
			
			
			SetWindowSize(ImVec2(300, 100));
			
			Checkbox("Navigazione trackball", &trackballMode);
			if(trackballMode){
				Checkbox("Navigazione in senso orario", &Clockwise);
			}

			End();

			Render();

		}


		void close_GUI() {
			ImGui_ImplOpenGL3_Shutdown(); // Chiude l'integrazione con OpenGL
			ImGui_ImplGlfw_Shutdown(); // Chiude l'integrazione con GLFW
			DestroyContext(); // Distrugge il contesto ImGui
		}

private:
	GLFWwindow* Window;
	int selected_id = -1;
};