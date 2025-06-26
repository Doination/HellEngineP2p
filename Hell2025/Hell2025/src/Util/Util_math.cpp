#include "Util.h"
#include "HellDefines.h"
#include <numeric>
#include <random>
#include <glm/gtx/quaternion.hpp>

namespace Util {

    glm::vec3 EulerRotationFromNormal(glm::vec3 normal, glm::vec3 forward) {
        normal = glm::normalize(normal);
        glm::quat q = glm::rotation(forward, normal);
        glm::vec3 e = glm::eulerAngles(q);
        e.z = 0.0f;
        return e;
    }

    float YRotationBetweenTwoPoints(glm::vec3 a, glm::vec3 b) {
        float delta_x = b.x - a.x;
        float delta_y = b.z - a.z;
        float theta_radians = atan2(delta_y, delta_x);
        return -theta_radians;
    }

    glm::mat4 GetRotationMat4FromForwardVector(glm::vec3 forward) {
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

        forward = glm::normalize(forward);
        glm::vec3 right = glm::normalize(glm::cross(worldUp, forward));
        worldUp = glm::cross(forward, right);
        glm::mat4 rotation = glm::mat4(1.0f);

        rotation[0] = glm::vec4(right, 0.0f);
        rotation[1] = glm::vec4(worldUp, 0.0f);
        rotation[2] = glm::vec4(-forward, 0.0f);

        return rotation;
    }

    glm::vec3 GetMidPoint(const glm::vec3& a, const glm::vec3 b) {
        return a + b * 0.5f;
    }

    float EulerYRotationBetweenTwoPoints(glm::vec3 a, glm::vec3 b) {
        float delta_x = b.x - a.x;
        float delta_y = b.z - a.z;
        float theta_radians = atan2(delta_y, delta_x);
        return -theta_radians;
    }

    glm::mat4 RotationMatrixFromForwardVector(glm::vec3 forward, glm::vec3 worldForward, glm::vec3 worldUp) {
        forward = glm::normalize(forward);
        worldForward = glm::normalize(worldForward);  // e.g., (0, 0, 1)
        // Compute the quaternion that rotates from worldForward to forward.
        glm::quat q = glm::rotation(worldForward, forward);
        return glm::mat4_cast(q);

        //forward = glm::normalize(forward);
        //worldForward = glm::normalize(worldForward);
        //worldUp = glm::normalize(worldUp);
        //
        //// If forward == worldForward, return identity
        //if (glm::all(glm::epsilonEqual(forward, worldForward, 0.0001f))) {
        //    return glm::mat4(1.0f);
        //}
        //
        //// If forward is exactly worldUp, return a rotation that points straight up
        //if (glm::all(glm::epsilonEqual(forward, worldUp, 0.0001f))) {
        //    return glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1, 0, 0)); // Rotate 90� around X-axis
        //}
        //
        //// If forward is exactly -worldUp, return a rotation that points straight down
        //if (glm::all(glm::epsilonEqual(forward, -worldUp, 0.0001f))) {
        //    return glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1, 0, 0)); // Rotate -90� around X-axis
        //}
        //
        //glm::quat q = glm::quatLookAt(forward, worldUp);
        //glm::quat qWorld = glm::quatLookAt(worldForward, worldUp);
        //
        //return glm::mat4_cast(q * glm::inverse(qWorld));
    }

    AABB GetAABBFromPoints(std::vector<glm::vec3>& points) {
        glm::vec3 aabbMin(std::numeric_limits<float>::max());
        glm::vec3 aabbMax(std::numeric_limits<float>::lowest());

        for (const glm::vec3& point : points) {
            aabbMin = glm::min(aabbMin, point);
            aabbMax = glm::max(aabbMax, point);
        }

        return AABB(aabbMin, aabbMax);
    }

    glm::vec2 ComputeCentroid2D(const std::vector<glm::vec2>& points) {
        glm::vec2 centroid(0.0f);
        for (const auto& p : points) {
            centroid += p;
        }
        return centroid / static_cast<float>(points.size());
    }

