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

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module("renderer", start_enabled)
{
	grid_state.lighting = false;
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
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			PLOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

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
		glClearColor(0.f, 0.f, 0.f, 1.f);

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
update_status ModuleRenderer3D::PostUpdate(float dt)
{

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
	glRotatef(- cum_dt * 100., 0., 1., 1.);
	glColor3f(1., 1., 1.);
	glEnable(GL_TEXTURE_2D);
	for (GPUMesh& m : meshes) {
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glBindBuffer(GL_ARRAY_BUFFER, m.vtx_id);
		glVertexPointer(3, GL_FLOAT, 0, NULL);
		if (m.norm_id != 0) {
			glEnableClientState(GL_NORMAL_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, m.norm_id);
			glNormalPointer(GL_FLOAT, 0, NULL);
		}
		if (m.uvs_id != 0) {
			if(textures.size() > 0)glBindTexture(GL_TEXTURE_2D, textures[0].img_id);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, m.uvs_id);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		}
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.idx_id);
		glDrawElements(GL_TRIANGLES, m.num_idx, GL_UNSIGNED_INT, nullptr);
		glDisableClientState(GL_VERTEX_ARRAY);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	PLOG("Destroying 3D Renderer");
	CleanUpPrimitives();
	for (GPUMesh& mesh : meshes)
		UnloadMesh(mesh);

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
			case EventType::LOAD_MESH_TO_GPU:
			{
				const NIMesh* mesh = App->fs->RetrievePValue<NIMesh>(ev->uint64);
				LoadMesh(mesh);
				continue;
			}
			case EventType::LOAD_TEX_TO_GPU:
				const Texture* tex = App->fs->RetrievePValue<Texture>(ev->uint64);
				LoadTexture(tex);
				continue;
		}
	}
}

#define GRID_SIZE 10

void ModuleRenderer3D::RenderGrid() const
{
	SetOpenGLState(grid_state);
	for (int i = 0; i < GRID_SIZE * 2 + 1; i++)
	{
		glBegin(GL_LINES);
		glColor3f(1.5f, 0.5f, 0.5f);

		//Z
		glVertex3i(GRID_SIZE - i, 0, GRID_SIZE);
		glVertex3i(GRID_SIZE - i, 0, -GRID_SIZE);

		//X
		glVertex3i(-GRID_SIZE, 0, -GRID_SIZE + i);
		glVertex3i(GRID_SIZE, 0, -GRID_SIZE + i);
		glEnd();
	}

	SetOpenGLState(default_state);
}

void ModuleRenderer3D::LoadMesh(const NIMesh* mesh)
{
	GPUMesh push;
	push.num_vtx = mesh->vertices.size();
	glGenBuffers(1, &push.vtx_id);
	glBindBuffer(GL_ARRAY_BUFFER, push.vtx_id);
	glBufferData(GL_ARRAY_BUFFER, push.num_vtx * sizeof(float3), mesh->vertices.data(), GL_STATIC_DRAW);
	if (mesh->normals.size() > 0) {
		glGenBuffers(1, &push.norm_id);
		glBindBuffer(GL_ARRAY_BUFFER, push.norm_id);
		glBufferData(GL_ARRAY_BUFFER, mesh->normals.size() * sizeof(float3), mesh->normals.data(), GL_STATIC_DRAW);
	}
	if (mesh->uvs.size() > 0) {
		glGenBuffers(1, &push.uvs_id);
		glBindBuffer(GL_ARRAY_BUFFER, push.uvs_id);
		glBufferData(GL_ARRAY_BUFFER, mesh->uvs.size() * sizeof(float2), mesh->uvs.data(), GL_STATIC_DRAW);
	}
	if (mesh->indices.size() > 0) {
		push.num_idx = mesh->indices.size();
		glGenBuffers(1, &push.idx_id);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, push.idx_id);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, push.num_idx * sizeof(uint32_t), mesh->indices.data(), GL_STATIC_DRAW);
	}

	meshes.push_back(push);
}

void ModuleRenderer3D::UnloadMesh(GPUMesh& mesh) {
	std::vector<uint32_t> ids;
	if (mesh.vtx_id > 0) ids.push_back(mesh.vtx_id);
	if (mesh.norm_id > 0) ids.push_back(mesh.norm_id);
	if (mesh.uvs_id > 0) ids.push_back(mesh.uvs_id);
	if (mesh.idx_id > 0) ids.push_back(mesh.idx_id);

	glDeleteBuffers(ids.size(), ids.data());

	mesh.vtx_id = mesh.norm_id = mesh.uvs_id = mesh.idx_id = 0;
}

void ModuleRenderer3D::LoadTexture(const Texture* tex)
{
	GPUTex push;
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &push.img_id);
	glBindTexture(GL_TEXTURE_2D, push.img_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, tex->format, tex->w, tex->h, 0, tex->format, tex->unit_type, tex->bytes);

	push.w = tex->w;
	push.h = tex->h;

	textures.push_back(push);
}

void ModuleRenderer3D::UnloadTex(GPUTex& tex)
{
	glDeleteTextures(1, &tex.img_id);
	tex.img_id = 0;
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
}
