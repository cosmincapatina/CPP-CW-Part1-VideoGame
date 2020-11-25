#pragma once

namespace engine
{
	class mesh;

	/// \brief Class creating a rectangle object with a mesh of a specified size
	class rectangle
	{
	public:
		/// \brief Constructor
		rectangle(glm::vec3 half_extents, bool inwards);

		/// \brief Destructor
		~rectangle();

		/// \brief Getter methods
		glm::vec3 half_extents() const { return m_half_extents; }
		bool inwards() const { return m_inwards; }
		ref<engine::mesh> mesh() const { return m_mesh; }

		static ref<rectangle> create(glm::vec3 half_extents, bool inwards);
	private:
		/// \brief Fields
		// Dimensions of the rectangle specified by a vector of half-extents. Rectangle is centered on the origin in local coordinates.
		glm::vec3 m_half_extents;

		// Boolean recording if the rectangle should be inverted (used in skybox)
		bool m_inwards;

		ref<engine::mesh> m_mesh;
	};
}
