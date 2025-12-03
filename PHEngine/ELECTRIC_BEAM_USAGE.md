# ElectricBeamComponent Usage Examples

## Overview
ElectricBeamComponent provides three rendering modes for creating electric beam effects:

1. **Lines Mode** - Fast, billboard-style lines (RuntimeGeneratedLineComponent)
2. **ProceduralMesh Mode** - True 3D cylindrical geometry with jitter
3. **ProceduralElectric Mode** - Advanced 3D geometry with segmented jitter for realistic electric arcs

## Basic Usage (Lua)

### Creating a Simple Electric Beam
```lua
local actorId = _CreateActorAndAddToScene("ElectricBarrier")
_CreateAndAttachComponentToActor(actorId, "ElectricBeamComponent", Json.encode({
    gameObjectName = "ElectricBeam",
    startPoint = {x = 0, y = 0, z = 0},
    endPoint = {x = 10, y = 0, z = 0},
    beamColor = {r = 0.3, g = 0.5, b = 1.0},
    beamThickness = 2.0,
    beamCount = 3,
    jitterAmount = 0.2,
    updateFrequency = 0.05,
    isActive = true
}))
```

## Advanced Usage (C++)

### Switching Render Modes
```cpp
auto beam = gameObject->GetComponent<ElectricBeamComponent>();

// Use simple lines (fast, for many beams)
beam->SetRenderMode(BeamRenderMode::Lines);

// Use procedural mesh (realistic 3D volume)
beam->SetRenderMode(BeamRenderMode::ProceduralMesh);
beam->SetGeometrySegments(8, 5); // 8 radial segments, 5 length segments

// Use electric arc mode (most realistic, with smooth animation)
beam->SetRenderMode(BeamRenderMode::ProceduralElectric);
beam->SetGeometrySegments(12, 15); // More segments = smoother arc
beam->SetJitterAmount(0.5f); // Increase jitter for more chaotic effect
beam->SetAnimationSpeed(3.0f); // Fast animation for energetic effect
```

### Creating Multiple Beams for a Barrier
```cpp
// Create a horizontal barrier with multiple parallel beams
for (int i = 0; i < 5; ++i) {
    auto beam = std::make_shared<ElectricBeamComponent>("Barrier_Beam_" + std::to_string(i));
    
    float yOffset = i * 0.5f; // Space beams vertically
    beam->SetStartPoint(glm::vec3(0.0f, yOffset, 0.0f));
    beam->SetEndPoint(glm::vec3(20.0f, yOffset, 0.0f));
    
    // Alternate colors for variety
    if (i % 2 == 0) {
        beam->SetBeamColor(glm::vec3(0.3f, 0.5f, 1.0f)); // Blue
    } else {
        beam->SetBeamColor(glm::vec3(0.8f, 0.3f, 1.0f)); // Purple
    }
    
    beam->SetBeamCount(3);
    beam->SetRenderMode(BeamRenderMode::ProceduralElectric);
    beam->SetUpdateFrequency(0.03f); // Fast animation
    
    gameObject->AddComponent(beam);
}
```

### Using ProceduralBeamGeometry Directly
```cpp
#include "Core/GameCore/Components/ProceduralBeamGeometry.h"

// Generate simple cylindrical beam
std::vector<BeamVertex> vertices;
std::vector<uint32_t> indices;

ProceduralBeamGeometry::GenerateBeamGeometry(
    glm::vec3(0, 0, 0),      // Start point
    glm::vec3(10, 0, 0),     // End point
    0.5f,                     // Radius
    8,                        // Radial segments (8 = octagonal)
    vertices,
    indices
);

// Generate electric beam with jittered segments
ProceduralBeamGeometry::GenerateElectricBeamGeometry(
    glm::vec3(0, 0, 0),      // Start point
    glm::vec3(10, 5, 0),     // End point
    0.3f,                     // Radius
    12,                       // Radial segments (12 = smoother)
    20,                       // Length segments (more = more detail)
    0.4f,                     // Jitter amount
    vertices,
    indices
);

// Generate tapered beam (cone-like, e.g., for lightning strike)
ProceduralBeamGeometry::GenerateTaperedBeamGeometry(
    glm::vec3(0, 10, 0),     // Start point (wide end)
    glm::vec3(0, 0, 0),      // End point (narrow end)
    1.0f,                     // Start radius
    0.1f,                     // End radius
    16,                       // Segments
    vertices,
    indices
);

// Use vertices/indices to create mesh...
```

## Performance Considerations

### Lines Mode
- **Best for:** Many beams, distant effects, UI elements
- **Pros:** Very fast, low memory, simple
- **Cons:** Always faces camera, less realistic

### ProceduralMesh Mode
- **Best for:** Medium number of beams, close-up views
- **Pros:** True 3D volume, realistic from all angles
- **Cons:** More expensive than lines

### ProceduralElectric Mode
- **Best for:** Hero effects, cinematic moments, few beams
- **Pros:** Most realistic, natural electric arc appearance
- **Cons:** Most expensive (many segments)

## Animation System

### How Jitter Animation Works

