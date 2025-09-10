# Copilot Instructions for RNGFix SourcePawn Plugin

## Repository Overview

This repository contains **RNGFix**, a sophisticated SourceMod plugin for Source engine games (CSS/CS:GO) that fixes physics bugs in movement-based game modes like bhop and surf. The plugin addresses six critical "random" physics issues that affect competitive movement gameplay by ensuring deterministic behavior in player physics interactions.

### Core Purpose
RNGFix eliminates pseudo-random physics behaviors that can make or break precise movement sequences. It fixes:
- **Downhill Inclines**: Ensures consistent speed boosts when landing on slopes
- **Uphill Inclines**: Prevents random speed loss when jumping up inclines  
- **Trigger Jumping**: Fixes thin triggers being skipped when jumped on
- **Telehops**: Prevents collision+teleport conflicts that randomly stop player speed
- **Edge Bugs**: Ensures consistent landing/jumping behavior on platform edges
- **Stair Sliding**: Enables consistent stair sliding on surf maps

## Technical Environment

- **Language**: SourcePawn (.sp files)
- **Platform**: SourceMod 1.10+ (currently supports CSS, designed for CS:GO compatibility)
- **Dependencies**: DHooks extension (required), Movement Unlocker (optional for CS:GO sliding)
- **Build System**: SourceKnight (Docker-based SourceMod compiler)
- **Build Configuration**: `sourceknight.yaml`
- **CI/CD**: GitHub Actions using `maxime1907/action-sourceknight@v1`

### Build Commands
```bash
# Build with SourceKnight (if available locally)
sourceknight build

# Build via CI (automatic on push/PR)
# See .github/workflows/ci.yml for full pipeline
```

### File Structure
```
addons/sourcemod/
├── scripting/
│   ├── rngfix.sp              # Main plugin source (1343 lines)
│   └── include/rngfix.inc     # Native functions API
├── gamedata/
│   └── rngfix.games.txt       # Game signatures and memory offsets
└── plugins/                   # Output directory for compiled .smx files
```

## Code Style & Standards

### Existing Code Patterns (FOLLOW THESE)
- **Indentation**: 4-space tabs (already consistent in codebase)
- **Variables**: camelCase for locals (`float velocity[3]`), PascalCase for functions (`CheckJumpButton`)
- **Globals**: `g_` prefix (`g_iTick`, `g_cvDownhill`)
- **Constants**: ALL_CAPS with underscores (`NON_JUMP_VELOCITY`, `MIN_STANDABLE_ZNRM`)
- **Pragmas**: Already uses `#pragma semicolon 1` and `#pragma newdecls required`

### Memory Management (CRITICAL)
- Use `delete` for cleanup - **never check for null first** (delete handles null automatically)
- **NEVER use `.Clear()` on StringMap/ArrayList** - causes memory leaks, use `delete` instead
- Use StringMap/ArrayList instead of arrays where appropriate
- Proper Handle cleanup in OnPluginEnd() if necessary

### Performance Requirements (CRITICAL FOR MOVEMENT PLUGINS)
- **Minimize operations in frequently called functions** (hooks run every tick for every player)
- **Cache expensive calculations** - avoid repeated vector operations
- **Optimize algorithmic complexity** - aim for O(1) instead of O(n) where possible
- **Avoid string operations in tick-critical paths**
- **Consider server tick rate impact** - code runs 66-128 times per second per player

## Plugin-Specific Architecture

### Key Components
1. **Pre-tick Prediction** (`DHook_ProcessMovementPre`): Predicts and prevents problematic collisions
2. **Post-tick Correction** (`Hook_PlayerPostThink`): Corrects physics results after tick simulation
3. **Trigger Tracking** (`Hook_TriggerStartTouch/EndTouch`): Monitors trigger interactions
4. **Physics Simulation**: Recreates engine movement calculations for prediction

### Critical Functions (Handle with Extreme Care)
- `RunPreTickChecks()`: Core prediction logic - affects all movement
- `DoInclineCollisionFixes()`: Speed-affecting collision corrections
- `PreventCollision()`: Modifies player position mid-tick
- `SetVelocity()`: Applies corrected velocities while preserving basevelocity

### Configuration System
All fixes can be individually disabled via ConVars:
- `rngfix_downhill`, `rngfix_uphill`, `rngfix_edge`, `rngfix_triggerjump`, `rngfix_telehop`, `rngfix_stairs`
- Debug modes: `rngfix_debug` (1=console, 2=console+visual)

