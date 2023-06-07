#pragma once

#define M_PI 3.14159265358979323846

#include <vector>
#include <unordered_map>
#include <memory>
#include <math.h>
#include <iostream>

#include <olb2D.h>
#include <olb2D.hh>
#include <olb3D.h>
#include <olb3D.hh>

#include <Module.h>
#include <Node.h>
#include <Channel.h>

namespace arch {

    // forward declared dependencies
    template<typename T>
    class Network;

    // Definition of opening struct
    template<typename T>
    struct Opening {
        std::shared_ptr<Node<T>> node;
        std::vector<T> normal;
        std::vector<T> tangent;
        T width;
        T height;

        Opening(std::shared_ptr<Node<T>> node_, std::vector<T> normal_, T width_, T height_=1e-4) :
            node(node_), normal(normal_), width(width_), height(height_) {
                // We rotate the normal vector 90 degrees counterclockwise to get the tangent
                T theta = 0.5*M_PI;
                tangent = { cos(theta)*normal_[0] - sin(theta)*normal_[1],
                            sin(theta)*normal_[0] + cos(theta)*normal_[1]};
            }
    };

    /**
     * @brief Class that defines the lbm module which is the interface between the 1D solver and OLB.
    */
    template<typename T>
    class lbmModule : public Module<T> {

        using DESCRIPTOR = olb::descriptors::D2Q9<>;
        using NoDynamics = olb::NoDynamics<T,DESCRIPTOR>;
        using BGKdynamics = olb::BGKdynamics<T,DESCRIPTOR>;
        using BounceBack = olb::BounceBack<T,DESCRIPTOR>;

        private:
            int stepIter = 100;                     ///< Number of iterations per communication step
            int maxIter = 1e5;                      
            int step = 0;
            std::unordered_map<int, T> pressures;   ///< Vector of pressure values at module nodes
            std::unordered_map<int, T> flowRates;   ///< Vector of flowRate values at module nodes
            std::unordered_map<int, T> resistances; ///< Vector of resistance values at module nodes 
            std::string name;
            std::string stlFile;
            bool initialized = false;
            bool isConverged = false;
            T charPhysLength;
            T charPhysVelocity;
            T alpha;
            T resolution;
            T epsilon;
            T relaxationTime;

            std::shared_ptr<const olb::UnitConverterFromResolutionAndRelaxationTime<T, DESCRIPTOR>> converter;   ///< Object that stores conversion factors from phyical to lattice parameters
            std::shared_ptr<olb::LoadBalancer<T>> loadBalancer;             ///< Loadbalancer for geometries in multiple cuboids
            std::shared_ptr<olb::CuboidGeometry<T,2>> cuboidGeometry;       ///< The geometry in a single cuboid
            std::shared_ptr<olb::SuperGeometry<T,2>> geometry;              ///< The final geometry of the channels
            std::shared_ptr<olb::SuperLattice<T, DESCRIPTOR>> lattice;      ///< The LBM lattice on the geometry
            std::unique_ptr<olb::util::ValueTracer<T>> converge;
            std::unordered_map<int, std::shared_ptr<olb::SuperPlaneIntegralFluxVelocity2D<T>>> fluxes;

            std::shared_ptr<Network<T>> moduleNetwork;

            std::unordered_map<int, Opening<T>> moduleOpenings;

            auto& getConverter() {
                return *converter;
            }

            auto& getGeometry() {
                return *geometry;
            }

            auto& getLattice() {
                return *lattice;
            }

        public:
            /**
             * @brief Constructor of an lbm module.
            */
            lbmModule(int id, std::string name, std::vector<T> pos, std::vector<T> size, std::unordered_map<int, std::shared_ptr<Node<T>>> nodes, 
                std::unordered_map<int, Opening<T>> openings, std::string stlFile, T charPhysLenth, T charPhysVelocity, T alpha, T resolution, T epsilon, T relaxationTime=0.932);

            /**
             * @brief Initialize an instance of the LBM solver for this module.
            */
            void lbmInit(T dynViscosity, T density);

            /**
             * @brief prepare the LBM geometry of this instance.
            */
            void prepareGeometry();

            /**
             * @brief prepare the LBM lattice on the LBM geometry.
            */
            void prepareLattice();

            /**
             * @brief set the boundary values on the lattice at the module nodes.
            */
            void setBoundaryValues(T iT);

            /**
             * @brief Conducts the collide and stream operations of the lattice.
            */
            void solve(int iteration);

            /**
             * @brief update the values at the module nodes based on the simulation result after stepIter iterations.
            */
            void getResults();

            /**
             * @brief Write the vtk file with results of the CFD simulation to file system.
            */
           void writeVTK(int iT);

            /**
             * @brief Set the pressure difference over a channel.
             * @param[in] Pressure in Pa.
             */
            void setPressures(std::unordered_map<int, T> pressure);

            /**
             * @brief Set resistance of a channel without droplets.
             * @param[in] channelResistance Resistance of a channel without droplets in Pas/L.
             */

            /**
             * @brief Add an opening of the CFD domain
             * @param[in]
            */
            void addOpening(int nodeId, std::vector<T> normal, T width, T height = -1);

            /**
             * @brief Calculates and returns pressure difference over a channel.
             * @returns Pressure in Pa.
             */
            std::unordered_map<int, T> getPressures() const;

            /**
             * @brief Calculate flow rate within the channel.
             * @returns Flow rate in m^3/s.
             */
            std::unordered_map<int, T> getFlowRates() const;

            /**
             * @brief Returns resistance caused by the channel itself.
             * @returns Resistance caused by the channel itself in Pas/L.
             */
            std::unordered_map<int, T> getResistances() const;

            /**
             * @brief Returns resistance caused by the channel itself.
             * @returns Resistance caused by the channel itself in Pas/L.
             */
            std::unordered_map<int, Opening<T>> getOpenings() const {
                return moduleOpenings;
             };

            /**
             * @brief Returns the iterations per CFD step to solve.
             * @returns Iterations per step.
            */
            int getStepIter() const;

            bool getInitialized() const { 
                return initialized; 
            };

            bool hasConverged() const {
                return converge->hasConverged();
            };

            void setInitialized(bool initialization) {
                this->initialized = initialization;
            };

            std::shared_ptr<Network<T>> getNetwork() const {
                return moduleNetwork;
            }

            T getCharPhysLength() const { 
                return charPhysLength; 
            };

            T getCharPhysVelocity() const { 
                return charPhysVelocity; 
            };

            T getAlpha() const { 
                return alpha; 
            };

            T getResolution() const { 
                return resolution; 
            };

            T getEpsilon() const { 
                return epsilon; 
            };
    };
}