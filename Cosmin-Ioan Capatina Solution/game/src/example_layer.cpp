#include "example_layer.h"
#include "platform/opengl/gl_shader.h"

#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>
#include "engine/events/key_event.h"
#include "engine/utils/track.h"
#include "pickup.h"


example_layer::example_layer()
	:m_2d_camera(-1.6f, 1.6f, -0.9f, 0.9f),
	m_3d_camera((float)engine::application::window().width(), (float)engine::application::window().height())


{

	m_fog = false;
	glm::vec3 m_fog_colour = glm::vec3(0.5f);
	int m_fog_factor_type = 2;

	// Hide the mouse and lock it inside the window
	//engine::input::anchor_mouse(true);
	engine::application::window().hide_mouse_cursor();

	// Initialise audio and play background music
	m_audio_manager = engine::audio_manager::instance();
	m_audio_manager->init();
	m_audio_manager->load_sound("assets/audio/bounce1.wav", engine::sound_type::event, "bounce"); // Royalty free sound from freesound.org
	m_audio_manager->load_sound("assets/audio/backgroundmusic.wav", engine::sound_type::track, "music");  // Royalty free music from freesound.org
	m_audio_manager->play("music");


	// Initialise the shaders, materials and lights
	auto mesh__material_shader = engine::renderer::shaders_library()->get("mesh_material");
	auto mesh_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");
	auto text_shader = engine::renderer::shaders_library()->get("text_2D");
	auto animated_mesh_shader = engine::renderer::shaders_library()->get("animated_mesh");

	m_directionalLight.Color = glm::vec3(1.0f, 1.0f, 1.0f);
	m_directionalLight.AmbientIntensity = 0.25f;
	m_directionalLight.DiffuseIntensity = 0.6f;
	m_directionalLight.Direction = glm::normalize(glm::vec3(1.0f, -1.0f, 0.0f));

	// set color texture unit
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gDirectionalLight.Base.Color", m_directionalLight.Color);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gDirectionalLight.Base.AmbientIntensity", m_directionalLight.AmbientIntensity);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gDirectionalLight.Direction", glm::normalize(m_directionalLight.Direction));
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gDirectionalLight.Base.DiffuseIntensity", m_directionalLight.DiffuseIntensity);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gMatSpecularIntensity", 0.5f);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gSpecularPower", 5.f);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("transparency", 1.0f);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("fog_colour", m_fog_colour);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("fog_factor_type", m_fog_factor_type);

	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("fog_colour", m_fog_colour);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("fog_factor_type", m_fog_factor_type);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gDirectionalLight.Base.Color", m_directionalLight.Color);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gDirectionalLight.Base.AmbientIntensity", m_directionalLight.AmbientIntensity);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gDirectionalLight.Direction", glm::normalize(m_directionalLight.Direction));
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gDirectionalLight.Base.DiffuseIntensity", m_directionalLight.DiffuseIntensity);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gMatSpecularIntensity", 0.5f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh__material_shader)->set_uniform("gSpecularPower", 5.f);

	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("fog_colour", m_fog_colour);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("fog_factor_type", m_fog_factor_type);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gColorMap", 0);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gDirectionalLight.Base.Color", m_directionalLight.Color);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gDirectionalLight.Base.AmbientIntensity", m_directionalLight.AmbientIntensity);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gDirectionalLight.Direction", glm::normalize(m_directionalLight.Direction));
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gDirectionalLight.Base.DiffuseIntensity", m_directionalLight.DiffuseIntensity);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gMatSpecularIntensity", 0.5f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("gSpecularPower", 5.f);
	std::dynamic_pointer_cast<engine::gl_shader>(mesh_lighting_shader)->set_uniform("transparency", 1.0f);

	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->bind();
	std::dynamic_pointer_cast<engine::gl_shader>(text_shader)->set_uniform("projection",
		glm::ortho(0.f, (float)engine::application::window().width(), 0.f, (float)engine::application::window().height()));


	m_material = engine::material::create(32.0f,
		glm::vec3(1.0f, 0.1f, 0.07f),
		glm::vec3(1.0f, 0.1f, 0.07f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		1.0f);

	m_tetrahedron_material = engine::material::create(30.0f,
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(1.0f, 0.5f, 0.0f),
		glm::vec3(0.5f, 0.5f, 0.5f),
		0.3f);


	

	std::vector<engine::ref<engine::texture_2d>> tetrahedron_textures = { engine::texture_2d::create("assets/textures/stone.bmp", false) };
	//std::vector<engine::ref<engine::texture_2d>> rectangle_textures = { engine::texture_2d::create("assets/textures/medkit.jpg", false) };
	std::vector<engine::ref<engine::texture_2d>> sphere_textures = { engine::texture_2d::create("assets/textures/basketball1.jpg", false) };

	// Skybox texture from http://www.vwall.it/wp-content/plugins/canvasio3dpro/inc/resource/cubeMaps/
	m_skybox = engine::skybox::create(50.f,
		{ engine::texture_2d::create("assets/textures/skybox/SkyboxLeft.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxFront.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxRight.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxBack.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxTop.bmp", true),
		  engine::texture_2d::create("assets/textures/skybox/SkyboxBottom.bmp", true)
		});

	// Load mannequin animations
	engine::ref<engine::skinned_mesh> m_skinned_mesh = engine::skinned_mesh::create("assets/models/animated/mannequin/free3Dmodel.dae");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/walking.dae");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/idle.dae");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/jump.dae");
	m_skinned_mesh->LoadAnimationFile("assets/models/animated/mannequin/standard_run.dae");
	m_skinned_mesh->switch_root_movement(false);

	// Load Mannequin
	engine::game_object_properties mannequin_props;
	mannequin_props.animated_mesh = m_skinned_mesh;
	mannequin_props.scale = glm::vec3(1.f / glm::max(m_skinned_mesh->size().x, glm::max(m_skinned_mesh->size().y, m_skinned_mesh->size().z)));
	mannequin_props.position = glm::vec3(3.0f, 3.f, -5.0f);
	mannequin_props.type = 0;
	mannequin_props.bounding_shape = glm::vec3(m_skinned_mesh->size().x / 2.f, m_skinned_mesh->size().y, m_skinned_mesh->size().x / 2.f);
	mannequin_props.friction = 0.0f;
	m_mannequin = engine::game_object::create(mannequin_props);

	m_player.initialise(m_mannequin);
	m_player_box.set_box(mannequin_props.bounding_shape.x* mannequin_props.scale.x, mannequin_props.bounding_shape.y* mannequin_props.scale.x, mannequin_props.bounding_shape.z* mannequin_props.scale.x, mannequin_props.position);

	// Load the terrain texture and create a terrain mesh. Create a terrain object. Set its properties
	std::vector<engine::ref<engine::texture_2d>> terrain_textures = { engine::texture_2d::create("assets/textures/terrain.bmp", false) };
	engine::ref<engine::terrain> terrain_shape = engine::terrain::create(100.f, 0.5f, 100.f);
	engine::game_object_properties terrain_props;
	terrain_props.meshes = { terrain_shape->mesh() };
	terrain_props.textures = terrain_textures;
	terrain_props.is_static = true;
	terrain_props.type = 0;
	terrain_props.bounding_shape = glm::vec3(100.f, 0.5f, 100.f);
	terrain_props.restitution = 0.92f;
	m_terrain = engine::game_object::create(terrain_props);

	// Load the cow model. Create a cow object. Set its properties
	engine::ref <engine::model> cow_model = engine::model::create("assets/models/static/cow4.3ds");
	engine::game_object_properties cow_props;
	cow_props.meshes = cow_model->meshes();
	cow_props.textures = cow_model->textures();
	float cow_scale = 1.f / glm::max(cow_model->size().x, glm::max(cow_model->size().y, cow_model->size().z));
	cow_props.position = { 0.f,0.5f, -20.f };
	cow_props.scale = glm::vec3(cow_scale);
	cow_props.bounding_shape = cow_model->size() / 2.f * cow_scale;
	m_cow = engine::game_object::create(cow_props);

	// Load the wall model. Create a wall object. Set its properties
	engine::ref <engine::model> fance_model = engine::model::create("assets/models/static/wall.obj");
	engine::game_object_properties fence_props;
	fence_props.meshes = fance_model->meshes();
	fence_props.textures = fance_model->textures();
	float fance_scale = 3.f / glm::max(fance_model->size().x, glm::max(fance_model->size().y, fance_model->size().z));
	fence_props.position = { 3.f, 0.5f, 20.f };
	fence_props.scale = glm::vec3(fance_scale);
	fence_props.bounding_shape = fance_model->size();
	m_fence = engine::game_object::create(fence_props);
	m_fence->set_offset(fance_model->offset());
	m_fence_box.set_box(fence_props.bounding_shape.x* fence_props.scale.x, fence_props.bounding_shape.y* fence_props.scale.x, fence_props.bounding_shape.z* fence_props.scale.x, fence_props.position);


	// Load the rusted car mode. Create a desk object. Set its properties
	engine::ref <engine::model> rusted_car_model = engine::model::create("assets/models/static/rustedcar.obj");
	engine::game_object_properties rusted_car_props;
	rusted_car_props.meshes = rusted_car_model->meshes();
	rusted_car_props.textures = rusted_car_model->textures();
	float rusted_car_scale = 3.f / glm::max(rusted_car_model->size().x, glm::max(rusted_car_model->size().y, rusted_car_model->size().z));
	rusted_car_props.position = { 0.f, 0.5f, 20.f };
	rusted_car_props.scale = glm::vec3(rusted_car_scale);
	rusted_car_props.bounding_shape = rusted_car_model->size() / 2.f * rusted_car_scale;
	m_rusted_car = engine::game_object::create(rusted_car_props);

	// Load the car model. Create a car model. Set its properties

	engine::ref <engine::model> car_model = engine::model::create("assets/models/static/car.obj");
	engine::game_object_properties car_props;
	car_props.meshes = car_model->meshes();
	car_props.textures = car_model->textures();
	float car_scale = 2.f / glm::max(car_model->size().x, glm::max(car_model->size().y, car_model->size().z));
	car_props.position = { -3.f, 0.5f, 20.f };
	car_props.scale = glm::vec3(car_scale);
	car_props.bounding_shape = car_model->size() / 2.f * car_scale;
	m_car = engine::game_object::create(car_props);

	// Load the jeep model. Create a jeep object. Set its properties
	engine::ref <engine::model> jeep_model = engine::model::create("assets/models/static/jeep1.obj");
	engine::game_object_properties jeep_props;
	jeep_props.meshes = jeep_model->meshes();
	jeep_props.textures = jeep_model->textures();
	float jeep_scale = 1.f;
	jeep_props.position = { -5.f,0.5f, 0.f };
	jeep_props.scale = glm::vec3(jeep_scale);
	jeep_props.bounding_shape = jeep_model->size() / 2.f * jeep_scale;
	m_jeep = engine::game_object::create(jeep_props); 

	// Load the tree model. Create a tree object. Set its properties
	engine::ref <engine::model> tree_model = engine::model::create("assets/models/static/elm.3ds");
	engine::game_object_properties tree_props;
	tree_props.meshes = tree_model->meshes();
	tree_props.textures = tree_model->textures();
	float tree_scale = 3.f / glm::max(tree_model->size().x, glm::max(tree_model->size().y, tree_model->size().z));
	tree_props.position = { 4.f, 0.5f, -5.f };
	tree_props.bounding_shape = tree_model->size() / 2.f * tree_scale;
	tree_props.scale = glm::vec3(tree_scale);
	m_tree = engine::game_object::create(tree_props);



	//-------------------------------------------Primitives------------------------------------------------------------------

	//Tetrahedron primitive
	std::vector<glm::vec3> tetrahedron_vertices;
	tetrahedron_vertices.push_back(glm::vec3(0.f, 2.f, 0.f));	//0
	tetrahedron_vertices.push_back(glm::vec3(0.f, 0.f, 2.f));	//1
	tetrahedron_vertices.push_back(glm::vec3(-2.f, 0.f, -2.f));	//2
	tetrahedron_vertices.push_back(glm::vec3(2.f, 0.f, -2.f));	//3
	engine::ref<engine::tetrahedron> tetrahedron_shape = engine::tetrahedron::create(tetrahedron_vertices);
	engine::game_object_properties tetrahedron_props;
	tetrahedron_props.position = { 0.f, 0.f, -20.f };
	tetrahedron_props.meshes = { tetrahedron_shape->mesh() };
	tetrahedron_props.textures = tetrahedron_textures;
	m_tetrahedron = engine::game_object::create(tetrahedron_props);

	// Load sphere
	engine::ref<engine::sphere> sphere_shape = engine::sphere::create(10, 20, 0.5f);
	engine::game_object_properties sphere_props;
	sphere_props.position = { -6.f, 6.f, 20.f };
	sphere_props.meshes = { sphere_shape->mesh() };
	sphere_props.textures = sphere_textures;
	sphere_props.type = 1;
	sphere_props.bounding_shape = glm::vec3(0.5f);
	sphere_props.restitution = 0.92f;
	sphere_props.mass = 0.000001f;
	m_ball = engine::game_object::create(sphere_props);


	// HyperRectangle 
	engine::ref<engine::rectangle> pickup_shape = engine::rectangle::create(glm::vec3(0.5f), false);
	engine::ref<engine::texture_2d> pickup_texture = engine::texture_2d::create("assets/textures/stonefence.jpg", true);
	engine::game_object_properties pickup_props;
	pickup_props.position = { 6.f, 1.f, 20.f };
	pickup_props.meshes = { pickup_shape->mesh() };
	pickup_props.textures = { pickup_texture };
	m_pickup = pickup::create(pickup_props);
	m_pickup->init();


	/*/ Rectangle primitive
	std::vector<glm::vec3> rectangle_vertices;
	rectangle_vertices.push_back(glm::vec3(0.f, 2.f, 0.f));		//0
	rectangle_vertices.push_back(glm::vec3(0.f, 0.f, 2.f));		//1
	rectangle_vertices.push_back(glm::vec3(-2.f, 0.f, -2.f));	//2
	rectangle_vertices.push_back(glm::vec3(2.f, 0.f, -2.f));	//3
	rectangle_vertices.push_back(glm::vec3(0.f, 2.f, 2.f));		//4
	rectangle_vertices.push_back(glm::vec3(2.f, 0.f, 0.f));		//5
	
	engine::ref<engine::rectangle> rectangle_shape = engine::rectangle::create(rectangle_vertices);
	engine::game_object_properties rectangle_props;
	rectangle_props.position = { 0.f, 1.f, -10.f };
	rectangle_props.meshes = { rectangle_shape->mesh() };
	rectangle_props.textures = rectangle_textures;
	m_rectangle = engine::game_object::create(rectangle_props);*/

	

	m_game_objects.push_back(m_terrain);
	m_game_objects.push_back(m_ball);
	//m_game_objects.push_back(m_cow);
	//m_game_objects.push_back(m_tree);
	//m_game_objects.push_back(m_pickup);
	m_physics_manager = engine::bullet_manager::create(m_game_objects);

	m_text_manager = engine::text_manager::create();

	m_skinned_mesh->switch_animation(1);

	
}