## Development Guidelines

### Making Changes
1. **Understand the physics first** - Read `tech.md` to understand the technical details of each fix
2. **Test on movement maps** - Changes must be verified on bhop and surf maps
3. **Consider tickrate impact** - Test on both 64-tick and 128-tick servers
4. **Profile performance** - Use SourceMod's profiler for tick-critical code
5. **Preserve determinism** - Any randomness defeats the plugin's purpose

### Testing Requirements
- **No automated tests exist** (movement plugins require server testing)
- **Manual testing process**:
  1. Compile plugin with SourceKnight
  2. Load on CSS test server with movement maps
  3. Test each fix individually using ConVar toggles
  4. Verify no performance regression with `sm_profiler`
  5. Test edge cases on different map geometries

### Documentation Requirements
- **Update technical documentation** (`tech.md`) for any physics changes
- **Comment complex physics calculations** - include mathematical reasoning
- **Document ConVar changes** in plugin info and README
- **No header comments needed** for the main plugin file (per existing style)
- **Document native functions** in `.inc` files with full parameter descriptions

### Native Functions API
The plugin exposes an API via `rngfix.inc`:
- `RNGFix_OnTriggerDetected()`: When trigger entities are hooked
- `RNGFix_OnTriggerStartTouch()`/`RNGFix_OnTriggerEndTouch()`: Trigger touch events
- `RNGFix_OnTriggerTeleportTouchPost()`: Post-teleport trigger activation

## Performance Considerations (CRITICAL)

### Tick-Critical Code Paths
- **Pre-tick hooks**: Run before every player movement tick
- **Post-tick hooks**: Run after every player movement tick  
- **Trigger hooks**: Run for every trigger interaction

### Optimization Strategies
- **Early returns**: Exit functions as early as possible when fixes aren't needed
- **Conditional compilation**: Use ConVar checks to skip entire fix categories
- **Vector operations**: Cache results, avoid repeated calculations
- **Memory allocation**: Minimize dynamic allocations in hot paths
- **Float precision**: Be aware that small position adjustments can accumulate

### Debugging and Profiling
- Use `rngfix_debug 2` for visual debugging (laser beams show collision predictions)
- Monitor `DebugMsg()` output for understanding fix activation
- Use SourceMod's `sm_profiler` to identify performance bottlenecks
- Test with multiple players to identify scaling issues

## Common Pitfalls

### Physics Accuracy
- **Never modify velocity without considering basevelocity** - use `SetVelocity()` helper
- **Respect engine constraints** (NON_JUMP_VELOCITY, MIN_STANDABLE_ZNRM constants)
- **Preserve frame timing** - adjustments must account for tick duration
- **Collision prediction accuracy** - small errors can compound over time

### Code Maintenance  
- **ConVar changes require careful testing** - they affect competitive gameplay
- **Engine updates may break gamedata** - monitor CSS/CS:GO updates
- **DHooks compatibility** - ensure DHooks extension version compatibility
- **Cross-game compatibility** - test changes on both CSS and CS:GO

## Build and Release Process

### Local Development
1. Modify source files in `addons/sourcemod/scripting/`
2. Update `sourceknight.yaml` if adding new dependencies
3. Build with SourceKnight Docker container
4. Test compiled `.smx` files on development server

### CI/CD Pipeline
- **Automatic builds** on push/PR via GitHub Actions
- **Artifact generation** creates distributable packages
- **Release automation** tags and publishes releases
- **Multi-platform support** (currently Linux-focused)

### Version Management
- **Plugin version** defined in `myinfo` structure
- **Semantic versioning** (MAJOR.MINOR.PATCH)
- **Git tags** synchronized with plugin versions
- **Breaking changes** require major version increment

## Security and Compatibility

### Game Compatibility
- **Primary target**: Counter-Strike: Source (CSS)
- **Secondary target**: Counter-Strike: Global Offensive (CS:GO) 
- **Engine dependency**: Source Engine movement physics
- **SourceMod version**: 1.10+ required (ray tracing functionality)

### Extension Dependencies
- **DHooks**: Required for memory patching and function hooking
- **Movement Unlocker**: Optional for CS:GO sliding mechanics
- **Gamedata maintenance**: Keep `rngfix.games.txt` updated with engine changes

This plugin directly interfaces with low-level engine physics systems and requires careful consideration of any changes that might affect competitive gameplay balance or server performance.