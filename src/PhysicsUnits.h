// PhysicsUnits.h
#pragma once

#include <cmath>
#include <cstdint>

// The simulation advances in fixed ticks: each step adds velocity to position and
// acceleration to velocity, with the timestep folded directly into the stored values
// (see PhysicsEngine::run). A stored velocity is therefore displacement per tick and a
// stored acceleration is velocity change per tick.
//
// Tuning is written in SI units (seconds) and converted to per-tick units at the point
// of definition, so the configured behaviour stays fixed in real-world terms whatever the
// tick rate turns out to be. Instead of a raw per-tick number, write e.g.
// PhysicsUnits::metersPerSecond(5.12). All conversions route through s_tickRateHz, the
// single source of truth for the tick length.
namespace PhysicsUnits {

// Simulation rate (ticks per second). The loop runs one tick per rendered frame and vsync
// paces the frames, so this is the display's refresh rate: GraphicsEngine reads it from
// the monitor when the window is created and calls setTickRate once, before anything that
// holds converted tuning is built. The value here is the fallback for a display that does
// not report a rate. Nothing may convert tuning before that call.
inline double s_tickRateHz{ 120. };

// Sets the rate every conversion below is taken against. Call once, at startup.
inline void setTickRate(double tickRateHz) {
   s_tickRateHz = tickRateHz;
}

// Seconds -> ticks. A real-time duration expressed as a number of simulation steps.
inline double seconds(double timeSeconds) {
   return timeSeconds * s_tickRateHz;
}

// Seconds -> whole ticks, for a duration stored as a tick count.
inline int64_t ticks(double timeSeconds) {
   return (int64_t)std::llround(seconds(timeSeconds));
}

// m/s -> m/tick. A RigidBody velocity (displacement added to position each tick).
inline double metersPerSecond(double velocity) {
   return velocity / s_tickRateHz;
}

// m/tick -> m/s. The inverse of metersPerSecond, for reading a stored velocity back out
// in SI so a coefficient applied to it can be written in SI too.
inline double toMetersPerSecond(double velocity) {
   return velocity * s_tickRateHz;
}

// m/s^2 -> m/tick^2. An acceleration (velocity added to velocity each tick).
inline double metersPerSecondSquared(double acceleration) {
   return acceleration / (s_tickRateHz * s_tickRateHz);
}

// rad/s -> rad/tick. An angular velocity applied to orientation each tick.
inline double radiansPerSecond(double angularVelocity) {
   return angularVelocity / s_tickRateHz;
}

// rad/s^2 -> rad/tick^2. An angular acceleration, i.e. a torque on a unit inertia.
inline double radiansPerSecondSquared(double angularAcceleration) {
   return angularAcceleration / (s_tickRateHz * s_tickRateHz);
}

// 1/s -> 1/tick. A first-order rate: a linear drag coefficient, a control gain, or the
// probability of an event that is sampled once per tick.
inline double perSecond(double rate) {
   return rate / s_tickRateHz;
}

// Half-life (s) -> the per-tick factor that halves a value over that time, for a quantity
// damped as value *= halfLife(t). Its reciprocal doubles a value over the same time.
inline double halfLife(double halfLifeSeconds) {
   return std::pow(0.5, 1. / (halfLifeSeconds * s_tickRateHz));
}

// Half-life (s) -> the per-tick weight of a glm::mix that closes the remaining distance to
// its target in that time. This is 1. - halfLife(t), evaluated so that the long half-lives,
// whose weights are a hair above zero, keep their precision.
inline double blendHalfLife(double halfLifeSeconds) {
   return -std::expm1(std::log(0.5) / (halfLifeSeconds * s_tickRateHz));
}

}
