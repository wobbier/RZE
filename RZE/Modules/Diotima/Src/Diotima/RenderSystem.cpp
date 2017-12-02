//#include <StdAfx.h>
#include <Diotima/RenderSystem.h>

#include <Diotima/Driver/OpenGL.h>
#include <Diotima/Graphics/Mesh.h>
#include <Diotima/Graphics/Texture2D.h>
#include <Diotima/Shaders/ShaderGroup.h>

#include <Utils/DebugUtils/Debug.h>
#include <Utils/Math/Vector4D.h>

#include <imGUI/imgui.h>

//
// <Imgui Stuff>
//
// #TODO(Josh) Move this to a more sane place

static GLuint gFontTexture = 0;

static int gShaderHandle = 0;
static int gVertHandle = 0;
static int gFragHandle = 0;

static int gAttribLocationTex = 0;
static int gAttribLocationProjMtx = 0;
static int gAttribLocationPosition = 0;
static int gAttribLocationUV = 0;
static int gAttribLocationColor = 0;

static unsigned int gVBOHandle = 0;
static unsigned int gVAOHandle = 0;
static unsigned int gElementsHandle = 0;

void ImGUIRender(ImDrawData* drawData)
{
	ImGuiIO& io = ImGui::GetIO();
	int fbWidth = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fbHeight = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fbWidth == 0 || fbHeight == 0)
		return;
	drawData->ScaleClipRects(io.DisplayFramebufferScale);

	// Backup GL state
	GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
	glActiveTexture(GL_TEXTURE0);
	GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_sampler; glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
	GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
	GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
	GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
	GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
	GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
	GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
	GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	const float ortho_projection[4][4] =
	{
		{ 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
		{ 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
		{ 0.0f,                  0.0f,                  -1.0f, 0.0f },
		{ -1.0f,                  1.0f,                   0.0f, 1.0f },
	};

	glUseProgram(gShaderHandle);
	glUniform1i(gAttribLocationTex, 0);
	glUniformMatrix4fv(gAttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
	glBindVertexArray(gVAOHandle);
	glBindSampler(0, 0); // Rely on combined texture/sampler state.

	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = drawData->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = 0;

		glBindBuffer(GL_ARRAY_BUFFER, gVBOHandle);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gElementsHandle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
				glScissor((int)pcmd->ClipRect.x, (int)(fbHeight - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}

	// Restore modified GL state
	glUseProgram(last_program);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindSampler(0, last_sampler);
	glActiveTexture(last_active_texture);
	glBindVertexArray(last_vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, last_polygon_mode[0]);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

bool ImGUICreateFontsTexture()
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

															  // Upload texture to graphics system
	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &gFontTexture);
	glBindTexture(GL_TEXTURE_2D, gFontTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (void *)(intptr_t)gFontTexture;

	// Restore state
	glBindTexture(GL_TEXTURE_2D, last_texture);

	return true;
}

bool ImGUICreateDeviceObjects()
{
	// Backup GL state
	GLint last_texture, last_array_buffer, last_vertex_array;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

	const GLchar *vertex_shader =
		"#version 330\n"
		"uniform mat4 ProjMtx;\n"
		"in vec2 Position;\n"
		"in vec2 UV;\n"
		"in vec4 Color;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"	Frag_UV = UV;\n"
		"	Frag_Color = Color;\n"
		"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
		"}\n";

	const GLchar* fragment_shader =
		"#version 330\n"
		"uniform sampler2D Texture;\n"
		"in vec2 Frag_UV;\n"
		"in vec4 Frag_Color;\n"
		"out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
		"}\n";

	gShaderHandle = glCreateProgram();
	gVertHandle = glCreateShader(GL_VERTEX_SHADER);
	gFragHandle = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(gVertHandle, 1, &vertex_shader, 0);
	glShaderSource(gFragHandle, 1, &fragment_shader, 0);
	glCompileShader(gVertHandle);
	glCompileShader(gFragHandle);
	glAttachShader(gShaderHandle, gVertHandle);
	glAttachShader(gShaderHandle, gFragHandle);
	glLinkProgram(gShaderHandle);

	gAttribLocationTex = glGetUniformLocation(gShaderHandle, "Texture");
	gAttribLocationProjMtx = glGetUniformLocation(gShaderHandle, "ProjMtx");
	gAttribLocationPosition = glGetAttribLocation(gShaderHandle, "Position");
	gAttribLocationUV = glGetAttribLocation(gShaderHandle, "UV");
	gAttribLocationColor = glGetAttribLocation(gShaderHandle, "Color");

	glGenBuffers(1, &gVBOHandle);
	glGenBuffers(1, &gElementsHandle);

	glGenVertexArrays(1, &gVAOHandle);
	glBindVertexArray(gVAOHandle);
	glBindBuffer(GL_ARRAY_BUFFER, gVBOHandle);
	glEnableVertexAttribArray(gAttribLocationPosition);
	glEnableVertexAttribArray(gAttribLocationUV);
	glEnableVertexAttribArray(gAttribLocationColor);

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
	glVertexAttribPointer(gAttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, pos));
	glVertexAttribPointer(gAttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, uv));
	glVertexAttribPointer(gAttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF

	ImGUICreateFontsTexture();

	// Restore modified GL state
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindVertexArray(last_vertex_array);

	return true;
}
//
// </Imgui Stuff>

namespace Diotima
{
	RenderSystem::RenderSystem()
	{
		
	}

	void RenderSystem::AddRenderItem(const RenderItemProtocol& itemProtocol)
	{
		mRenderList.push_back(itemProtocol);
	}

	void RenderSystem::AddLightItem(const LightItemProtocol& itemProtocol)
	{
		mLightingList.push_back(itemProtocol);
	}

	void RenderSystem::Initialize()
	{
		// #TODO(Josh) The window should probably be in utils/renderer or something but not the engine 
// 		WindowSettings& windowSettings = RZE_Engine::Get()->GetWindowSettings();
// 
// 		{
// 			OpenGLRHI::OpenGLCreationParams creationParams;
// 			creationParams.WindowWidth = static_cast<int>(windowSettings.GetDimensions().X());
// 			creationParams.WindowHeight = static_cast<int>(windowSettings.GetDimensions().Y());
// 
// 			OpenGLRHI::Get().Init(creationParams);
// 		}
// 
// 		OpenGLRHI::Get().ClearColor(0.0f, 0.0f, 1.0f, 1.0f);

// 		SceneCameraProps cameraProps;	cameraProps.Direction = Vector3D(0.0f, -2.5f, -10.0f);
// 		cameraProps.FrontDir = Vector3D(0.0f, 0.0f, -1.0f);
// 		cameraProps.UpDir = Vector3D(0.0f, 1.0f, 0.0f);
// 		cameraProps.FOV = 45.0f;
// 		cameraProps.AspectRatio = windowSettings.GetDimensions().X() / windowSettings.GetDimensions().Y();
// 		cameraProps.NearCull = 0.1f;
// 		cameraProps.FarCull = 1000.0f;
// 
// 		mSceneCamera = new SceneCamera(cameraProps);
// 		mSceneCamera->GenerateProjectionMat();
// 		mSceneCamera->GenerateViewMat();
// 
		OpenGLRHI::Get().EnableCapability(EGLCapability::DepthTest);

		ImGuiIO& io = ImGui::GetIO();
		ImGUICreateDeviceObjects();
		io.RenderDrawListsFn = ImGUIRender;
	}

	void RenderSystem::Update()
	{
		const OpenGLRHI& openGL = OpenGLRHI::Get();
		openGL.Clear(EGLBufferBit::Color | EGLBufferBit::Depth);

		for (auto& item : mRenderList)
		{
			RenderSingleItem(item);
		}

		// 	mLightingList.clear();
		// 	mFontList.clear();
	}

	void RenderSystem::ShutDown()
	{

	}

	void RenderSystem::ClearLists()
	{
		mRenderList.clear();
		mLightingList.clear();
		mFontList.clear();
	}

	void RenderSystem::ResizeCanvas(const Vector2D& newSize)
	{
		OpenGLRHI::Get().Viewport(0, 0, static_cast<GLsizei>(newSize.X()), static_cast<GLsizei>(newSize.Y()));
	}

	SceneCamera& RenderSystem::GetSceneCamera()
	{
		AssertNotNull(mSceneCamera);
		return *mSceneCamera;
	}

	void RenderSystem::RenderSingleItem(RenderItemProtocol& renderItem)
	{
		const OpenGLRHI& openGL = OpenGLRHI::Get();
		// @implementation should we have this type of assumption?
		if (renderItem.mShaderGroup)
		{
			if (renderItem.mTextureData)
			{
				// #TODO replace with RHI call
				openGL.BindTexture(EGLCapability::Texture2D, renderItem.mTextureData->GetTextureID());
			}

			renderItem.mShaderGroup->Use();

			renderItem.mShaderGroup->SetUniformMatrix4x4("UModelMat", renderItem.mModelMat);
			renderItem.mShaderGroup->SetUniformMatrix4x4("UProjectionMat", renderItem.mProjectionMat);
			renderItem.mShaderGroup->SetUniformMatrix4x4("UViewMat", renderItem.mViewMat);

			renderItem.mShaderGroup->SetUniformVector4D("UFragColor", Vector4D(0.25f, 0.25f, 0.25f, 1.0f));

			for (auto& light : mLightingList)
			{
				renderItem.mShaderGroup->SetUniformVector3D("ULightPosition", light.mLightPos);
				renderItem.mShaderGroup->SetUniformVector3D("UViewPosition", mSceneCamera->GetPositionVec());
				renderItem.mShaderGroup->SetUniformVector3D("ULightColor", light.mLightColor);
				renderItem.mShaderGroup->SetUniformFloat("ULightStrength", light.mLightStrength);
			}
		}

		const std::vector<GFXMesh*>& meshList = renderItem.mMeshData->GetMeshList();
		for (auto& mesh : meshList)
		{
			mesh->GetVAO().Bind();

			OpenGLRHI::Get().DrawElements(EGLDrawMode::Triangles, mesh->GetIndices().size(), EGLDataType::UnsignedInt, nullptr);

			mesh->GetVAO().Unbind();
		}
	}

	RenderSystem::RenderItemProtocol::RenderItemProtocol()
	{
		mShaderGroup = nullptr;
		mMeshData = nullptr;
		mTextureData = nullptr;
	}
}
