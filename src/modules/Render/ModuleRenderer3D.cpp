#include <src/helpers/Globals.h>
#include <src/Application.h>
#include "ModuleRenderer3D.h"
#include <libs/glew/include/GL/glew.h>
#include <libs\SDL\include\SDL_opengl.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "glew/lib/Win32/glew32.lib")
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#include <stdint.h>
#define SET_STATE(enabled, value) if (enabled) glEnable(value); else glDisable(value);
//====================================
typedef void(*VoidFun)();
VoidFun primitive_draw_funs[] = {
	DDTriangle, DDCube, DDCube_BadIndices, DDCube_VecIndices,
	VB_Cube, VBI_Pyramid, VBI_DiskSphere,
};

//====================================
void SetOpenGLState(const OpenGLState& state) {
	SET_STATE(state.depth_test, GL_DEPTH_TEST);
	SET_STATE(state.color_material, GL_COLOR_MATERIAL);
	SET_STATE(state.cull_faces, GL_CULL_FACE);
	SET_STATE(state.lighting, GL_LIGHTING);
	SET_STATE(state.texture2D, GL_TEXTURE_2D);
	glBlendFunc(state.src_blend, state.dst_blend);
	glPolygonMode(state.poly_mode, state.poly_fill);

	PLOG("Check for error: %s\n", gluErrorString(glGetError()));
}

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module("renderer")
{
	grid_state.lighting = false;
	states.push_back(default_state);
	states.push_back(grid_state);
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	PLOG("Creating 3D Renderer context");
	bool ret = true;
	
	// Base Flags
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);

	GLenum err = glewInit();
	PLOG("Using Glew %s", glewGetString(GLEW_VERSION));

	PLOG("Vendor: %s", glGetString(GL_VENDOR));
	PLOG("Renderer: %s", glGetString(GL_RENDERER));
	PLOG("OpenGL version supported %s", glGetString(GL_VERSION));
	PLOG("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	if(context == NULL)
	{
		PLOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if(ret == true)
	{
		//Use Vsync
		//if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			//PLOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			PLOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			PLOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.1f, 0.1f, 0.1f, 1.f);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			PLOG("Error initializing OpenGL! %s\n", gluErrorString(glGetError()));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		glShadeModel(GL_SMOOTH);
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);
	SetOpenGLState(default_state);
	glEnable(GL_DEBUG_OUTPUT);
	InitPrimitives();

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	SDL_GL_SwapWindow(App->window->window);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	// TODO: Camera Matrix part of a Renderdata that can be freely read each frame
	// Will then be useful for splitting the rendering in a different thread
	glLoadMatrixf(App->camera->GetViewMatrix());

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
double cum_dt = 0;
void ModuleRenderer3D::BindMaterial(const GPUMat& m)
{
	int baset = GL_TEXTURE0;
	glEnable(baset);	

	for (TexRelation tr : m.gpu_textures) {
		if (EnsureTexture(tr.tex_uid)) {
			glEnable(baset);
			const GPUTex& t = textures[tr.tex_uid];
			glBindTexture(GL_TEXTURE_2D, t.img_id);
		}
		++baset;
	}
}

update_status ModuleRenderer3D::PostUpdate(float dt)
{
	if (hijack_framebuffer != nullptr) {
		glBindFramebuffer(GL_FRAMEBUFFER, hijack_framebuffer->framebuffer_id);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	PLOG("Incomplete Framebuffer: %s", glewGetErrorString(glGetError()));
	cum_dt += dt;
	float4 light_position = { (float)sin(cum_dt)*5.f, 0.f, (float)cos(cum_dt)*5.f, .5f};
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 50.0 };
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, (float*)&light_position);
	float4 neg_lp = float4(-light_position.xyz(), 1.);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, (float*)&neg_lp);
	glPointSize(10.);
	glBegin(GL_POINTS);
	glColor3f(1., 1., 1.);
	glVertex3f(light_position[0], light_position[1], light_position[2]);
	glEnd();
	RenderGrid();
	glPointSize(1.);

	glRotatef(cum_dt * 100., 0., 1., 1.);

	if (draw_example_primitive) primitive_draw_funs[example_fun]();
	glBindTexture(GL_TEXTURE_2D, 0);

	glRotatef(- cum_dt * 100., 0., 1., 1.);
	glColor3f(1., 1., 1.);

	if (ecs_renderables != nullptr) {
		for (int i = 0; i < ecs_renderables->gl_state_groups.size(); ++i) {
			const RenderGroup& g = ecs_renderables->gl_state_groups[i];
			// Setup OpenGL state
			SetOpenGLState(default_state);
			for (int j = 0; j < g.materialgroups.size(); ++j) {
				const MaterialGroup& mg = g.materialgroups[j];
				// Set Material state/shader
				if (!EnsureMaterial(mg.material)) continue;
				BindMaterial(materials[mg.material]);
				for (int k = 0; k < mg.meshes.size(); ++k) {
					// Push Matrix
					if(!EnsureMesh(mg.meshes[k])) continue;
					GPUMesh& m = meshes[mg.meshes[k]];
					glEnableClientState(GL_VERTEX_ARRAY);
					glBindBuffer(GL_ARRAY_BUFFER, m.vtx_id);
					glVertexPointer(3, GL_FLOAT, 0, NULL);
					if (m.norm_id != 0) {
						glEnableClientState(GL_NORMAL_ARRAY);
						glBindBuffer(GL_ARRAY_BUFFER, m.norm_id);
						glNormalPointer(GL_FLOAT, 0, NULL);
					}
					if (m.uvs_id != 0) {
						glEnableClientState(GL_TEXTURE_COORD_ARRAY);
						glBindBuffer(GL_ARRAY_BUFFER, m.uvs_id);
						glTexCoordPointer(2, GL_FLOAT, 0, NULL);
					}
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.idx_id);

					const float4x4& matx = ecs_renderables->transforms[mg.world_matrices[k]];
					// TODO: Repair transform tree...
					glPushMatrix();
					glMultMatrixf(matx.Transposed().ptr());

					glDrawElements(GL_TRIANGLES, m.num_idx, GL_UNSIGNED_INT, nullptr);

					glPopMatrix();
				}
			}
		}
	}

	//glEnable(GL_TEXTURE_2D);
	//for (GPUMesh& m : meshes) {
	//	
	//	
	//
	//	
	//
	//	
	//	glDisableClientState(GL_VERTEX_ARRAY);
	//	glBindTexture(GL_TEXTURE_2D, 0);
	//}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	PLOG("Destroying 3D Renderer");
	CleanUpPrimitives();
	for (auto& mesh : meshes)
		UnloadMesh(mesh.second);

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::ReceiveEvents(std::vector<std::shared_ptr<Event>>& evt_vec)
{
	for (std::shared_ptr<Event> ev : evt_vec) {
		switch (ev->type) {
			case EventType::WINDOW_RESIZE:
				OnResize(ev->point2d.x, ev->point2d.y);
				continue;
			case EventType::CHANGED_DEFAULT_OPENGL_STATE:
				default_state = ev->ogl_state;
				SetOpenGLState(default_state);
				continue;
			case EventType::TOGGLE_RENDERER_PRIMITIVES:
				draw_example_primitive = ev->boolean;
				continue;
			case EventType::CHANGE_RENDERER_PRIMITIVE:
				example_fun = ev->uint32;
				continue;
			case EventType::LOAD_MESH_TO_GPU:{
				const NIMesh* mesh = App->fs->RetrievePValue<NIMesh>(ev->uint64);
				meshes.insert(std::pair<uint64_t, GPUMesh>(ev->uint64, LoadMesh(mesh)));
				continue; }
			case EventType::LOAD_TEX_TO_GPU: {
				const Texture* tex = App->fs->RetrievePValue<Texture>(ev->uint64);
				textures.insert(std::pair<uint64_t, GPUTex>(ev->uint64, LoadTexture(tex)));
				continue; }
			case EventType::LOAD_MAT_TO_GPU: {
				const Material* mat = App->fs->RetrievePValue<Material>(ev->uint64);
				materials.insert(std::pair<uint64_t, GPUMat>(ev->uint64,LoadMaterial(mat)));
				continue; }
			case EventType::FRAMEBUFFER_HIJACK: {
				hijack_framebuffer = (GPUFBO*)ev->generic_pointer;
				continue;
			}
			case EventType::ECS_RENDERABLES: {
				ecs_renderables = (FullGroup*)ev->generic_pointer;
				continue;
			}
		}
	}

	// Not ideal, but should work to update materials of meshes
	SetMeshMats();
}



void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Regen Hijacked Framebuffer just in case...
	if (hijack_framebuffer != nullptr) {
		glDeleteTextures(1, &hijack_framebuffer->attachment.img_id);
		*hijack_framebuffer = GenerateScreenFBO();
	}
}
