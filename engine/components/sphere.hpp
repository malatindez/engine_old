#pragma once
#include "entt/entt.hpp"
#include "math.hpp"
#include "math/intersection.hpp"
#include "math/ray.hpp"
#include "transform.hpp"
namespace engine::components
{
    struct Sphere
    {
    public:
        static float Hit(math::Ray const& r, math::vec3 center, float radius) noexcept
        {
            // TODO
            // figure out how can we adjust this formula to the sphere scaled by different factors
            math::vec3 oc = r.origin() - center;
            float a = math::dot(r.direction(), r.direction());
            float b = 2.0f * math::dot(oc, r.direction());
            float c = math::dot(oc, oc) - radius;
            float discriminant = b * b - 4 * a * c;
            assert(discriminant > 0);
            if (discriminant < 0)
            {
                return -1.0f;
            }
            else
            {
                float d = sqrt(discriminant);
                return std::min(std::abs(-b - d), std::abs(-b + d)) / (2.0f * a);
            }
        }

        [[nodiscard]] static bool CheckIntersection(entt::registry reg, entt::entity entity, math::Intersection& i, math::Ray const& ray)
        {
            auto const& transform = reg.get<Transform>(entity);
            if (math::length(ray.origin() - transform.position) - transform.scale.x < i.t)
            {
                return false;
            }
            float t = Hit(ray, transform.position, transform.scale.x);

            if (t < i.t)
            {
                return false;
            }
            i.t = t;
            i.point = ray.direction() * t;
            return true;
        }
    };
}