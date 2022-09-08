#include "model-system.hpp"
namespace engine::render
{
    using namespace core::math;
    std::shared_ptr<ModelSystem> ModelSystem::instance_;
    namespace
    {
        inline bool CheckForIntersection(AABB const &aabb,
                                         Ray const &ray,
                                         Intersection &nearest)
        {
            float tmin, tmax, tymin, tymax, tzmin, tzmax;

            auto const &bounds = reinterpret_cast<std::array<vec3, 2> const &>(aabb);

            tmin = (bounds[ray.sign()[0]].x - ray.origin().x) * ray.inv_direction().x;
            tmax = (bounds[1ll - ray.sign()[0]].x - ray.origin().x) * ray.inv_direction().x;
            tymin = (bounds[ray.sign()[1]].y - ray.origin().y) * ray.inv_direction().y;
            tymax = (bounds[1ll - ray.sign()[1]].y - ray.origin().y) * ray.inv_direction().y;

            if ((tmin > tymax) || (tymin > tmax))
                return false;

            if (tymin > tmin)
                tmin = tymin;
            if (tymax < tmax)
                tmax = tymax;

            tzmin = (bounds[ray.sign()[2]].z - ray.origin().z) * ray.inv_direction().z;
            tzmax = (bounds[1ll - ray.sign()[2]].z - ray.origin().z) * ray.inv_direction().z;

            if ((tmin > tzmax) || (tzmin > tmax))
                return false;

            if (tzmin > tmin)
                tmin = tzmin;
            if (tzmax < tmax)
                tmax = tzmax;

            float t = tmin;

            if (t < 0)
            {
                t = tmax;
                if (t < 0 || t > nearest.t)
                    return false;
            }
            nearest.t = t;
            nearest.point = ray.PointAtParameter(t);
            return true;
        }
        bool CheckForIntersection(Model const &model,
                                  components::TransformComponent transform,
                                  Ray const &ray,
                                  Intersection &nearest)
        {
            Ray local = ray;
            local.SetDirection(normalize((core::math::vec4{ local.direction(), 0 } *transform.inv_model).xyz));
            local.origin() = (core::math::vec4{ local.origin(), 1 } *transform.inv_model).xyz;
            bool rv = false;
            for (auto const &mesh : model.meshes)
            {
                Ray mesh_local = local;
                mesh_local.origin() = (core::math::vec4{ local.origin(), 1 } *mesh.inv_mesh_to_model).xyz;
                mesh_local.SetDirection((core::math::vec4{ local.direction(), 0 } *mesh.inv_mesh_to_model).xyz);
                bool t = CheckForIntersection(mesh.mesh_range.bounding_box, mesh_local, nearest);
                if (t)
                {
                    nearest.point = (core::math::vec4{ nearest.point, 1 } *mesh.mesh_to_model).xyz;
                    nearest.point = (core::math::vec4{ nearest.point, 1 } *transform.model).xyz;
                    nearest.t *= core::math::length(mesh_local.direction());
                    rv = t;
                }
            }
            return rv;
        }
    }
    std::optional<entt::entity> ModelSystem::FindIntersection(entt::registry &registry,
                                                              Ray const &ray,
                                                              Intersection &nearest)
    {
        auto group = registry.group<components::OpaqueComponent, components::TransformComponent>();
        std::optional<entt::entity> rv = std::nullopt;
        group.each([&rv, &ray, &nearest] (auto const entity, auto const &model_instance, auto const &transform)
                   {
                       auto const &model = GetModel(model_instance.model_id);
                       if (CheckForIntersection(model, transform, ray, nearest))
                       {
                           rv = entity;
                       }
                   });
        return rv;
    }
    uint64_t ModelSystem::AddModel(Model &&model_)
    {
        instance_->models_.emplace(current_index, std::move(model_));
        return current_index++;
    }

} // namespace engine::render