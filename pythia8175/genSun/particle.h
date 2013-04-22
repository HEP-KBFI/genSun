#ifndef PARTICLE_H
#define PARTICLE_H

namespace SolarNu {
struct Particle {
    float energy;
    int id;
    int parent_id;

    Particle(float energy_, int id_, int parent_id_) {
        energy = energy_;
        id = id_;
        parent_id = parent_id_;
    }
};
}
#endif