example_layer::~example_layer() {}

void example_layer::on_update(const engine::timestep& time_step) 
{
    //m_3d_camera.on_update(time_step);
	m_player.update_camera(m_3d_camera);

	m_physics_manager->dynamics_world_update(m_game_objects, double(time_step));

	glm::vec3 pos = m_player.object()->position();

	m_player.on_update(time_step);
	m_player_box.on_update(m_player.object()->position());

	if (m_fence_box.collision(m_player_box))
		m_player.object()->set_position(pos);
	m_fence_box.on_update(m_fence->position());



	check_bounce();
		
} 

void example_layer::on_render()
{
	engine::render_command::clear_color({ 0.2f, 0.3f, 0.3f, 1.0f });
	engine::render_command::clear();
	

	//const auto textured_shader = engine::renderer::shaders_library()->get("mesh_static");
	//engine::renderer::begin_scene(m_3d_camera, textured_shader);


	const auto textured_lighting_shader = engine::renderer::shaders_library()->get("mesh_lighting");
	engine::renderer::begin_scene(m_3d_camera, textured_lighting_shader);

	// Set up some of the scene's parameters in the shader
	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());
	std::dynamic_pointer_cast<engine::gl_shader>(textured_lighting_shader)->set_uniform("fog_on", m_fog);

	// Position the skybox centred on the player and render it
	glm::mat4 skybox_tranform(1.0f);
	skybox_tranform = glm::translate(skybox_tranform, m_3d_camera.position());
	for (const auto& texture : m_skybox->textures())
	{
		texture->bind();
	}
	engine::renderer::submit(textured_lighting_shader, m_skybox, skybox_tranform);

	engine::renderer::submit(textured_lighting_shader, m_terrain);

	engine::renderer::submit(textured_lighting_shader, m_tetrahedron);

	engine::renderer::submit(textured_lighting_shader, m_ball);

	//engine::renderer::submit(textured_lighting_shader, m_desk);

	//engine::renderer::submit(textured_lighting_shader, m_rectangle);

	m_player_box.on_render(2.5f, 0.f, 0.f, textured_lighting_shader);
	m_fence_box.on_render(2.5f, 0.f, 0.f, textured_lighting_shader);

	//Render the tree
	glm::mat4 tree_transform(1.0f);
	tree_transform = glm::translate(tree_transform, glm::vec3(4.f, 0.5, -5.0f));
	tree_transform = glm::rotate(tree_transform, m_tree->rotation_amount(), m_tree->rotation_axis());
	tree_transform = glm::scale(tree_transform, m_tree->scale());
	engine::renderer::submit(textured_lighting_shader, tree_transform, m_tree);

	//Render the cow
	glm::mat4 cow_transform(1.0f);
	cow_transform = glm::translate(cow_transform, m_cow->position());
	cow_transform = glm::rotate(cow_transform, m_cow->rotation_amount(), m_cow->rotation_axis());
	cow_transform = glm::scale(cow_transform, m_cow->scale());
	engine::renderer::submit(textured_lighting_shader, cow_transform, m_cow);

	//Render fence

	glm::mat4 fence_transform(1.0f);
	fence_transform = glm::translate(fence_transform, m_fence->position() - glm::vec3(m_fence->offset().x, 0.f, m_fence->offset().z) * m_fence->scale().x);
	fence_transform = glm::rotate(fence_transform, m_fence->rotation_amount(), m_fence->rotation_axis());
	fence_transform = glm::scale(fence_transform, m_fence->scale());
	engine::renderer::submit(textured_lighting_shader, fence_transform, m_fence);

	//Render the jeep
	glm::mat4 jeep_transform(1.0f);
	jeep_transform = glm::translate(jeep_transform, m_jeep->position());
	jeep_transform = glm::rotate(jeep_transform, m_jeep->rotation_amount(), m_jeep->rotation_axis());
	jeep_transform = glm::scale(jeep_transform, m_jeep->scale());
	engine::renderer::submit(textured_lighting_shader, jeep_transform, m_jeep);

	//Render the desk with the correct properties
	for (int i = 0; i < 4; i++)
	{
		glm::mat4 rusted_car_transform(1.0f);
		rusted_car_transform = glm::translate(rusted_car_transform, glm::vec3(-0.5f, 0.5, 20.0f + i * 4.0f));
		rusted_car_transform = glm::rotate(rusted_car_transform, m_rusted_car->rotation_amount(), m_rusted_car->rotation_axis());
		rusted_car_transform = glm::scale(rusted_car_transform, m_rusted_car->scale());
		engine::renderer::submit(textured_lighting_shader, rusted_car_transform, m_rusted_car);
	}

	// Render car

	glm::mat4 car_transform(1.0f);
	car_transform = glm::translate(car_transform, m_car->position());
	car_transform = glm::rotate(car_transform, m_car->rotation_amount(), m_car->rotation_axis());
	car_transform = glm::scale(car_transform, m_car->scale());
	engine::renderer::submit(textured_lighting_shader, car_transform, m_car);

	// Render HyperRectangle

	if (m_pickup->active()) {
		m_pickup->textures().at(0)->bind();
		glm::mat4 pickup_transform(1.0f);
		pickup_transform = glm::translate(pickup_transform, m_pickup->position());
		pickup_transform = glm::rotate(pickup_transform, m_pickup->rotation_amount(), m_pickup->rotation_axis());
		engine::renderer::submit(textured_lighting_shader, m_pickup->meshes().at(0), pickup_transform);
	}


	engine::renderer::end_scene();

	// Set up material shader. (does not render textures, renders materials instead)
	const auto material_shader = engine::renderer::shaders_library()->get("mesh_material");
	engine::renderer::begin_scene(m_3d_camera, material_shader);

	m_material->submit(material_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(material_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());
	std::dynamic_pointer_cast<engine::gl_shader>(material_shader)->set_uniform("fog_on", m_fog);

	engine::renderer::submit(material_shader, m_ball);

	engine::renderer::end_scene();

	const auto animated_mesh_shader = engine::renderer::shaders_library()->get("animated_mesh");

	engine::renderer::begin_scene(m_3d_camera, animated_mesh_shader);
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("gEyeWorldPos", m_3d_camera.position());
	std::dynamic_pointer_cast<engine::gl_shader>(animated_mesh_shader)->set_uniform("fog_on", m_fog);
	glm::mat4 aniTransform = glm::mat4(1.0f);

	engine::renderer::submit(animated_mesh_shader, m_player.object());
	
	// Render text
	


	score = std::to_string(x);
	x = y;
	y = y + 10;

	
		const auto text_shader = engine::renderer::shaders_library()->get("text_2D");
		m_text_manager->render_text(text_shader, "Score=", 10.f, (float)engine::application::window().height() - 100.f, 0.5f, glm::vec4(0.f, 1.f, 0.f, 1.f));
		m_text_manager->render_text(text_shader, score, 100.f, (float)engine::application::window().height() - 100.f, 0.5f, glm::vec4(0.f, 0.f, 1.f, 1.f));
		m_text_manager->render_text(text_shader, "A - move left", 10.f, (float)engine::application::window().height() - 25.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
		m_text_manager->render_text(text_shader, "D - move right", 10.f, (float)engine::application::window().height() - 50.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
		m_text_manager->render_text(text_shader, "Space - jump", 10.f, (float)engine::application::window().height() - 75.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));
		engine::renderer::end_scene();


	

	

	//m_text_manager->render_text(text_shader, score, 10.f, (float)engine::application::window().height() - 100.f, 0.5f, glm::vec4(1.f, 0.5f, 0.f, 1.f));



} 

void example_layer::on_event(engine::event& event) 
{ 
    if(event.event_type() == engine::event_type_e::key_pressed) 
    { 
        auto& e = dynamic_cast<engine::key_pressed_event&>(event); 
        if(e.key_code() == engine::key_codes::KEY_TAB) 
        { 
            engine::render_command::toggle_wireframe();
        }
		if (e.key_code() == engine::key_codes::KEY_5)
		{
			m_fog = !m_fog;
		}
    } 
}

void example_layer::check_bounce()
{
	if (m_prev_sphere_y_vel < 0.f && m_game_objects.at(1)->velocity().y > 0.f)
		m_audio_manager->play("bounce");
	m_prev_sphere_y_vel = m_game_objects.at(1)->velocity().y;
}