    // Sort the points in a valid clockwise order around the centroid
    std::vector<glm::vec2> SortConvexHullPoints2D(std::vector<glm::vec2>& points) {
        glm::vec2 centroid = ComputeCentroid2D(points);

        std::sort(points.begin(), points.end(), [&](const glm::vec2& a, const glm::vec2& b) {
            float angleA = atan2(a.y - centroid.y, a.x - centroid.x);
            float angleB = atan2(b.y - centroid.y, b.x - centroid.x);
            return angleA < angleB;
        });

        return points;
    }

    std::vector<glm::vec2> ComputeConvexHull2D(std::vector<glm::vec2> points) {
        if (points.size() <= 3) return points;  // Already a valid shape

        // Sort points by x-coordinate, break ties by y-coordinate
        std::sort(points.begin(), points.end(), [](const glm::vec2& a, const glm::vec2& b) {
            return a.x < b.x || (a.x == b.x && a.y < b.y);
        });

        std::vector<glm::vec2> hull;

        // Build lower hull
        for (const auto& p : points) {
            while (hull.size() >= 2 && Cross2D(hull[hull.size() - 2], hull[hull.size() - 1], p) <= 0) {
                hull.pop_back();
            }
            hull.push_back(p);
        }

        // Build upper hull
        size_t lowerSize = hull.size();
        for (int i = points.size() - 1; i >= 0; i--) {
            while (hull.size() > lowerSize && Cross2D(hull[hull.size() - 2], hull[hull.size() - 1], points[i]) <= 0) {
                hull.pop_back();
            }
            hull.push_back(points[i]);
        }

        hull.pop_back();  // Remove last duplicate point
        return hull;
    }

    float Cross2D(const glm::vec2& O, const glm::vec2& A, const glm::vec2& B) {
        return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
    }

    glm::vec3 ClosestPointOnSegmentToRay(const glm::vec3& A, const glm::vec3& B, const glm::vec3& rayOrigin, const glm::vec3& rayDir) {
        // Compute edge vector and its squared length.
        glm::vec3 u = B - A;
        float a = glm::dot(u, u);
        if (a < 1e-6f) // the segment is degenerate
            return A;

        // For the ray, we have: R(s) = rayOrigin + s * rayDir.
        // Let v = rayDir (assumed normalized) and define:
        glm::vec3 v = rayDir; // assume rayDir is normalized
        float c = glm::dot(v, v); // should be 1 if normalized
        // And the vector from A to rayOrigin:
        glm::vec3 w = A - rayOrigin;

        float b = glm::dot(u, v);
        float d = glm::dot(u, w);
        float e = glm::dot(v, w);

        // Denom: a*c - b*b
        float denom = a * c - b * b;
        float t = 0.0f;
        if (fabs(denom) > 1e-6f)
        {
            // Parameter for the segment (not the ray)
            t = (b * e - c * d) / denom;
        }
        // Clamp t to [0, 1] to stay on the segment.
        t = glm::clamp(t, 0.0f, 1.0f);

        return A + t * u;
    }

    float DistanceSquared(const glm::vec3& a, const glm::vec3& b) {
        glm::vec3 diff = a - b;
        return diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
    }