The `ProceduralElectric` mode uses a smooth noise function (Perlin-like) to animate the jitter:

1. **Continuous Time Update**: `Tick()` method updates `mAnimationTime` every frame
2. **Smooth Noise**: Uses sine-based 3D noise for organic, continuous motion
3. **Per-Beam Phase Offset**: Each beam gets a unique time offset for variety
4. **Radius Pulsing**: Optional subtle pulsing effect synchronized with jitter

### Animation Speed Guidelines

```cpp
// Static beam (no animation)
beam->SetAnimationSpeed(0.0f);

// Slow, lazy arc (electric fence, barrier)
beam->SetAnimationSpeed(0.5f);

// Normal electric arc (default)
beam->SetAnimationSpeed(2.0f);

// Fast, energetic arc (tesla coil, high voltage)
beam->SetAnimationSpeed(5.0f);

// Chaotic, violent arc (lightning, explosion)
beam->SetAnimationSpeed(10.0f);
```

### Combining Update Frequency and Animation Speed

- **updateFrequency**: How often geometry is regenerated (affects performance)
- **animationSpeed**: How fast the noise function evolves (affects visual speed)

Example:
```cpp
// Smooth 60fps animation, fast visual motion
beam->SetUpdateFrequency(0.016f); // 60 fps
beam->SetAnimationSpeed(3.0f);

// Choppy 20fps animation, slow visual motion  
beam->SetUpdateFrequency(0.05f); // 20 fps
beam->SetAnimationSpeed(1.0f);
```

**Best Practice**: Match update frequency to frame rate, adjust animation speed for effect.

## Parameter Tuning Guide

### beamThickness
- **Lines mode:** Pixel width (1.0 - 5.0)
- **Mesh modes:** Radius multiplier (0.5 - 3.0)

### beamCount
- Multiple parallel beams for thickness
- Lines: 1-10 recommended
- Mesh: 1-5 recommended (expensive)

### jitterAmount
- Controls randomness
- 0.0 = straight beam
- 0.1-0.3 = subtle vibration
- 0.5-1.0 = chaotic arc

### updateFrequency
- How often beam regenerates (seconds)
- 0.016 = 60fps (very smooth, expensive)
- 0.05 = 20fps (good balance)
- 0.1 = 10fps (stuttery but cheap)

### Geometry Segments
- **Radial segments:** Roundness of cylinder
  - 6 = hexagonal (chunky)
  - 8 = octagonal (good)
  - 12 = smooth (recommended)
  - 16+ = very smooth (overkill)

- **Length segments:** Arc detail
  - 5 = blocky arc
  - 10 = decent arc (recommended)
  - 20 = smooth arc (expensive)
  - 30+ = very smooth (rarely needed)

## Common Use Cases

### Energy Barrier
```cpp
beam->SetBeamColor(glm::vec3(0.0f, 1.0f, 1.0f)); // Cyan
beam->SetBeamCount(5);
beam->SetJitterAmount(0.15f);
beam->SetUpdateFrequency(0.05f);
beam->SetRenderMode(BeamRenderMode::Lines);
```

### Lightning Strike
```cpp
beam->SetBeamColor(glm::vec3(1.0f, 1.0f, 0.8f)); // White-yellow
beam->SetBeamCount(1);
beam->SetJitterAmount(0.8f);
beam->SetUpdateFrequency(0.033f); // 30 fps update
beam->SetAnimationSpeed(5.0f); // Very fast, chaotic animation
beam->SetRenderMode(BeamRenderMode::ProceduralElectric);
beam->SetGeometrySegments(8, 25);
```

### Laser Beam
```cpp
beam->SetBeamColor(glm::vec3(1.0f, 0.0f, 0.0f)); // Red
beam->SetBeamCount(1);
beam->SetJitterAmount(0.0f); // No jitter for laser
beam->SetAnimationSpeed(0.0f); // No animation
beam->SetUpdateFrequency(0.0f); // Static
beam->SetRenderMode(BeamRenderMode::ProceduralMesh);
beam->SetGeometrySegments(6, 2); // Simple cylinder
```

### Pulsing Energy Beam
```cpp
beam->SetBeamColor(glm::vec3(0.0f, 1.0f, 0.5f)); // Green-cyan
beam->SetBeamCount(2);
beam->SetJitterAmount(0.15f); // Slight jitter
beam->SetAnimationSpeed(1.5f); // Moderate pulsing
beam->SetUpdateFrequency(0.033f); // 30 fps
beam->SetRenderMode(BeamRenderMode::ProceduralElectric);
beam->SetGeometrySegments(12, 12);
// The radius will pulse automatically in ProceduralElectric mode
```

### Plasma Arc
```cpp
beam->SetBeamColor(glm::vec3(0.8f, 0.2f, 1.0f)); // Purple
beam->SetBeamCount(3);
beam->SetJitterAmount(0.4f);
beam->SetUpdateFrequency(0.02f); // Very dynamic
beam->SetAnimationSpeed(4.0f); // Fast organic animation
beam->SetRenderMode(BeamRenderMode::ProceduralElectric);
beam->SetGeometrySegments(10, 15);
```
