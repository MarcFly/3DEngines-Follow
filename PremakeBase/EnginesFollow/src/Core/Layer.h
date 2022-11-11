#pragma once

#include "../Globals.h"

namespace Engine {
	// Layers are like modules, managed by a higher power like application
	// The idea is to make it more gradual
	struct EF_API Layer {
		uint64_t pos = UINT64_MAX;
		std::string debug_name;

		Layer(const char* name = "Layer") : debug_name(name) {};
		virtual ~Layer() {};

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void PreUpdate() {}
		virtual void Update() {}
		virtual void PostUpdate() {}

		virtual void OnEvent() {}

		virtual void CleanUp() {}

		inline const char* GetName() const { return debug_name.c_str(); }
	};

	struct EF_API LayerStack {

		void PushLayer(Layer* layer) { 
			layers.emplace_back(layer); 
			layer->pos = layers.size() - 1; 
			layer->OnAttach(); 
		}
		void PopLayer(Layer* layer) {
			int i = 0;
			for (std::vector<Layer*>::iterator layer_it = layers.begin(); layer_it != layers.end(); ++layer_it) {
				if ((*layer_it)->pos == i) {
					layers.erase(layer_it);
					break;
				}
			} 
			for (i; i < layers.size(); ++i)
				layers[i]->pos = i;
			layer->OnDetach();
		}
		void PushOverlay(Layer* layer) { overlays.emplace_back(layer); layer->pos = overlays.size() - 1; layer->OnAttach(); }
		void PopOverlay(Layer* layer) {
			int i = 0;
			for (std::vector<Layer*>::iterator layer_it = overlays.begin(); layer_it != overlays.end(); ++layer_it) {
				if ((*layer_it)->pos == i) {
					overlays.erase(layer_it);
					break;
				}
			}
			for (i; i < overlays.size(); ++i)
				overlays[i]->pos = i;
			layer->OnDetach();
		}

		std::vector<Layer*> layers;
		std::vector<Layer*> overlays;
	};
};