    float ManhattanDistance(const glm::vec3& a, const glm::vec3& b) {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y) + std::abs(a.z - b.z);
    }

    glm::ivec2 WorldToScreenCoords(const glm::vec3& worldPos, const glm::mat4& viewProjection, int screenWidth, int screenHeight, bool flipY) {
        glm::vec4 clipCoords = viewProjection * glm::vec4(worldPos, 1.0f);
        glm::vec3 ndcCoords = glm::vec3(clipCoords) / clipCoords.w;
        glm::ivec2 screenCoords;
        screenCoords.x = (ndcCoords.x + 1.0f) * 0.5f * screenWidth;
        screenCoords.y = flipY
            ? screenHeight - (ndcCoords.y + 1.0f) * 0.5f * screenHeight
            : (1.0f - ndcCoords.y) * 0.5f * screenHeight;
        return screenCoords;
    }

    bool IsWithinThreshold(const glm::ivec2& pointA, const glm::ivec2& pointB, float threshold) {
        // Calculate the difference in x and y coordinates.
        float dx = static_cast<float>(pointA.x - pointB.x);
        float dy = static_cast<float>(pointA.y - pointB.y);

        // Check if the Euclidean distance is within the threshold.
        return (dx * dx + dy * dy) <= (threshold * threshold);
    }

    //glm::ivec2 WorldToScreenCoordsOrtho(const glm::vec3& worldPos, const glm::mat4& orthoMatrix, int screenWidth, int screenHeight, bool flipY) {
    //    glm::vec4 clipCoords = orthoMatrix * glm::vec4(worldPos, 1.0f);
    //    glm::vec2 ndcCoords(clipCoords.x, clipCoords.y);
    //    glm::ivec2 screenCoords;
    //    screenCoords.x = static_cast<int>((ndcCoords.x + 1.0f) * 0.5f * screenWidth);
    //    if (flipY) {
    //        screenCoords.y = screenHeight - static_cast<int>(((ndcCoords.y + 1.0f) * 0.5f * screenHeight));
    //    }
    //    else {
    //        screenCoords.y = static_cast<int>((ndcCoords.y + 1.0f) * 0.5f * screenHeight);
    //    }
    //    return screenCoords;
    //}

    float FInterpTo(float current, float target, float deltaTime, float interpSpeed) {
        if (interpSpeed <= 0.f)
            return target;
        const float Dist = target - current;
        if (Dist * Dist < 9.99999993922529e-9f)
            return target;
        return current + Dist * glm::clamp(deltaTime * interpSpeed, 0.0f, 1.0f);
    }

    glm::vec3 LerpVec3(glm::vec3 current, glm::vec3 target, float deltaTime, float interpSpeed) {
        glm::vec3 result;
        result.x = FInterpTo(current.x, target.x, deltaTime, interpSpeed);
        result.y = FInterpTo(current.y, target.y, deltaTime, interpSpeed);
        result.z = FInterpTo(current.z, target.z, deltaTime, interpSpeed);
        return result;
    }

    float RandomFloat(float min, float max) {
        return min + static_cast<float>(rand()) / (RAND_MAX / (max - min));
    }

    int RandomInt(int min, int max) {
        static std::random_device dev;
        static std::mt19937 rng(dev());
        std::uniform_int_distribution<int> dist(min, max);
        return dist(rng);
    }

    void InterpolateQuaternion(glm::quat& Out, const glm::quat& Start, const glm::quat& End, float pFactor) {
        // calc cosine theta
        float cosom = Start.x * End.x + Start.y * End.y + Start.z * End.z + Start.w * End.w;
        // adjust signs (if necessary)
        glm::quat end = End;
        if (cosom < static_cast<float>(0.0)) {
            cosom = -cosom;
            end.x = -end.x;   // Reverse all signs
            end.y = -end.y;
            end.z = -end.z;
            end.w = -end.w;
        }
        // Calculate coefficients
        float sclp, sclq;
        if ((static_cast<float>(1.0) - cosom) > static_cast<float>(0.0001)) // 0.0001 -> some epsillon
        {
            // Standard case (slerp)
            float omega, sinom;
            omega = std::acos(cosom); // extract theta from dot product's cos theta
            sinom = std::sin(omega);
            sclp = std::sin((static_cast<float>(1.0) - pFactor) * omega) / sinom;
            sclq = std::sin(pFactor * omega) / sinom;
        }
        else {
            // Very close, do linear interp (because it's faster)
            sclp = static_cast<float>(1.0) - pFactor;
            sclq = pFactor;
        }
        Out.x = sclp * Start.x + sclq * end.x;
        Out.y = sclp * Start.y + sclq * end.y;
        Out.z = sclp * Start.z + sclq * end.z;
        Out.w = sclp * Start.w + sclq * end.w;
    }

    void NormalizeWeights(std::vector<float>& weights) {
        // Calculate the sum of all weights
        float sum = std::accumulate(weights.begin(), weights.end(), 0.0f);
        // Check if the sum is non-zero to avoid division by zero
        if (sum == 0.0f) {
            throw std::invalid_argument("Sum of weights cannot be zero.");
        }
        // Normalize each weight
        for (float& weight : weights) {
            weight /= sum;
        }
    }

    bool IsNan(float value) {
        return glm::isnan(value);
    }

    bool IsNan(glm::vec2 value) {
        return glm::isnan(value.x) || glm::isnan(value.y);
    }

    bool IsNan(glm::vec3 value) {
        return glm::isnan(value.x) || glm::isnan(value.y) || glm::isnan(value.z);
    }

    bool IsNan(glm::vec4 value) {
        return glm::isnan(value.x) || glm::isnan(value.y) || glm::isnan(value.z) || glm::isnan(value.w);
    }

    float GetDensity(float mass, float volume) {
        return mass / volume;
    }

    float GetCubeVolume(const glm::vec3& halfExtents) {
        return 8.0f * halfExtents.x * halfExtents.y * halfExtents.z;
    }

    float GetCubeVolume(const float& halfWidth, const float& halfHeight, const float& halfDepth) {
        return GetCubeVolume(glm::vec3(halfWidth, halfHeight, halfDepth));
    }

    float GetSphereVolume(float radius) {
        return (4.0f / 3.0f) * HELL_PI * radius * radius * radius;
    }

    float GetCapsuleVolume(float radius, float halfHeight) {
        float cylHeight = halfHeight * 2.0f;
        float cylVol = HELL_PI * radius * radius * cylHeight;
        float sphVol = (4.0f / 3.0f) * HELL_PI * radius * radius * radius;
        return cylVol + sphVol;
    }

    float GetConvexHullVolume(const std::span<Vertex>& vertices, const std::span<unsigned int>& indices) {
        // Compute the centroid to use as the reference point
        glm::vec3 reference(0.0f);
        for (const Vertex& v : vertices) {
            reference += v.position;
        }
        reference /= static_cast<float>(vertices.size());

        float totalVolume = 0.0f;

        // Each consecutive group of three indices defines a triangle
        for (size_t i = 0; i < indices.size(); i += 3) {
            const glm::vec3& v0 = vertices[indices[i]].position;
            const glm::vec3& v1 = vertices[indices[i + 1]].position;
            const glm::vec3& v2 = vertices[indices[i + 2]].position;

            // Compute the tetrahedron volume formed by the triangle (v0, v1, v2) and the reference point
            glm::vec3 crossProd = glm::cross(v1 - v0, v2 - v0);
            float tetraVolume = std::abs(glm::dot(crossProd, reference - v0)) / 6.0f;
            totalVolume += tetraVolume;
        }
        return totalVolume;
    }

    bool IsPointInTriangle2D(const glm::vec2& pt, const glm::vec2& v0, const glm::vec2& v1, const glm::vec2& v2) {
        glm::vec2 v0v1 = v1 - v0;
        glm::vec2 v0v2 = v2 - v0;
        glm::vec2 v0pt = pt - v0;
        // Precompute the determinant to avoid division in each barycentric calculation
        float denom = v0v1.x * v0v2.y - v0v1.y * v0v2.x;
        // Early out if the denominator is zero (i.e., the triangle is degenerate)
        if (denom == 0.0f) return false;
        float invDenom = 1.0f / denom;
        // Compute the barycentric coordinates (u, v, w) directly
        float v = (v0pt.x * v0v2.y - v0pt.y * v0v2.x) * invDenom;
        float w = (v0v1.x * v0pt.y - v0v1.y * v0pt.x) * invDenom;
        float u = 1.0f - v - w;
        // Check if point is inside the triangle
        return (u >= 0.0f) && (v >= 0.0f) && (w >= 0.0f);
    }
}